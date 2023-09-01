#include "Status.h"

CStatus::CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus& rhs)
	: CComposite(rhs)
{
}

HRESULT CStatus::Initialize(void* pArg)
{
	return S_OK;
}

void CStatus::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CStatus::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

CStatus* CStatus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatus* pInstance = new CStatus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStatus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CStatus* CStatus::Clone(void* pArg)
{
	CStatus* pInstance = new CStatus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStatus");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatus::Free()
{
	__super::Free();
}
