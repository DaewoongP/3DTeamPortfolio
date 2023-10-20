#include "Jacket_DragonGrab.h"
#include "Inventory.h"
#include "Player.h"

CJacket_DragonGrab::CJacket_DragonGrab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_DragonGrab::CJacket_DragonGrab(const CJacket_DragonGrab& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_DragonGrab::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_DRAGONGRAB;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("모자1");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_001_F_Gryffindor.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_DragonGrab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_DragonGrab::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_DRAGONGRAB);
	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_DRAGONGRAB_A);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_DragonGrab::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_DragonGrab")
		, CCustomModel::MESHTYPE::ROBE);
}

CJacket_DragonGrab* CJacket_DragonGrab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_DragonGrab* pInstance = New CJacket_DragonGrab(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CJacket_DragonGrab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_DragonGrab* CJacket_DragonGrab::Clone(void* pArg)
{
	CJacket_DragonGrab* pInstance = New CJacket_DragonGrab(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_DragonGrab");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_DragonGrab::Free(void)
{
	__super::Free();
}
