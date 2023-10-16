#include "Hat_Arcane.h"
#include "Inventory.h"
#include "Player.h"

CHat_Arcane::CHat_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CHat_Arcane::CHat_Arcane(const CHat_Arcane& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CHat_Arcane::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_HAT_ARCANE;
	m_ItemCreateDesc.eItemType = HEAD;
	m_ItemCreateDesc.wstrKoreanName = TEXT("아케인 모자");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Head_059_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHat_Arcane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CHat_Arcane::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_HAT_ARCANE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CHat_Arcane::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Hat_Arcane")
		, CCustomModel::MESHTYPE::HAT);
}

CHat_Arcane* CHat_Arcane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHat_Arcane* pInstance = New CHat_Arcane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHat_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHat_Arcane* CHat_Arcane::Clone(void* pArg)
{
	CHat_Arcane* pInstance = New CHat_Arcane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CHat_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_Arcane::Free(void)
{
	__super::Free();
}
