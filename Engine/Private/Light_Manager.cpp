#include "..\Public\Light_Manager.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

//void CLight_Manager::Set_Light(_uint iIndex, _float fWinSizeX, _float fWinSizeY, CLight::LIGHTDESC LightDesc)
//{
//	/*auto	iter = m_Lights.begin();
//
//	for (size_t i = 0; i < iIndex; ++i)
//		++iter;
//
//	if (LightDesc.eType == CLight::TYPE_LUMOS)
//	{
//		m_ViewLight = XMMatrixLookAtLH(Get_Light(CLight::TYPE_LUMOS)->vPos, Get_Light(CLight::TYPE_LUMOS)->vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
//		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fWinSizeX / fWinSizeY, 0.1f, 1000.f);
//	}
//	else
//	{
//		m_ViewLight = XMMatrixLookAtLH(Get_Light(CLight::TYPE_DIRECTIONAL)->vPos, Get_Light(CLight::TYPE_DIRECTIONAL)->vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
//		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fWinSizeX / fWinSizeY, 0.1f, 1000.f);
//	}
//
//	CPipeLine* pPipeLine = CPipeLine::GetInstance();
//	Safe_AddRef(pPipeLine);
//
//	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_VIEW, m_ViewLight);
//	pPipeLine->Set_LightTransform(CPipeLine::D3DTS_PROJ, m_ProjLight);
//
//	Safe_Release(pPipeLine);
//
//	(*iter)->Set_LightDesc(LightDesc);*/
//}

HRESULT CLight_Manager::Reserve_Lights(_uint iNumReserve)
{
	for (_uint i = 0; i < iNumReserve; ++i)
	{
		m_LightPool.push(CLight::Create(CLight::LIGHTDESC()));
	}
	
	return S_OK;
}

CLight* CLight_Manager::Add_Lights(const CLight::LIGHTDESC& LightDesc, _bool isShadow, _uint iLightViewIndex, _float fAspect)
{
	CLight* pLight = Create_Light(LightDesc);

	if (true == isShadow)
	{
		m_LightViewMatrix[iLightViewIndex] = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_LightProjMatrix[iLightViewIndex] = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), fAspect, 0.1f, 1000.f);
		++m_iBindedLightMatrices;
	}

	if (MAX_SHADOW < m_iBindedLightMatrices)
	{
		// 그림자를 위한 빛이 두개이상 바인딩 되어 행렬이 꼬인상태
		MSG_BOX("Shadow Option is Already Binded");
		__debugbreak();
	}
	
	if (nullptr == pLight)
		return nullptr;

	m_Lights.push_back(pLight);

	return pLight;
}

HRESULT CLight_Manager::Clear_Lights()
{
	m_iBindedLightMatrices = 0;

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

CLight* CLight_Manager::Create_Light(const CLight::LIGHTDESC& LightDesc)
{
	if (m_LightPool.empty())
	{
		return CLight::Create(LightDesc);
	}
	
	CLight* pLight = m_LightPool.front();
	pLight->Set_LightDesc(LightDesc);
	m_LightPool.pop();

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