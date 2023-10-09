#include "..\Public\Light_Manager.h"
#include "Transform.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const CLight::LIGHTDESC* CLight_Manager::Get_Light(_uint iIndex)
{
	if (0 == m_Lights.size())
		return nullptr;

	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

void CLight_Manager::Set_Light(_uint iIndex, _float fWinSizeX, _float fWinSizeY, CLight::LIGHTDESC LightDesc)
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	if (LightDesc.eType == CLight::TYPE_LUMOS)
	{
		m_ViewLight = XMMatrixLookAtLH(Get_Light(CLight::TYPE_LUMOS)->vPos, Get_Light(CLight::TYPE_LUMOS)->vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fWinSizeX / fWinSizeY, 0.1f, 1000.f);
	}
	else
	{
		m_ViewLight = XMMatrixLookAtLH(Get_Light(CLight::TYPE_DIRECTIONAL)->vPos, Get_Light(CLight::TYPE_DIRECTIONAL)->vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fWinSizeX / fWinSizeY, 0.1f, 1000.f);
	}

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_VIEW, m_ViewLight);
	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_PROJ, m_ProjLight);

	Safe_Release(pPipeLine);

	(*iter)->Set_LightDesc(LightDesc);
}

CLight* CLight_Manager::Add_Lights(_float fWinSizeX, _float fWinSizeY, const CLight::LIGHTDESC& LightDesc)
{
	CLight* pLight = CLight::Create(LightDesc);

	if (LightDesc.eType == CLight::TYPE_DIRECTIONAL)
	{
		m_ViewLight = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fWinSizeX / fWinSizeY, 0.1f, 1000.f);
	}

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_VIEW, m_ViewLight);
	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_PROJ, m_ProjLight);

	Safe_Release(pPipeLine);
	m_fLightPos = LightDesc.vPos;
	if (nullptr == pLight)
		return nullptr;

	m_Lights.push_back(pLight);

	return pLight;
}

HRESULT CLight_Manager::Delete_Lights(_uint iIndex, const _char* Name)
{
	auto iter = m_Lights.begin();
	for (size_t i = 0; i < iIndex; ++i)
		++iter;
	Safe_Release(*iter);
	m_Lights.erase(iter);

	return S_OK;
}

HRESULT CLight_Manager::Clear_Lights()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();

	return S_OK;
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

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}