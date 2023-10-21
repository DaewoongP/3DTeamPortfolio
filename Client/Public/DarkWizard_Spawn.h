#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CDarkWizard_Spawn final : public CGameObject
{
private:
	explicit CDarkWizard_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDarkWizard_Spawn(const CDarkWizard_Spawn& rhs);
	virtual ~CDarkWizard_Spawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

#ifdef _DEBUG
private:
	_bool m_isCheckPosition = { false };
	_float3 m_vAngle;
	void Tick_TestShake();
#endif // _DEBUG

private:
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CDarkWizard_Spawn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END