#include "Pants_Stealth.h"
#include "Inventory.h"
#include "Player.h"

CPants_Stealth::CPants_Stealth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CPants_Stealth::CPants_Stealth(const CPants_Stealth& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CPants_Stealth::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_PANTS_STEALTH;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("모자1");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_001_F_Gryffindor.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPants_Stealth::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CPants_Stealth::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_PANTS_STEALTH);

	Safe_Release(pPlayerInventory);

	return true;
}

void CPants_Stealth::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Pants_Stealth")
		, CCustomModel::MESHTYPE::PANTS);
}

CPants_Stealth* CPants_Stealth::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CPants_Stealth* pInstance = New CPants_Stealth(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPants_Stealth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CPants_Stealth* CPants_Stealth::Clone(void* pArg)
{
	CPants_Stealth* pInstance = New CPants_Stealth(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CPants_Stealth");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPants_Stealth::Free(void)
{
	__super::Free();
}
