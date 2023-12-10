#include "Event_Enter_Vault.h"
#include "GameInstance.h"

#include "Player.h"
#include "Trigger.h"
#include "Card_Fig.h"

CEvent_Enter_Vault::CEvent_Enter_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Enter_Vault::CEvent_Enter_Vault(const CEvent_Enter_Vault& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Enter_Vault::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("[CEvent_Enter_Vault] pArg is nullptr");
		return E_FAIL;
	}
	INITEVENTENTERVAULT* pInitDesc = reinterpret_cast<INITEVENTENTERVAULT*>(pArg);

	FAILED_CHECK_RETURN(Add_Components(*pInitDesc), E_FAIL);

	BEGININSTANCE;
	CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	ENDINSTANCE;
	if (nullptr == pPlayer)
		return E_FAIL;

	m_pCard_Fig = pPlayer->m_pCard_Fig;
	Safe_AddRef(m_pCard_Fig);

	return S_OK;
}

void CEvent_Enter_Vault::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Enter_Vault();
}

void CEvent_Enter_Vault::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Enter_Vault::Check_Event_Enter_Vault()
{
	if (true == m_isCallTrigger)
		return;

	if (true == m_pTrigger->Is_Collision())
	{
		m_pCard_Fig->On_Enter_Vault_Script();

		if (m_pTrigger->isDead())
			m_isCallTrigger = true;
	}
}

HRESULT CEvent_Enter_Vault::Add_Components(const INITEVENTENTERVAULT& InitDesc)
{
	/* For.Trigger_Spawn */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Sanctum_Spawn");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = InitDesc.vTriggerSize;
	TriggerDesc.vTriggerWorldPos = InitDesc.vTriggerWorldPosition;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger"), reinterpret_cast<CComponent**>(&m_pTrigger), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Enter_Vault Failed Add_Components : Trigger_Spawn");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Enter_Vault* CEvent_Enter_Vault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Enter_Vault* pInstance = New CEvent_Enter_Vault(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Enter_Vault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Enter_Vault* CEvent_Enter_Vault::Clone(void* pArg)
{
	CEvent_Enter_Vault* pInstance = New CEvent_Enter_Vault(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Enter_Vault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Enter_Vault::Free()
{
	__super::Free();

	Safe_Release(m_pCard_Fig);
	Safe_Release(m_pTrigger);
}