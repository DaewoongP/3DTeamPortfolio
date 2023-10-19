#include "Robe_Beast.h"
#include "Inventory.h"
#include "Player.h"

CRobe_Beast::CRobe_Beast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe_Beast::CRobe_Beast(const CRobe_Beast& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe_Beast::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE_BEAST;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("비스트 로브");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_079_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe_Beast::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe_Beast::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE_BEAST);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe_Beast::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Robe_Beast")
		, CCustomModel::MESHTYPE::ROBE);
}

CRobe_Beast* CRobe_Beast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe_Beast* pInstance = New CRobe_Beast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe_Beast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRobe_Beast* CRobe_Beast::Clone(void* pArg)
{
	CRobe_Beast* pInstance = New CRobe_Beast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe_Beast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe_Beast::Free(void)
{
	__super::Free();
}
