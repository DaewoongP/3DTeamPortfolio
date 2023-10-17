#include "Boots_Merlin.h"
#include "Inventory.h"
#include "Player.h"

CBoots_Merlin::CBoots_Merlin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CBoots_Merlin::CBoots_Merlin(const CBoots_Merlin& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CBoots_Merlin::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_BOOTS_MERLIN;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("켈트 부츠");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Outfit_105_M.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoots_Merlin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CBoots_Merlin::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_BOOTS_MERLIN);

	Safe_Release(pPlayerInventory);

	return true;
}

void CBoots_Merlin::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Boots_Merlin")
		, CCustomModel::MESHTYPE::SHOES);
}

CBoots_Merlin* CBoots_Merlin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBoots_Merlin* pInstance = New CBoots_Merlin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBoots_Merlin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoots_Merlin* CBoots_Merlin::Clone(void* pArg)
{
	CBoots_Merlin* pInstance = New CBoots_Merlin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CBoots_Merlin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoots_Merlin::Free(void)
{
	__super::Free();
}
