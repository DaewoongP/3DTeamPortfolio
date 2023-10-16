#include "Pants_Merlin.h"
#include "Inventory.h"
#include "Player.h"

CPants_Merlin::CPants_Merlin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CPants_Merlin::CPants_Merlin(const CPants_Merlin& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CPants_Merlin::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_PANTS_MERLIN;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("켈트 바지");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_105_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPants_Merlin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CPants_Merlin::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_PANTS_MERLIN);

	Safe_Release(pPlayerInventory);

	return true;
}

void CPants_Merlin::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Pants_Merlin")
		, CCustomModel::MESHTYPE::PANTS);
}

CPants_Merlin* CPants_Merlin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CPants_Merlin* pInstance = New CPants_Merlin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPants_Merlin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPants_Merlin* CPants_Merlin::Clone(void* pArg)
{
	CPants_Merlin* pInstance = New CPants_Merlin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CPants_Merlin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPants_Merlin::Free(void)
{
	__super::Free();
}
