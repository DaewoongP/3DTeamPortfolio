#include "MaximaPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CMaximaPotion_Item::CMaximaPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CMaximaPotion_Item::CMaximaPotion_Item(const CMaximaPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CMaximaPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_MAXIMA_POTION;
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 300;											// 가격
	m_ItemCreateDesc.fDuration = 20.f;                                      //지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("막시마 마법약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_Maxima.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaximaPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Clone_Desc.pIsStartTiming = static_cast<_bool*>(pArg);


	return S_OK;
}

void CMaximaPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMaximaPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CMaximaPotion_Item::Render()
{
	return __super::Render();
}

void CMaximaPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	m_pPlayer->DamageTiming();
}

void CMaximaPotion_Item::CreateTool(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_MaximaPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str(), pArg)))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CMaximaPotion_Item* CMaximaPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CMaximaPotion_Item* pInstance = New CMaximaPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CMaximaPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMaximaPotion_Item::Clone(void* pArg)
{
	CMaximaPotion_Item* pInstance = New CMaximaPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMaximaPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaximaPotion_Item::Free()
{
	__super::Free();
}