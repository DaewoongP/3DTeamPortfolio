#include "Defence.h"

CDefence::CDefence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CDefence::CDefence(const CDefence& rhs)
	: CComponent(rhs)
{
}

HRESULT CDefence::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		DEFFENCEDESC Desc = *static_cast<DEFFENCEDESC*>(pArg);
		m_iDeffence = Desc.iDeffence;
	}

	return S_OK;
}

CDefence* CDefence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDefence* pInstance = new CDefence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDefence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDefence* CDefence::Clone(void* pArg)
{
	CDefence* pInstance = new CDefence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDefence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefence::Free()
{
	__super::Free();
}
