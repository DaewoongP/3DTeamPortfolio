#include "Mask_Guardian.h"
#include "Inventory.h"
#include "Player.h"

CMask_Guardian::CMask_Guardian(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CMask_Guardian::CMask_Guardian(const CMask_Guardian& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CMask_Guardian::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MASK_GUARDIAN;
	m_ItemCreateDesc.eItemType = FACE;
	m_ItemCreateDesc.wstrKoreanName = TEXT("가디언 마스크");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Face_001.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMask_Guardian::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CMask_Guardian::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_MASK_GUARDIAN);

	Safe_Release(pPlayerInventory);

	return true;
}

void CMask_Guardian::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Mask_Guardian")
		, CCustomModel::MESHTYPE::MASK);
}

CMask_Guardian* CMask_Guardian::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMask_Guardian* pInstance = New CMask_Guardian(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMask_Guardian");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMask_Guardian* CMask_Guardian::Clone(void* pArg)
{
	CMask_Guardian* pInstance = New CMask_Guardian(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CMask_Guardian");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMask_Guardian::Free(void)
{
	__super::Free();
}
