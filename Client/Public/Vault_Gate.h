#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CMesh;
class CModel;
class CShader;
class CRenderer;
class CParticleSystem;
END

BEGIN(Client)

class CLightStand;

class CVault_Gate final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

private:
	explicit CVault_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CVault_Gate(const CVault_Gate& rhs);
	virtual ~CVault_Gate() = default;

public:
	_bool Get_GateOpen() const { return m_isCheckOnce; }
	_bool Get_All_LightStands_On() const { return m_isAllLightStandsOn; } // 컷신을 위해 한 단계 추가
	void Set_Gate_On() { m_isGateOpen = true; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	CParticleSystem* m_pEffect = { nullptr };

private:
	// 절두체 컬링을 위해 Bounding Box를 생성 하기위한 최소, 최대 정점
	_float3			m_vMinPoint, m_vMaxPoint, m_vCenterPoint;
	_float			m_fRadius = { 0.f };

	_bool			m_isCheckOnce = { false }; // 한번만 상호작용 가능
	_bool			m_isAllLightStandsOn = { false };
	_bool			m_isGateOpen = { false };

	_bool			m_isSound0 = { true }; // 문 열리는 소리
	_uint			m_iSound0 = { 0 };	   // 문 열리는 소리 인덱스

	_bool			m_isSound1 = { true }; // 문 닫히고 쿵 소리
	
	vector<CLightStand*> m_pLightStands; // 맵에 존재하는 화로들

private:
	MAPOBJECTDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_MinMaxPoint(_float3 vPoint);
	void	Check_FireOn();

public:
	static CVault_Gate* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END