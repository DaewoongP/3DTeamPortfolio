#include "Hat_Fedora.h"
#include "Inventory.h"
#include "Player.h"

CHat_Fedora::CHat_Fedora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CHat_Fedora::CHat_Fedora(const CHat_Fedora& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CHat_Fedora::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_HAT_FEDORA;
	m_ItemCreateDesc.eItemType = HEAD;
	m_ItemCreateDesc.wstrKoreanName = TEXT("페도라");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Head_010_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHat_Fedora::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CHat_Fedora::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_HAT_FEDORA);

	Safe_Release(pPlayerInventory);

	return true;
}

void CHat_Fedora::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Hat_Fedora")
		, CCustomModel::MESHTYPE::HAT);
}

CHat_Fedora* CHat_Fedora::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHat_Fedora* pInstance = New CHat_Fedora(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHat_Fedora");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHat_Fedora* CHat_Fedora::Clone(void* pArg)
{
	CHat_Fedora* pInstance = New CHat_Fedora(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CHat_Fedora");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_Fedora::Free(void)
{
	__super::Free();
}
