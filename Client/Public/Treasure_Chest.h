#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
class CMeshEffect;
class CParticleSystem;
END

BEGIN(Client)

class CPlayer;
class CPlayer_Information;

class CUI_Interaction;

class CTreasure_Chest final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

private:
	explicit CTreasure_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTreasure_Chest(const CTreasure_Chest& rhs);
	virtual ~CTreasure_Chest() = default;

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
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CParticleSystem* m_pEffect0 = { nullptr };
	CParticleSystem* m_pEffect1 = { nullptr };
	CMeshEffect* m_pEffect2 = { nullptr };

	CPlayer* m_pPlayer = { nullptr }; // 플레이어 주소
	CPlayer_Information* m_pPlayerInformation = { nullptr }; // 플레이어 인벤토리와 상호작용하기 위한 주소

	CUI_Interaction* m_pUI_Interaction = { nullptr };

private:
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };

	_bool			m_isGetItem = { true }; // 아이템 획득 가능 여부
	_bool			m_isCol_with_Player = { false }; // 플레이어와 충돌 여부

	_float4x4		m_UIMatirx;

private:
	MAPOBJECTDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_MinMaxPoint(_float3 vPoint);

public:
	static CTreasure_Chest* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END