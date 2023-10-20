#include "Outfit_Celtic_A.h"
#include "Inventory.h"
#include "Player.h"

COutfit_Celtic_A::COutfit_Celtic_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

COutfit_Celtic_A::COutfit_Celtic_A(const COutfit_Celtic_A& rhs)
	: CGear_Item(rhs)
{
}

HRESULT COutfit_Celtic_A::Initialize_Prototype(_uint iLevel)
{
	// ������ ����
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_CELTIC_A;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("��Ʈ ��Ʈ_R");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_105_M.png");
	m_ItemCreateDesc.iCost = 100;

	// ��� ����

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT COutfit_Celtic_A::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool COutfit_Celtic_A::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_CELTIC_A);

	Safe_Release(pPlayerInventory);

	return true;
}

void COutfit_Celtic_A::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_Celtic_A")
		, CCustomModel::MESHTYPE::TOP);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Pants_Merlin")
		, CCustomModel::MESHTYPE::PANTS);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Socks_E")
		, CCustomModel::MESHTYPE::SOCKS);
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Boots_Merlin")
		, CCustomModel::MESHTYPE::SHOES);
}

COutfit_Celtic_A* COutfit_Celtic_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	COutfit_Celtic_A* pInstance = New COutfit_Celtic_A(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe1_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

COutfit_Celtic_A* COutfit_Celtic_A::Clone(void* pArg)
{
	COutfit_Celtic_A* pInstance = New COutfit_Celtic_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created COutfit_Celtic_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COutfit_Celtic_A::Free(void)
{
	__super::Free();
}
