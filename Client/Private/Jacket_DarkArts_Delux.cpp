#include "Jacket_DarkArts_Delux.h"
#include "Inventory.h"
#include "Player.h"

CJacket_DarkArts_Delux::CJacket_DarkArts_Delux(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_DarkArts_Delux::CJacket_DarkArts_Delux(const CJacket_DarkArts_Delux& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_DarkArts_Delux::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_JACKET_DARKARTS_DELUX;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("다크 디럭스 세트");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_072_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_DarkArts_Delux::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_DarkArts_Delux::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_JACKET_DARKARTS_DELUX_A);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_PANTS_DARKARTS_DELUX);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_JACKET_DARKARTS_DELUX);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_DarkArts_Delux::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_DarkArtsDelux")
		, CCustomModel::MESHTYPE::TOP);
}

CJacket_DarkArts_Delux* CJacket_DarkArts_Delux::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_DarkArts_Delux* pInstance = New CJacket_DarkArts_Delux(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe1_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_DarkArts_Delux* CJacket_DarkArts_Delux::Clone(void* pArg)
{
	CJacket_DarkArts_Delux* pInstance = New CJacket_DarkArts_Delux(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_DarkArts_Delux");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_DarkArts_Delux::Free(void)
{
	__super::Free();
}
