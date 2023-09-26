#include "..\Public\Light_Manager.h"
#include"Transform.h"
#include"PipeLine.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const CLight::LIGHTDESC* CLight_Manager::Get_Light(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

void CLight_Manager::Set_Light(_uint iIndex, CLight::LIGHTDESC LightDesc)
{
	auto	iter = m_Lights.begin();

	for (size_t i = 0; i < iIndex; ++i)
		++iter;
	
	if (LightDesc.eType == CLight::TYPE_LUMOS)
	{
		_uint				iNumViews = { 1 };
		D3D11_VIEWPORT		ViewportDesc;

		m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

		m_ViewLight = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), ViewportDesc.Width / ViewportDesc.Height, 1.f, 1000.f);
	}


	(*iter)->Set_LightDesc(LightDesc);
}

CLight* CLight_Manager::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	CLight* pLight = CLight::Create(LightDesc);

	CTransform* pTransform = CTransform::Create(pDevice, pContext);

	_uint				iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	if (LightDesc.eType == CLight::TYPE_DIRECTIONAL)
	{
		m_ViewLight = XMMatrixLookAtLH(LightDesc.vPos, LightDesc.vLookAt, _float4(0.f, 1.f, 0.f, 0.f));
		m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), ViewportDesc.Width / ViewportDesc.Height, 1.f, 1000.f);
	}

	//m_ViewLight = pTransform->Get_WorldMatrix_Inverse();
	Safe_Release(pTransform);

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
