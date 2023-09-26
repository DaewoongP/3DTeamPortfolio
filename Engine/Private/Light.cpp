#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_char szPassName[MAX_STR] = "";

	if (TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		
		if (FAILED(pShader->Bind_RawValue("g_vCamPosition", &m_LightDesc.vDir, sizeof(_float4))))
				return E_FAIL;

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
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fSpotPower", &m_LightDesc.fSpotPower, sizeof(_float))))
			return E_FAIL;

		strcpy_s(szPassName, sizeof(_char) * MAX_STR, "Light_Spotlight");
	}
	else if (TYPE_LUMOS == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		

		strcpy_s(szPassName, sizeof(_char) * MAX_STR, "Light_Point");
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

CLight * CLight::Create(const LIGHTDESC & LightDesc)
{
	CLight*	pInstance = New CLight();

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}
