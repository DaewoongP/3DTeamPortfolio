#include "Boots_Default.h"
#include "Inventory.h"
#include "Player.h"

CBoots_Default::CBoots_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CBoots_Default::CBoots_Default(const CBoots_Default& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CBoots_Default::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_BOOTS_DEFAULT;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("부츠");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_001_F_Gryffindor.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoots_Default::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CBoots_Default::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_BOOTS_DEFAULT);

	Safe_Release(pPlayerInventory);

	return true;
}

void CBoots_Default::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Boots_Default")
		, CCustomModel::MESHTYPE::SHOES);
}

CBoots_Default* CBoots_Default::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CBoots_Default* pInstance = New CBoots_Default(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBoots_Default");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoots_Default* CBoots_Default::Clone(void* pArg)
{
	CBoots_Default* pInstance = New CBoots_Default(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CBoots_Default");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoots_Default::Free(void)
{
	__super::Free();
}
