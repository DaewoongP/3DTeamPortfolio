#include "Mask_Demiguise.h"
#include "Inventory.h"
#include "Player.h"

CMask_Demiguise::CMask_Demiguise(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CMask_Demiguise::CMask_Demiguise(const CMask_Demiguise& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CMask_Demiguise::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MASK_DEMIGUISE;
	m_ItemCreateDesc.eItemType = FACE;
	m_ItemCreateDesc.wstrKoreanName = TEXT("흉내쟁이 마스크");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Face_043.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMask_Demiguise::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CMask_Demiguise::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_MASK_DEMIGUISE);

	Safe_Release(pPlayerInventory);

	return true;
}

void CMask_Demiguise::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Mask_Demiguise")
		, CCustomModel::MESHTYPE::MASK);
}

CMask_Demiguise* CMask_Demiguise::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMask_Demiguise* pInstance = New CMask_Demiguise(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMask_Demiguise");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMask_Demiguise* CMask_Demiguise::Clone(void* pArg)
{
	CMask_Demiguise* pInstance = New CMask_Demiguise(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMask_Demiguise");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMask_Demiguise::Free(void)
{
	__super::Free();
}
