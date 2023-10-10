#include "WiggenweldPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CWiggenweldPotion_Item::CWiggenweldPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CWiggenweldPotion_Item::CWiggenweldPotion_Item(const CWiggenweldPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CWiggenweldPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_WIGGENWELD_POTION;
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 100;											// 가격
	m_ItemCreateDesc.wstrKoreanName = TEXT("위젠웰드 묘약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_WoundCleaning.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWiggenweldPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWiggenweldPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CWiggenweldPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CWiggenweldPotion_Item::Render()
{
	return __super::Render();
}

void CWiggenweldPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);
	 
	m_pPlayerInformation->Get_Health()->Heal(m_fRecoveryAmount);
}

void CWiggenweldPotion_Item::CreateTool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_WiggenweldPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str())))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CWiggenweldPotion_Item* CWiggenweldPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CWiggenweldPotion_Item* pInstance = New CWiggenweldPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CWiggenweldPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWiggenweldPotion_Item::Clone(void* pArg)
{
	CWiggenweldPotion_Item* pInstance = New CWiggenweldPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWiggenweldPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWiggenweldPotion_Item::Free()
{
	__super::Free();
}