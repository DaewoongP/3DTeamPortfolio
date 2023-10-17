#include "Robe_DarkArts_Delux.h"
#include "Inventory.h"
#include "Player.h"

CRobe_DarkArts_Delux::CRobe_DarkArts_Delux(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe_DarkArts_Delux::CRobe_DarkArts_Delux(const CRobe_DarkArts_Delux& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe_DarkArts_Delux::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE_DARKARTS_DELUX;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("다크 디럭스 로브");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_112_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe_DarkArts_Delux::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe_DarkArts_Delux::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE_DARKARTS_DELUX);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe_DarkArts_Delux::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Robe_DarkArtsDelux_HoodDown")
		, CCustomModel::MESHTYPE::ROBE);
}

CRobe_DarkArts_Delux* CRobe_DarkArts_Delux::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe_DarkArts_Delux* pInstance = New CRobe_DarkArts_Delux(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe_DarkArts_Delux");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRobe_DarkArts_Delux* CRobe_DarkArts_Delux::Clone(void* pArg)
{
	CRobe_DarkArts_Delux* pInstance = New CRobe_DarkArts_Delux(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe_DarkArts_Delux");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe_DarkArts_Delux::Free(void)
{
	__super::Free();
}
