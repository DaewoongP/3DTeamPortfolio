#include "Robe_Arcane.h"
#include "Inventory.h"
#include "Player.h"

CRobe_Arcane::CRobe_Arcane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe_Arcane::CRobe_Arcane(const CRobe_Arcane& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe_Arcane::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE_ARCANE;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("아케인 로브");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_074_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe_Arcane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe_Arcane::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE_ARCANE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe_Arcane::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Robe_Arcane")
		, CCustomModel::MESHTYPE::ROBE);

	m_pPlayerModel->Set_Top_For_Robe(LEVEL_STATIC);
}

CRobe_Arcane* CRobe_Arcane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe_Arcane* pInstance = New CRobe_Arcane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRobe_Arcane* CRobe_Arcane::Clone(void* pArg)
{
	CRobe_Arcane* pInstance = New CRobe_Arcane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe_Arcane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe_Arcane::Free(void)
{
	__super::Free();
}
