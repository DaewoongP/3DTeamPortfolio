#include "InvisibilityPotion_Item.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Information.h"

CInvisibilityPotion_Item::CInvisibilityPotion_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool(pDevice, pContext)
{
}

CInvisibilityPotion_Item::CInvisibilityPotion_Item(const CInvisibilityPotion_Item& rhs)
	: CTool(rhs)
{
}

HRESULT CInvisibilityPotion_Item::Initialize_Prototype(_uint iLevel)
{
	// 아이템 정보
	m_ItemCreateDesc.eItemType = ITEMTYPE::POTION;
	m_ItemCreateDesc.iCost = 500;											// 가격
	m_ItemCreateDesc.fDuration = 4.f;										//지속시간
	m_ItemCreateDesc.wstrKoreanName = TEXT("투명 마법약");					// 한글명
	m_ItemCreateDesc.wstrUIPath = TEXT("../../Resources/UI/Game/UI/Icons/Potions/UI_T_InvisibilityPotion.png"); // UI경로

	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvisibilityPotion_Item::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CInvisibilityPotion_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInvisibilityPotion_Item::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInvisibilityPotion_Item::Render()
{
	return __super::Render();
}

void CInvisibilityPotion_Item::Use(_float3 vPlayPos)
{
	__super::Use(vPlayPos);

	cout << "투명해요" << endl;
	// 메쉬 올리기.
	//m_pPlayerInformation->Get_Health()->Heal(m_fRecoveryAmount);
}

void CInvisibilityPotion_Item::CreateTool()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_uint iLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(
		LEVEL_STATIC,
		iLevelIndex,
		TEXT("Prototype_GameObject_InvisibilityPotion"),
		TEXT("Layer_BackGround"),
		Generate_HashtagW().c_str())))
	{
		__debugbreak();
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

CInvisibilityPotion_Item* CInvisibilityPotion_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CInvisibilityPotion_Item* pInstance = New CInvisibilityPotion_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CInvisibilityPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInvisibilityPotion_Item::Clone(void* pArg)
{
	CInvisibilityPotion_Item* pInstance = New CInvisibilityPotion_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInvisibilityPotion_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvisibilityPotion_Item::Free()
{
	__super::Free();
}