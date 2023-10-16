#include "Hat_Lodgok.h"
#include "Inventory.h"
#include "Player.h"

CHat_Lodgok::CHat_Lodgok(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CHat_Lodgok::CHat_Lodgok(const CHat_Lodgok& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CHat_Lodgok::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_HAT_LODGOK;
	m_ItemCreateDesc.eItemType = HEAD;
	m_ItemCreateDesc.wstrKoreanName = TEXT("로드곡 모자");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Head_052_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHat_Lodgok::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CHat_Lodgok::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_HAT_LODGOK);

	Safe_Release(pPlayerInventory);

	return true;
}

void CHat_Lodgok::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Hat_Lodgok")
		, CCustomModel::MESHTYPE::HAT);
}

CHat_Lodgok* CHat_Lodgok::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CHat_Lodgok* pInstance = New CHat_Lodgok(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CHat_Lodgok");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHat_Lodgok* CHat_Lodgok::Clone(void* pArg)
{
	CHat_Lodgok* pInstance = New CHat_Lodgok(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CHat_Lodgok");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHat_Lodgok::Free(void)
{
	__super::Free();
}
