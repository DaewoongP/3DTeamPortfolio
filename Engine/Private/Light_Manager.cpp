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

	(*iter)->Set_LightDesc(LightDesc);

	CTransform* pTransform = CTransform::Create(m_pDevice, m_pContext);
	pTransform->Set_Position(_float3(LightDesc.vPos.x, LightDesc.vPos.y, LightDesc.vPos.z));
	pTransform->LookAt(_float3(LightDesc.vDir.x, LightDesc.vDir.y, LightDesc.vDir.z));

	m_ViewLight = XMMatrixLookAtLH(_float4(-5.f, 15.f, -5.f, 1.f), _float4(3.f, 0.f, 3.f, 1.f), _float4(0.f, 1.f, 0.f, 0.f));
	m_ProjLight = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), _float(1280) / 720.f, 1.f, 100.f);
	//m_ViewLight = pTransform->Get_WorldMatrix_Inverse();
	Safe_Release(pTransform);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

		//m_ProjLight=XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1280.f / 720.f, 0.2f,100.f);
	Safe_Release(pPipeLine);
}

const CLight::LIGHTDESC* CLight_Manager::Get_Light_Name(string Name)
{
	return nullptr;
}

CLight* CLight_Manager::Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CLight::LIGHTDESC& LightDesc)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	CLight* pLight = CLight::Create(LightDesc);


	CTransform* pTransform = CTransform::Create(pDevice, pContext);
	if (LightDesc.eType == CLight::TYPE_DIRECTIONAL)
	{
		pTransform->Set_Position(_float3(30.f, 50.f, 30.f));
		//pTransform->LookAt(_float3(0.34f, 0.98f, 0.34f));
		pTransform->LookAt(_float3(0.34f, 0.98f, 0.34f));

	}
	else
	{
		pTransform->Set_Position(_float3(LightDesc.vPos.x, LightDesc.vPos.y, LightDesc.vPos.z));
		pTransform->LookAt(_float3(LightDesc.vDir.x, LightDesc.vDir.y, LightDesc.vDir.z));

	}
	m_ViewLight = pTransform->Get_WorldMatrix_Inverse();
	Safe_Release(pTransform);

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

		

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
	//XMStoreFloat4x4(&m_ProjLight, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1280.f / 720.f, 0.1f,100.f));
	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
