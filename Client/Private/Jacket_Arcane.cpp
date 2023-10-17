#include "Jacket_Arcane.h"
#include "Inventory.h"
#include "Player.h"

CJacket_Arcane::CJacket_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_Arcane::CJacket_Arcane(const CJacket_Arcane& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_Arcane::Initialize_Prototype(_uint iLevel)
{
	// ������ ����
	m_ItemCreateDesc.eItemId = ITEM_ID_JACKET_ARCANE;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("������ ��Ʈ");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_076_M.png");
	m_ItemCreateDesc.iCost = 100;

	// ��� ����

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_Arcane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_Arcane::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_JACKET_ARCANE_A);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_PANTS_ARCANE);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_JACKET_ARCANE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_Arcane::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_Arcane")
		, CCustomModel::MESHTYPE::TOP);
}

CJacket_Arcane* CJacket_Arcane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_Arcane* pInstance = New CJacket_Arcane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CJacket_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_Arcane* CJacket_Arcane::Clone(void* pArg)
{
	CJacket_Arcane* pInstance = New CJacket_Arcane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_Arcane::Free(void)
{
	__super::Free();
}
