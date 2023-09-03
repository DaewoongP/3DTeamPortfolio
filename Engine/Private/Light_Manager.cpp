#include "..\Public\Light_Manager.h"
#include"Transform.h"
#include"PipeLine.h"
IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const CLight::LIGHTDESC * CLight_Manager::Get_Light(_uint iIndex)
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

	(*iter)->Set_LightDesc(LightDesc);
}

const CLight::LIGHTDESC* CLight_Manager::Get_Light_Name(string Name)
{
	return nullptr;
}

CLight* CLight_Manager::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC & LightDesc)
{
	CLight*		pLight = CLight::Create(LightDesc);

	
	CTransform* pTransform = CTransform::Create(pDevice, pContext);
	pTransform->Set_Position(_float3(LightDesc.vPos.x, LightDesc.vPos.y, LightDesc.vPos.z));
	pTransform->Set_Look(_float3(LightDesc.vDir.x, LightDesc.vDir.y, LightDesc.vDir.z));
	m_ViewLight = pTransform->Get_WorldMatrix_Inverse();
	Safe_Release(pTransform);
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);
	if(0.f<*pPipeLine->Get_CamFar())
	XMStoreFloat4x4(&m_ProjLight, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1280.f / 720.f, 0.2f, *pPipeLine->Get_CamFar()));
	Safe_Release(pPipeLine);



		if (nullptr == pLight)
		return nullptr;

	m_Lights.push_back(pLight);

	return pLight;
}

HRESULT CLight_Manager::Delete_Lights(_uint iIndex,const _char* Name)
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
