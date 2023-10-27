#include "Event_Spawn_Dragon_2.h"
#include "GameInstance.h"

#include "ConjuredDragon.h"
#include "Trigger.h"

CEvent_Spawn_Dragon_2::CEvent_Spawn_Dragon_2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Spawn_Dragon_2::CEvent_Spawn_Dragon_2(const CEvent_Spawn_Dragon_2& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Spawn_Dragon_2::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;

	m_pDragon = static_cast<CConjuredDragon*>(pGameInstance->Find_Component_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"), TEXT("GameObject_ConjuredDragon")));
	if (nullptr == m_pDragon)
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	Safe_AddRef(m_pDragon);

	ENDINSTANCE;

	return S_OK;
}

void CEvent_Spawn_Dragon_2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Spawn_Dragon_2();
}

void CEvent_Spawn_Dragon_2::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Spawn_Dragon_2::Check_Event_Spawn_Dragon_2()
{
	if (true == m_isSpawned)
		return;

	if (true == m_pTrigger_Spawn->Is_Collision())
	{
		m_pDragon->Spawn();

		if (m_pTrigger_Spawn->isDead())
			m_isSpawned = true;

		BEGININSTANCE;
		pGameInstance->Play_Particle(TEXT("Particle_CircularRocks02"), _float3(-33.933f, -61.301f, 230.270f));
		ENDINSTANCE;
	}
}

HRESULT CEvent_Spawn_Dragon_2::Add_Components()
{
	/* For.Trigger_Spawn */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Sanctum_Spawn");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(20.f, 10.f, 5.f);
	TriggerDesc.vTriggerWorldPos = _float3(-37.f, -54.f, 203.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn"), reinterpret_cast<CComponent**>(&m_pTrigger_Spawn), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Spawn_Dragon_2 Failed Add_Components : Trigger_Spawn");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Spawn_Dragon_2* CEvent_Spawn_Dragon_2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Spawn_Dragon_2* pInstance = New CEvent_Spawn_Dragon_2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Spawn_Dragon_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Spawn_Dragon_2* CEvent_Spawn_Dragon_2::Clone(void* pArg)
{
	CEvent_Spawn_Dragon_2* pInstance = New CEvent_Spawn_Dragon_2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Spawn_Dragon_2");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Spawn_Dragon_2::Free()
{
	__super::Free();

	Safe_Release(m_pTrigger_Spawn);
	Safe_Release(m_pDragon);
}