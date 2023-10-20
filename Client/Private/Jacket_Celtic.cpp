#include "Jacket_Celtic.h"
#include "Inventory.h"
#include "Player.h"

CJacket_Celtic::CJacket_Celtic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_Celtic::CJacket_Celtic(const CJacket_Celtic& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_Celtic::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_CELTIC;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("켈트 세트");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_105_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_Celtic::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_Celtic::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_CELTIC_A);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_CELTIC);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_Celtic::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_Celtic")
		, CCustomModel::MESHTYPE::TOP);
}

CJacket_Celtic* CJacket_Celtic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_Celtic* pInstance = New CJacket_Celtic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe1_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_Celtic* CJacket_Celtic::Clone(void* pArg)
{
	CJacket_Celtic* pInstance = New CJacket_Celtic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_Celtic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_Celtic::Free(void)
{
	__super::Free();
}
