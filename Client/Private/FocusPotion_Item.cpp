#include "FocusPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CFocusPotion_Item::CFocusPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CFocusPotion_Item::CFocusPotion_Item(const CFocusPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CFocusPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemId = ITEM_ID_FOCUS_POTION;
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 500;									// 가격
	m_ItemCreateDesc.fDuration = 15.f;								//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("집중력 물약");			// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_AMFillPotion.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFocusPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFocusPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CFocusPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CFocusPotion_Item::Render()
{
	return __super::Render();
}

void CFocusPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	cout << "집중하고 있어요" << endl;
	// 메쉬 올리기.
	//m_pPlayerInformation->Get_Health()->Heal(m_fRecoveryAmount);
}

void CFocusPotion_Item::CreateTool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_FocusPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str())))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CFocusPotion_Item* CFocusPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CFocusPotion_Item* pInstance = New CFocusPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CFocusPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFocusPotion_Item::Clone(void* pArg)
{
	CFocusPotion_Item* pInstance = New CFocusPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFocusPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFocusPotion_Item::Free()
{
	__super::Free();
}