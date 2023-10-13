#include "Gear_Item.h"
#include "GameInstance.h"

CGear_Item::CGear_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem(pDevice, pContext)
{
}

CGear_Item::CGear_Item(const CGear_Item& rhs)
	: CItem(rhs)
	, m_Gear_ItemCreateDesc(rhs.m_Gear_ItemCreateDesc)
{
}

HRESULT CGear_Item::Initialize_Prototype(_uint iLevel)
{
	m_ItemCreateDesc.eItemType = RESOURCE;

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGear_Item::Initialize(void* pArg)
{
	//m_eGear_Item = ((CLONE_DESC*)pArg)->m_eGear_Item;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CGear_Item::Free(void)
{
	__super::Free();
}
