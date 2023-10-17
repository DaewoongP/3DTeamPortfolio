#include "Robe_DarkArts.h"
#include "Inventory.h"
#include "Player.h"

CRobe_DarkArts::CRobe_DarkArts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe_DarkArts::CRobe_DarkArts(const CRobe_DarkArts& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe_DarkArts::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE_DARKARTS;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("다크 아트 로브");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_110_Male.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe_DarkArts::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe_DarkArts::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE_DARKARTS);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe_DarkArts::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Robe_DarkArts")
		, CCustomModel::MESHTYPE::ROBE);
}

CRobe_DarkArts* CRobe_DarkArts::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe_DarkArts* pInstance = New CRobe_DarkArts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe_DarkArts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRobe_DarkArts* CRobe_DarkArts::Clone(void* pArg)
{
	CRobe_DarkArts* pInstance = New CRobe_DarkArts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe_DarkArts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe_DarkArts::Free(void)
{
	__super::Free();
}
