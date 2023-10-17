#include "Hat_Wizard.h"
#include "Inventory.h"
#include "Player.h"

CHat_Wizard::CHat_Wizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CHat_Wizard::CHat_Wizard(const CHat_Wizard& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CHat_Wizard::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_HAT_WIZARD;
	m_ItemCreateDesc.eItemType = HEAD;
	m_ItemCreateDesc.wstrKoreanName = TEXT("위자드 모자");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Head_064_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHat_Wizard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CHat_Wizard::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_HAT_WIZARD);

	Safe_Release(pPlayerInventory);

	return true;
}

void CHat_Wizard::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Hat_Wizard")
		, CCustomModel::MESHTYPE::HAT);
}

CHat_Wizard* CHat_Wizard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHat_Wizard* pInstance = New CHat_Wizard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHat_Wizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHat_Wizard* CHat_Wizard::Clone(void* pArg)
{
	CHat_Wizard* pInstance = New CHat_Wizard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CHat_Wizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_Wizard::Free(void)
{
	__super::Free();
}
