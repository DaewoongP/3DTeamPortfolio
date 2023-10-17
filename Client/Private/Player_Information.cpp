#include "Player_Information.h"
#include "Client_Defines.h"
#include "GameInstance.h"

#include "Inventory.h"

#include "UI_Group_HP.h"
#include "UI_Group_Finisher.h"

// Áö¿ï°Å
#include "WiggenweldPotion.h"
#include "Item.h"
#include "PotionTap.h"
#include "Tool.h"
CPlayer_Information::CPlayer_Information(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CPlayer_Information::CPlayer_Information(const CPlayer_Information& rhs)
	: CComposite(rhs)
{
}

void CPlayer_Information::Add_Potion()
{
	m_vecTool.push_back(static_cast<CTool*>(CItem::SimpleFactory(ITEM_ID::ITEM_ID_WIGGENWELD_POTION, LEVEL_STATIC, nullptr)));
}

CTool* CPlayer_Information::Get_Healpotion()
{
	if (true == m_vecTool.empty())
	{
		return nullptr;
	}

	return m_vecTool.back();
}

HRESULT CPlayer_Information::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Information::Initialize(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Add_Components : CPlayer_Information");

		return E_FAIL;
	}


	m_vecItemCount.resize(PLAYER_ITEM_END);

	return S_OK;
}

void CPlayer_Information::Tick(_float fTimeDelta)
{
	CComposite::Tick(fTimeDelta);

	if (nullptr == m_pInventory)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		
		m_pInventory = static_cast<CInventory*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Inventory"), TEXT("GameObject_Inventory")));
		Safe_AddRef(m_pInventory);

		Safe_Release(pGameInstance);
	}
}

void CPlayer_Information::Late_Tick(_float fTimeDelta)
{
	CComposite::Late_Tick(fTimeDelta);
	
}

HRESULT CPlayer_Information::Add_Components()
{
	CHealth::HEALTHDESC HealthDesc;

	HealthDesc.iMaxHP = 100;

	/* Com_Health */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
		TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_Health)");
		__debugbreak();
		return E_FAIL;
	}

	HealthDesc.iMaxHP = 100;

	/* Com_Finisher */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
		TEXT("Com_Finisher"), reinterpret_cast<CComponent**>(&m_pFinisher), &HealthDesc)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_Finisher)");
		__debugbreak();
		return E_FAIL;
	}



	CUI_Group_HP::HPDESC HpDesc;

	HpDesc.pHealth = m_pHealth;
	lstrcpy(HpDesc.wszFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_HP.uidata"));
	lstrcpy(HpDesc.wszNumber, TEXT("0"));

	/* Com_UI_Group_HP */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_HP"),
		TEXT("Com_UI_Group_HP"), reinterpret_cast<CComponent**>(&m_pUI_Health), &HpDesc)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_UI_Group_HP)");
		__debugbreak();
		return E_FAIL;
	}


	CUI_Group_Finisher::FINISHERDESC FinisherDesc;

	FinisherDesc.pHealth = m_pFinisher;
	lstrcpy(FinisherDesc.szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Front.uidata"));

	/* Com_UI_Group_Finisher */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Finisher"),
		TEXT("Com_UI_Group_Finisher"), reinterpret_cast<CComponent**>(&m_pUI_Finisher), &FinisherDesc)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_UI_Group_Finisher)");
		__debugbreak();
		return E_FAIL;
	}

	/* Com_Potion_Tap */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Potion_Tap"),
		TEXT("Com_Potion_Tap"), reinterpret_cast<CComponent**>(&m_pPotionTap))))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_UI_Group_Finisher)");
		__debugbreak();
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Potion.uidata"));
	/* Com_UI_Group_Potion */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Potion"),
		TEXT("Com_UI_Group_Potion"), reinterpret_cast<CComponent**>(&m_pPotion), szFilePath)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_UI_Group_Potion)");
		__debugbreak();
		return E_FAIL;
	}

	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Icon.uidata"));
	/* Com_UI_Group_Finisher_Icon */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
		TEXT("Com_UI_Group_Finisher_Icon"), reinterpret_cast<CComponent**>(&m_pFinisherIcon), szFilePath)))
	{
		MSG_BOX("Failed CPlayer_Information Add_Component : (Com_UI_Group_Finisher_Icon)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

_bool CPlayer_Information::Is_Use_Fnisher()
{
	if (0.5f <= m_pFinisher->Get_Current_HP_Percent())
	{
		return true;
	}
	m_vecTool.push_back(static_cast<CTool*>(CItem::SimpleFactory(ITEM_ID::ITEM_ID_WIGGENWELD_POTION, LEVEL_STATIC, nullptr)));
	return false;
}

void CPlayer_Information::Using_Fnisher()
{
	m_pFinisher->Set_HP(m_pFinisher->Get_HP() - m_pFinisher->Get_MaxHP() / 2);
	int a = 0;
}

void CPlayer_Information::fix_HP(_int _iNum)
{
	_int iResult = m_pHealth->Get_HP() + _iNum;
	if (0 <= iResult)
	{
		Clamp<_int>(iResult, 0, m_pHealth->Get_MaxHP());
		m_pHealth->Set_HP(iResult);
	}
}

CPlayer_Information* CPlayer_Information::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Information* pInstance = New CPlayer_Information(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer_Information");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPlayer_Information::Clone(void* pArg)
{
	CPlayer_Information* pInstance = New CPlayer_Information(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer_Information");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Information::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pHealth);
		Safe_Release(m_pFinisher);
		Safe_Release(m_pUI_Finisher);
		Safe_Release(m_pUI_Health);
		Safe_Release(m_pInventory);
		Safe_Release(m_pPotionTap);
		Safe_Release(m_pFinisherIcon);
		Safe_Release(m_pPotion);
	}
}
