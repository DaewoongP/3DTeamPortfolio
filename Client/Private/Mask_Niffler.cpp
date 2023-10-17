#include "Mask_Niffler.h"
#include "Inventory.h"
#include "Player.h"

CMask_Niffler::CMask_Niffler(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CMask_Niffler::CMask_Niffler(const CMask_Niffler& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CMask_Niffler::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MASK_NIFFLER;
	m_ItemCreateDesc.eItemType = FACE;
	m_ItemCreateDesc.wstrKoreanName = TEXT("니플러 마스크");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Face_042.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMask_Niffler::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CMask_Niffler::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_MASK_NIFFLER);

	Safe_Release(pPlayerInventory);

	return true;
}

void CMask_Niffler::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Mask_Niffler")
		, CCustomModel::MESHTYPE::MASK);
}

CMask_Niffler* CMask_Niffler::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMask_Niffler* pInstance = New CMask_Niffler(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMask_Niffler");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMask_Niffler* CMask_Niffler::Clone(void* pArg)
{
	CMask_Niffler* pInstance = New CMask_Niffler(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMask_Niffler");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMask_Niffler::Free(void)
{
	__super::Free();
}
