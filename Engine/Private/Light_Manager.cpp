#include "..\Public\Light_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight* CLight_Manager::Add_Lights(const CLight::LIGHTDESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);

	if (nullptr == pLight)
		return nullptr;

	if (CLight::TYPE_DIRECTIONAL == LightDesc.eType)
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

HRESULT CLight_Manager::Clear_Lights()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	return S_OK;
}

void CLight_Manager::Free()
{
	Clear_Lights();
}
