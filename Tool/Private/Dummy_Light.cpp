#include "Dummy_Light.h"
#include"GameInstance.h"

CDummy_Light::CDummy_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CGameObject(pDevice,pContext)
{
}

CDummy_Light::CDummy_Light(const CDummy_Light& rhs) : CGameObject(rhs)
{
}

HRESULT CDummy_Light::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_Light::Initialize(LIGHTDESC& LightDesc)
{
	if (FAILED(__super::Initialize(LightDesc)))
		return E_FAIL;

	return S_OK;

}

void CDummy_Light::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CDummy_Light::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDummy_Light::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_char szPassName[MAX_STR] = "";

	if (TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof(_float4))))
			return E_FAIL;

		strcpy_s(szPassName, sizeof(_char) * MAX_STR, "Light_Directional");
	}
	else if (TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		strcpy_s(szPassName, sizeof(_char) * MAX_STR, "Light_Point");
	}
	else if (TYPE_SPOTLIGHT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fSpotPower", &m_LightDesc.fSpotPower, sizeof(_float))))
			return E_FAIL;

		strcpy_s(szPassName, sizeof(_char) * MAX_STR, "Light_Spotlight");
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(pShader->Begin(szPassName)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}



HRESULT CDummy_Light::Add_Components()
{
	return S_OK;
}

HRESULT CDummy_Light::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

CDummy_Light* CDummy_Light::Create(const LIGHTDESC& LightDesc)
{
	CDummy_Light* pInstance = New CDummy_Light();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Cloned CDummy_Light");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDummy_Light::Free()
{
	__super::Free();
	Safe_Release(m_pShader);
}
