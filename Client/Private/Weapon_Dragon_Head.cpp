#include "Weapon_Dragon_Head.h"
#include "GameInstance.h"

CWeapon_Dragon_Head::CWeapon_Dragon_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Dragon_Head::CWeapon_Dragon_Head(const CWeapon_Dragon_Head& rhs)
	: CParts(rhs)
{
}

CWeapon_Dragon_Head* CWeapon_Dragon_Head::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Dragon_Head* pInstance = New CWeapon_Dragon_Head(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Dragon_Head");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Dragon_Head* CWeapon_Dragon_Head::Clone(void* pArg)
{
	CWeapon_Dragon_Head* pInstance = New CWeapon_Dragon_Head(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Dragon_Head");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Dragon_Head::Free()
{
	__super::Free();
}
