#include "Jacket_DarkArts_Delux_A.h"
#include "Inventory.h"
#include "Player.h"

CJacket_DarkArts_Delux_A::CJacket_DarkArts_Delux_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_DarkArts_Delux_A::CJacket_DarkArts_Delux_A(const CJacket_DarkArts_Delux_A& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_DarkArts_Delux_A::Initialize_Prototype(_uint iLevel)
{
	// ������ ����
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_DARKARTS_DELUX_A;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("��ũ �𷰽� ����_R");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_072_M.png");
	m_ItemCreateDesc.iCost = 100;

	// ��� ����

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_DarkArts_Delux_A::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_DarkArts_Delux_A::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_DARKARTS_DELUX_A);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_DarkArts_Delux_A::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_DarkArtsDelux_A")
		, CCustomModel::MESHTYPE::TOP);
}

CJacket_DarkArts_Delux_A* CJacket_DarkArts_Delux_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_DarkArts_Delux_A* pInstance = New CJacket_DarkArts_Delux_A(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CJacket_DarkArts_Delux_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_DarkArts_Delux_A* CJacket_DarkArts_Delux_A::Clone(void* pArg)
{
	CJacket_DarkArts_Delux_A* pInstance = New CJacket_DarkArts_Delux_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_DarkArts_Delux_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_DarkArts_Delux_A::Free(void)
{
	__super::Free();
}
