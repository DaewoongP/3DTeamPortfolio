#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CPlayer;

class CGull final : public CGameObject
{
	// GULL_IDLE1은 먹이를 먹는 모션
	// GULL_IDLE2는 주변을 두리번 거리는 모션
	enum GULL_ANIMATION {
		GULL_ALERT, GULL_FLY, GULL_IDLE1, GULL_IDLE2,
		GULL_START_FLY, GULL_START_FLY_REVERSE, GULL_END
	};

public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

private:
	explicit CGull(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGull(const CGull& rhs);
	virtual ~CGull() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

	CPlayer* m_pPlayer = { nullptr };			 // 플레이어 주소

private:
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float3			m_vecOriginPos = { 0.f, 0.f, 0.f };
	_float			m_fRadius = { 0.f };
	_float			m_fDist_From_Player = { 0.f }; // 갈매기와 플레이어와의 거리
	_float			m_fDeadTime = { 0.f }; // 갈매기 죽는 시간
	_float			m_fFlyTime = { 0.f };  // v편대 비행 시간

	_float			m_fRandTime = { 0.f }; // v편대 랜덤 애니메이션 타이밍
	_float			m_fRandTimeAcc = { 0.f }; // v편대 랜덤 애니메이션 타이밍 누적값
	
	_uint			m_iGullIndex = { 0 };	// 갈매기 인덱스 

	_bool			m_isCol_with_Player = { false }; // 플레이어와 충돌 여부
	_bool			m_isSound = { true };

private:
	MAPOBJECTDESC	m_ObjectDesc;
	GULL_ANIMATION  m_GullAnimIndex = { GULL_END };
	GULL_ANIMATION  m_PreGullAnimIndex = { m_GullAnimIndex }; // 이전 애니메이션 인덱스

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_Dist_From_Player(_float fTimeDelta);
	void	Fly_V_Squadron(_float fTimeDelta);

public:
	static CGull* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END