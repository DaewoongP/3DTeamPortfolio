#include "Status_Ignite.h"

CStatus_Ignite::CStatus_Ignite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CStatus_Ignite::CStatus_Ignite(const CStatus_Ignite& rhs)
	: CComponent(rhs)
{
}

HRESULT CStatus_Ignite::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		STATUSIGNITEDESC* pStatusIgniteDesc = static_cast<STATUSIGNITEDESC*>(pArg);
		m_fDuration = pStatusIgniteDesc->fDuration;
	}

	return S_OK;
}

void CStatus_Ignite::Tick(_float fTimeDelta)
{

}

CStatus_Ignite* CStatus_Ignite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus_Ignite* pInstance = new CStatus_Ignite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStatus_Ignite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStatus_Ignite* CStatus_Ignite::Clone(void* pArg)
{
	CStatus_Ignite* pInstance = new CStatus_Ignite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStatus_Ignite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus_Ignite::Free()
{
	__super::Free();
}
