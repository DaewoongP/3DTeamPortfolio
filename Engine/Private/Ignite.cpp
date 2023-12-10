#include "Ignite.h"

CIgnite::CIgnite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CIgnite::CIgnite(const CIgnite& rhs)
	: CComponent(rhs)
{
}

HRESULT CIgnite::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		STATUSIGNITEDESC* pStatusIgniteDesc = static_cast<STATUSIGNITEDESC*>(pArg);
		m_fDuration = pStatusIgniteDesc->fDuration;
	}

	return S_OK;
}

void CIgnite::Tick(_float fTimeDelta)
{

}

CIgnite* CIgnite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIgnite* pInstance = New CIgnite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CIgnite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CIgnite* CIgnite::Clone(void* pArg)
{
	CIgnite* pInstance = New CIgnite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CIgnite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIgnite::Free()
{
	__super::Free();
}
