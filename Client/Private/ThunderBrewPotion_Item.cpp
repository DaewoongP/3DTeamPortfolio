#include "ThunderBrewPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CThunderBrewPotion_Item::CThunderBrewPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CThunderBrewPotion_Item::CThunderBrewPotion_Item(const CThunderBrewPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CThunderBrewPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_THUNDERBEW_POTION;
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 1000;											// 가격
	m_ItemCreateDesc.fDuration = 20.f;                                      //지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("썬더브루 천둥약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_EssenceOfDittany.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThunderBrewPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CThunderBrewPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CThunderBrewPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CThunderBrewPotion_Item::Render()
{
	return __super::Render();
}

void CThunderBrewPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);
}

void CThunderBrewPotion_Item::CreateTool(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_ThunderBrewPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str(), pArg)))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CThunderBrewPotion_Item* CThunderBrewPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CThunderBrewPotion_Item* pInstance = New CThunderBrewPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CThunderBrewPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThunderBrewPotion_Item::Clone(void* pArg)
{
	CThunderBrewPotion_Item* pInstance = New CThunderBrewPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CThunderBrewPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThunderBrewPotion_Item::Free()
{
	__super::Free();
}