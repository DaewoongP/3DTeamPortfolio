#include "..\Public\Light_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

HRESULT CLight_Manager::Reserve_Lights(_uint iNumLights)
{
	for (_uint i = 0; i < iNumLights; ++i)
	{
		m_LightPool.push(CLight::Create(CLight::LIGHTDESC()));
	}

	return S_OK;
}

CLight* CLight_Manager::Add_Lights(const CLight::LIGHTDESC& LightDesc, _bool isShadow)
{
	CLight* pLight = { nullptr };
	if (m_LightPool.empty())
	{
		pLight = CLight::Create(LightDesc);
	}
	else
	{
		pLight = m_LightPool.front();
		m_LightPool.pop();
		pLight->Set_LightDesc(LightDesc);
	}

	if (true == isShadow)
	{
		CPipeLine* pPipeLine = CPipeLine::GetInstance();
		Safe_AddRef(pPipeLine);
		pPipeLine->Set_LightTransform(CPipeLine::D3DTS_VIEW, XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vPos + LightDesc.vDir, _float3(0.f, 1.f, 0.f)));
		pPipeLine->Set_LightTransform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1280.f / 720.f, 0.1f, 1000.f));
		Safe_Release(pPipeLine);
	}

	m_Lights.push_back(pLight);

	return pLight;
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

HRESULT CLight_Manager::Delete_Light(CLight* pLight)
{
	auto iter = find_if(m_Lights.begin(), m_Lights.end(), [&](auto& pSourLight) {
		if (pSourLight == pLight)
		{
			return true;
		}
		return false;
		}
	);

	if (m_Lights.end() == iter)
		return E_FAIL;

	m_Lights.erase(iter);
	m_LightPool.push(*iter);

	return S_OK;
}

HRESULT CLight_Manager::Clear_Lights()
{
	for (auto& pLight : m_Lights)
	{
		m_LightPool.push(pLight);
	}

	m_Lights.clear();

	return S_OK;
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
