#include "..\Public\Robe1_Item.h"
#include "Inventory.h"
#include "Player.h"
CRobe1_Item::CRobe1_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe1_Item::CRobe1_Item(const CRobe1_Item& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe1_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE1;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("로브1");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_001_F_Gryffindor.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe1_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe1_Item::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE1);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe1_Item::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Player_Top_test")
		, CCustomModel::MESHTYPE::ROBE, nullptr);
}

CRobe1_Item* CRobe1_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe1_Item* pInstance = New CRobe1_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe1_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRobe1_Item::Clone(void* pArg)
{
	CRobe1_Item* pInstance = New CRobe1_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe1_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe1_Item::Free(void)
{
	__super::Free();
}
