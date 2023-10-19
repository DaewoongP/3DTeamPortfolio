#include "Robe_Quidditch.h"
#include "Inventory.h"
#include "Player.h"

CRobe_Quidditch::CRobe_Quidditch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGear_Item(pDevice, pContext)
{
}

CRobe_Quidditch::CRobe_Quidditch(const CRobe_Quidditch& rhs)
	: CGear_Item(rhs)
{
}

HRESULT CRobe_Quidditch::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_ROBE_QUDDITCH;
	m_ItemCreateDesc.eItemType = BACK;
	m_ItemCreateDesc.wstrKoreanName = TEXT("퀴디치 로브");
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Gear/UI_T_GA_Back_008_F.png");
	m_ItemCreateDesc.iCost = 100;

	// 기어 정보

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRobe_Quidditch::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CRobe_Quidditch::Buy()
{
	if (false == __super::Buy())
		return false;

	CInventory* pPlayerInventory = m_pPlayerInformation->Get_Inventory();
	Safe_AddRef(pPlayerInventory);

	pPlayerInventory->Add_Item(ITEM_ID::ITEM_ID_ROBE_QUDDITCH);

	Safe_Release(pPlayerInventory);

	return true;
}

void CRobe_Quidditch::Equipment()
{
	m_pPlayerModel->Add_MeshParts(LEVEL_STATIC, TEXT("Prototype_Component_MeshPart_Robe_Quidditch")
		, CCustomModel::MESHTYPE::ROBE, _float4(1.f, 1.f, 1.f, 1.f), TEXT("../../Resources/GameData/ClothData/Robe_Quidditch.cloth"));
	m_pPlayer->Set_Robe_Mesh_Index(0);
}

CRobe_Quidditch* CRobe_Quidditch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CRobe_Quidditch* pInstance = New CRobe_Quidditch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CRobe_Quidditch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRobe_Quidditch* CRobe_Quidditch::Clone(void* pArg)
{
	CRobe_Quidditch* pInstance = New CRobe_Quidditch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created CRobe_Quidditch");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobe_Quidditch::Free(void)
{
	__super::Free();
}
