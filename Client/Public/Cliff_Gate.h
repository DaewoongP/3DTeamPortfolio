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

class CCliff_Gate final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc
	{
		_float4x4 WorldMatrix; // 상태 행렬
		_uint iTagLen = { 0 }; // 문자열 길이
		_tchar wszTag[MAX_PATH] = TEXT(""); // 오브젝트 종류(모델 컴포넌트 이름)
	}MAPOBJECTDESC;

private:
	explicit CCliff_Gate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCliff_Gate(const CCliff_Gate& rhs);
	virtual ~CCliff_Gate() = default;

public:
	_bool Get_GateOpen() const { return m_isEffectOn; } // 포탈 이펙트가 나타나면 게이트가 활성화된 것으로 간주
	_bool Get_All_LightStands_On() const { return m_isAllLightStandsOn; } // 화로가 다 점화되면 true 리턴
	void  Set_Gate_Reparo_On() { m_isReparoOn = true; } // 컷신을 위해서 한단계 추가

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
	_bool			m_isCheckOnce = { true }; // 한번만 상호작용 가능
	_bool			m_isEffectOn = { false }; // 이펙트 한 번만 켜주기
	_bool			m_isAllLightStandsOn = { false }; // 화로가 전부 점화되었는지
	_bool			m_isReparoOn = { false };	// 컷신을 위해서 한단계 추가

	_uint			m_iLightStandsCnt = 0;  // 화로의 개수

	vector<CLightStand*> m_pLightStands; // 맵에 존재하는 화로들

private:
	MAPOBJECTDESC	m_ObjectDesc;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);
	void	Check_FireOn();

public:
	static CCliff_Gate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END