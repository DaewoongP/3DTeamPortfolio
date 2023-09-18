#include "Attack.h"

CAttack::CAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CAttack::CAttack(const CAttack& rhs)
	: CComponent(rhs)
{
}

HRESULT CAttack::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		ATTACKDESC Desc = *static_cast<ATTACKDESC*>(pArg);

		m_iDamage = Desc.iDamage;
	}

	return S_OK;
}

CAttack* CAttack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttack* pInstance = New CAttack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAttack* CAttack::Clone(void* pArg)
{
	CAttack* pInstance = New CAttack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAttack::Free()
{
	__super::Free();
}
