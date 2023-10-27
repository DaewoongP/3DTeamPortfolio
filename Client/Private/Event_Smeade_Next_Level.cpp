#include "..\Public\Event_Smeade_Next_Level.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Trigger.h"

CEvent_Smeade_Next_Level::CEvent_Smeade_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Smeade_Next_Level::CEvent_Smeade_Next_Level(const CEvent_Smeade_Next_Level& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Smeade_Next_Level::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	return S_OK;
}

void CEvent_Smeade_Next_Level::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Cliff(fTimeDelta);
	Check_Event_Sanctum(fTimeDelta);
	Check_Event_Sky(fTimeDelta);
}

void CEvent_Smeade_Next_Level::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEvent_Smeade_Next_Level::Add_Components()
{
	/* For.Trigger_Hogsmeade_Next_Level1 */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Hogsmeade_Next_Level_Cliff");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(1.f, 2.5f, 0.1f);
	TriggerDesc.vTriggerWorldPos = _float3(31.f, 0.f, 20.5f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Hogsmeade_Next_Level1"), reinterpret_cast<CComponent**>(&m_pNext_Level_Cliff), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Smeade_Next_Level Failed Add_Components : Trigger_Hogsmeade_Next_Level1");
		return E_FAIL;
	}

	/* For.Trigger_Hogsmeade_Next_Level2 */
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Hogsmeade_Next_Level_Sanctum");
	TriggerDesc.vTriggerSize = _float3(1.f, 2.5f, 0.1f);
	TriggerDesc.vTriggerWorldPos = _float3(116.7f, 10.75f, 140.8f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Hogsmeade_Next_Level2"), reinterpret_cast<CComponent**>(&m_pNext_Level_Sanctum), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Smeade_Next_Level Failed Add_Components : Trigger_Hogsmeade_Next_Level2");
		return E_FAIL;
	}

	/* For.Trigger_Hogsmeade_Next_Level3 */
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Hogsmeade_Next_Level_Sky");
	TriggerDesc.vTriggerSize = _float3(1.f, 1.f, 1.f);
	TriggerDesc.vTriggerWorldPos = _float3(29.5f, 3.5f, 70.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Hogsmeade_Next_Level3"), reinterpret_cast<CComponent**>(&m_pNext_Level_Sky), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Smeade_Next_Level Failed Add_Components : Trigger_Hogsmeade_Next_Level3");
		return E_FAIL;
	}

	return S_OK;
}

void CEvent_Smeade_Next_Level::Check_Event_Cliff(_float fTimeDelta)
{
	if (true == m_isCheck_Cliff)
		return;

	if (true == m_pNext_Level_Cliff->Is_Collision())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_CLIFFSIDE);
		Safe_Release(pGameInstance);
		m_isCheck_Cliff = true;
	}
}

void CEvent_Smeade_Next_Level::Check_Event_Sanctum(_float fTimeDelta)
{
	if (true == m_isCheck_Sanctum)
		return;

	if (true == m_pNext_Level_Sanctum->Is_Collision())
	{
		BEGININSTANCE;

		if (false == g_isPensiveDead)
		{
			pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_VAULT);
		}

		else
		{
			pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_SANCTUM);
		}

		m_isCheck_Sanctum = true;
		ENDINSTANCE;
	}
}

void CEvent_Smeade_Next_Level::Check_Event_Sky(_float fTimeDelta)
{
	if (true == m_isCheck_Sky)
		return;

	if (true == m_pNext_Level_Sky->Is_Collision())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_SKY);
		Safe_Release(pGameInstance);

		m_isCheck_Sky = true;
	}
}

CEvent_Smeade_Next_Level* CEvent_Smeade_Next_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Smeade_Next_Level* pInstance = New CEvent_Smeade_Next_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Smeade_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CEvent_Smeade_Next_Level::Clone(void* pArg)
{
	CEvent_Smeade_Next_Level* pInstance = New CEvent_Smeade_Next_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Smeade_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Smeade_Next_Level::Free()
{
	__super::Free();

	Safe_Release(m_pNext_Level_Cliff);
	Safe_Release(m_pNext_Level_Sanctum);
	Safe_Release(m_pNext_Level_Sky);
}