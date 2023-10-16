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
class CPlayer_Information;

class CGatherer final : public CGameObject
{
public: 
	// 순서대로 애쉬와인드 알, 호클럼프 즙, 독버섯 갓, 거머리 즙 
	enum GATHERING { ASHWINDEREGG, HORKLUMP, LEAPINGTOADSTOOLS, LEECH, GATHERING_END };

public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

private:
	explicit CGatherer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGatherer(const CGatherer& rhs);
	virtual ~CGatherer() = default;

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

	CPlayer*			 m_pPlayer = { nullptr };			 // 플레이어 주소
	CPlayer_Information* m_pPlayerInformation = { nullptr }; // 플레이어 인벤토리와 상호작용하기 위한 주소

private:
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };

	_bool			m_isGetItem = { true }; // 아이템 획득 가능 여부
	_bool			m_isCol_with_Player = { false }; // 플레이어와 충돌 여부

	_uint			m_iCurrentLevel = { LEVEL_END };

private:
	MAPOBJECTDESC	m_ObjectDesc;
	GATHERING		m_GatheringType = { CGatherer::GATHERING_END }; // 채집물 종류

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_MinMaxPoint(_float3 vPoint);

public:
	static CGatherer* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END