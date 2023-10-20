#include "Outfit_Arcane_A.h"
#include "Inventory.h"
#include "Player.h"

COutfit_Arcane_A::COutfit_Arcane_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

COutfit_Arcane_A::COutfit_Arcane_A(const COutfit_Arcane_A& rhs)
	: CGear_Item(rhs)
{
}

HRESULT COutfit_Arcane_A::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_ARCANE_A;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("아케인 자켓_R");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_076_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT COutfit_Arcane_A::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool COutfit_Arcane_A::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_ARCANE_A);

	Safe_Release(pPlayerInventory);

	return true;
}

void COutfit_Arcane_A::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Outfit_Arcane_A")
		, CCustomModel::MESHTYPE::TOP);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Pants_Arcane")
		, CCustomModel::MESHTYPE::PANTS);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Socks_E")
		, CCustomModel::MESHTYPE::SOCKS);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Boots_Arcane")
		, CCustomModel::MESHTYPE::SHOES);
}

COutfit_Arcane_A* COutfit_Arcane_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	COutfit_Arcane_A* pInstance = New COutfit_Arcane_A(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created COutfit_Arcane_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

COutfit_Arcane_A* COutfit_Arcane_A::Clone(void* pArg)
{
	COutfit_Arcane_A* pInstance = New COutfit_Arcane_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created COutfit_Arcane_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COutfit_Arcane_A::Free(void)
{
	__super::Free();
}
