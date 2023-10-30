#include "..\Public\Light_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

_uint CLight_Manager::Get_CurrentLightShadowNum()
{
	_uint iNum = { 0 };
	for (auto isRender : m_isShadowRender)
	{
		if (true == isRender)
			++iNum;
	}

	return iNum;
}

const CLight::LIGHTDESC* CLight_Manager::Get_ShadowLightDesc(_uint iIndex)
{
	if (nullptr == m_pShadowLights[iIndex])
		return nullptr;

	return m_pShadowLights[iIndex]->Get_LightDesc();
}

HRESULT CLight_Manager::Reserve_Lights(_uint iNumReserve)
{
	for (_uint i = 0; i < iNumReserve; ++i)
	{
		m_LightPool.push(CLight::Create(CLight::LIGHTDESC()));
	}
	
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const CLight::LIGHTDESC& LightDesc, _Inout_ class CLight** ppLight, _bool isShadow, _uint iLightViewIndex, _float fAspect)
{
	CLight* pLight = Create_Light(LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	if (true == isShadow)
	{
		m_LightViewMatrix[iLightViewIndex] = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_LightProjMatrix[iLightViewIndex] = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fAspect, 0.1f, 1000.f);
		m_fAspect = fAspect;
		m_isShadowRender[iLightViewIndex] = true;
		m_pShadowLights[iLightViewIndex] = pLight;
		Safe_AddRef(pLight);
	}
	
	m_Lights.push_back(pLight);

	if (nullptr != ppLight)
	{
		*ppLight = pLight;
		Safe_AddRef(pLight);
	}

	return S_OK;
}

HRESULT CLight_Manager::Add_InstanceLight(_float3 vPos, _float fStartRange, _float fTime, _float4 vColor, _bool isIncrease, _float fIncreasePower)
{
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = fStartRange;
	LightDesc.fDecreaseStartRange = LightDesc.fRange;
	LightDesc.vDiffuse = vColor;
	LightDesc.vAmbient = vColor;
	LightDesc.vSpecular = vColor;
	LightDesc.vPos = vPos.TransCoord();
	LightDesc.isIncrease = isIncrease;
	LightDesc.fIncreasePower = fIncreasePower;
	LightDesc.fTime = fTime;

	CLight* pLight = Create_Light(LightDesc);

	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);
	m_InstanceLights.push_back(pLight);
	Safe_AddRef(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Clear_Lights()
{
	for (_uint i = 0; i < MAX_SHADOW; ++i)
	{
		m_isShadowRender[i] = false;
		Safe_Release(m_pShadowLights[i]);
	}

	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
	
	for (auto& pLight : m_InstanceLights)
		Safe_Release(pLight);

	m_InstanceLights.clear();

	return S_OK;
}

void CLight_Manager::Tick(_float fTimeDelta)
{
	for (auto iter = m_InstanceLights.begin(); iter != m_InstanceLights.end();)
	{
		if (nullptr != (*iter))
		{
			if (true == (*iter)->Is_Increase())
			{
				if (false == (*iter)->Tick_Increase(fTimeDelta))
				{
					Return_Light(*iter);
					iter = m_InstanceLights.erase(iter);
					continue;
				}
			}
			else
			{
				if (false == (*iter)->Tick_Decrease(fTimeDelta))
				{
					Return_Light(*iter);
					iter = m_InstanceLights.erase(iter);
					continue;
				}
			}

			++iter;
		}
	}
}

HRESULT CLight_Manager::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
	{
		if (nullptr != pLight)
			pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

HRESULT CLight_Manager::Return_Light(CLight* pLight)
{
	if (nullptr == pLight)
		return S_OK;

	auto iter = find_if(m_Lights.begin(), m_Lights.end(), [&](auto value) {
		if (value == pLight)
			return true;
		return false;
		});

	if (m_Lights.end() != iter)
	{
		m_Lights.erase(iter);
		m_LightPool.push(pLight);
	}

	return S_OK;
}

HRESULT CLight_Manager::Update_ShadowMatrix(_uint iShadowIndex, CLight::LIGHTDESC LightDesc)
{
	m_LightViewMatrix[iShadowIndex] = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
	m_LightProjMatrix[iShadowIndex] = XMMatrixPerspectiveFovLH(XMConvertToRadians(110.f), m_fAspect, 0.1f, 1000.f);
	
	return S_OK;
}

CLight* CLight_Manager::Create_Light(const CLight::LIGHTDESC& LightDesc)
{
	CLight* pLight = { nullptr };

	if (m_LightPool.empty())
	{
		pLight = CLight::Create(LightDesc);
	}
	else
	{
		pLight = m_LightPool.front();
		pLight->Set_LightDesc(LightDesc);
		m_LightPool.pop();
	}
	
	return pLight;
}

void CLight_Manager::Free()
{
	Clear_Lights();

	while (!m_LightPool.empty())
	{
		Safe_Release(m_LightPool.front());
		m_LightPool.pop();
	}
}