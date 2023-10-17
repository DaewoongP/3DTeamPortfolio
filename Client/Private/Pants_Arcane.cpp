#include "Pants_Arcane.h"
#include "Inventory.h"
#include "Player.h"

CPants_Arcane::CPants_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CPants_Arcane::CPants_Arcane(const CPants_Arcane& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CPants_Arcane::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_PANTS_ARCANE;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("아케인 바지");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_076_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPants_Arcane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CPants_Arcane::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_PANTS_ARCANE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CPants_Arcane::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Pants_Arcane")
		, CCustomModel::MESHTYPE::PANTS);
}

CPants_Arcane* CPants_Arcane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CPants_Arcane* pInstance = New CPants_Arcane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPants_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPants_Arcane* CPants_Arcane::Clone(void* pArg)
{
	CPants_Arcane* pInstance = New CPants_Arcane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CPants_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPants_Arcane::Free(void)
{
	__super::Free();
}
