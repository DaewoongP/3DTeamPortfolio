#include "Boots_Arcane.h"
#include "Inventory.h"
#include "Player.h"

CBoots_Arcane::CBoots_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CBoots_Arcane::CBoots_Arcane(const CBoots_Arcane& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CBoots_Arcane::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_BOOTS_ARCANE;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("아케인 부츠");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_096_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoots_Arcane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CBoots_Arcane::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_BOOTS_ARCANE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CBoots_Arcane::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Boots_Arcane")
		, CCustomModel::MESHTYPE::SHOES);
}

CBoots_Arcane* CBoots_Arcane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBoots_Arcane* pInstance = New CBoots_Arcane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBoots_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoots_Arcane* CBoots_Arcane::Clone(void* pArg)
{
	CBoots_Arcane* pInstance = New CBoots_Arcane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CBoots_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoots_Arcane::Free(void)
{
	__super::Free();
}
