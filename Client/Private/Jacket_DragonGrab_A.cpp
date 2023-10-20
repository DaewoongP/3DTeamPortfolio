#include "Jacket_DragonGrab_A.h"
#include "Inventory.h"
#include "Player.h"

CJacket_DragonGrab_A::CJacket_DragonGrab_A(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CJacket_DragonGrab_A::CJacket_DragonGrab_A(const CJacket_DragonGrab_A& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CJacket_DragonGrab_A::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_OUTFIT_DRAGONGRAB_A;
	m_ItemCreateDesc.eItemType = OUTFIT;
	m_ItemCreateDesc.wstrKoreanName = TEXT("모자1");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_001_F_Gryffindor.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CJacket_DragonGrab_A::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CJacket_DragonGrab_A::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_OUTFIT_DRAGONGRAB_A);

	Safe_Release(pPlayerInventory);

	return true;
}

void CJacket_DragonGrab_A::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Jacket_DragonGrab_A")
		, CCustomModel::MESHTYPE::ROBE);
}

CJacket_DragonGrab_A* CJacket_DragonGrab_A::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CJacket_DragonGrab_A* pInstance = New CJacket_DragonGrab_A(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CJacket_DragonGrab_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CJacket_DragonGrab_A* CJacket_DragonGrab_A::Clone(void* pArg)
{
	CJacket_DragonGrab_A* pInstance = New CJacket_DragonGrab_A(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CJacket_DragonGrab_A");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJacket_DragonGrab_A::Free(void)
{
	__super::Free();
}
