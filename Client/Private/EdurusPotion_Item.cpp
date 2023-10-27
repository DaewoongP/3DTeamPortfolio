#include "EdurusPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CEdurusPotion_Item::CEdurusPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CEdurusPotion_Item::CEdurusPotion_Item(const CEdurusPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CEdurusPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_EDURUS_POTION;
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 300;											// 가격
	m_ItemCreateDesc.fDuration = 20.f;										//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("에두루스 마법약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_Edurus.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdurusPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Clone_Desc.pIsStartTiming = static_cast<_bool*>(pArg);

	return S_OK;
}

void CEdurusPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEdurusPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEdurusPotion_Item::Render()
{
	return __super::Render();
}

void CEdurusPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	m_pPlayer->DefenceTiming();

#ifdef _DEBUG
	
	cout << "방어력이 증가했어요" << endl;

#endif // _DEBUG

}

void CEdurusPotion_Item::CreateTool(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_EdurusPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str(),pArg)))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CEdurusPotion_Item* CEdurusPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CEdurusPotion_Item* pInstance = New CEdurusPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CEdurusPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdurusPotion_Item::Clone(void* pArg)
{
	CEdurusPotion_Item* pInstance = New CEdurusPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEdurusPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdurusPotion_Item::Free()
{
	__super::Free();
}