#include "..\Public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

void CLight::Set_LightDesc(LIGHTDESC LightDesc)
{
	m_LightDesc = LightDesc;
}

void CLight::Set_Position(_float4 vPosition)
{
	m_LightDesc.vPos = vPosition;
}

void CLight::Set_Range(_float fRange)
{
	m_LightDesc.fRange = fRange;
}

HRESULT CLight::Initialize(const LIGHTDESC& LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

_bool CLight::Tick_Increase(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc > m_LightDesc.fTime)
		return false;
	
	m_LightDesc.fRange += m_LightDesc.fIncreasePower * fTimeDelta;

	return true;
}

_bool CLight::Tick_Decrease(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc > m_LightDesc.fTime)
	{
		m_fTimeAcc = 0.f;
		return false;
	}
	
	// Delta Range / Total Time - ����
	m_LightDesc.fRange -= m_LightDesc.fDecreaseStartRange * fTimeDelta / m_LightDesc.fTime;

	return true;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
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
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPos, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDir, sizeof(_float4))))
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

CLight* CLight::Create(const LIGHTDESC& LightDesc)
{
	CLight* pInstance = New CLight();

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