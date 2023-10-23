#include "Event_Spawn.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

CEvent_Spawn::CEvent_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Spawn::CEvent_Spawn(const CEvent_Spawn& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Spawn::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("[CEvent_Spawn] pArg is nullptr");
		return E_FAIL;
	}
	INITEVENTSPAWM* pInitDesc = reinterpret_cast<INITEVENTSPAWM*>(pArg);

	FAILED_CHECK_RETURN(Add_Components(*pInitDesc), E_FAIL);

	BEGININSTANCE;
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(pInitDesc->eMonsterLevel, TEXT("Layer_Monster"));
	ENDINSTANCE;

	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;

		if (wstring::npos != wstrObjTag.find(pInitDesc->wstrSpawnEnemyComponentTag))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	return S_OK;
}

void CEvent_Spawn::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Spawn();
}

void CEvent_Spawn::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Spawn::Check_Event_Spawn()
{
	if (true == m_isSpawned)
		return;

	if (true == m_pTrigger_Spawn->Is_Collision())
	{
		for (auto Pair : m_pMonsters)
			Pair.second->Spawn();

		if (m_pTrigger_Spawn->isDead())
			m_isSpawned = true;
	}
}

HRESULT CEvent_Spawn::Add_Components(const INITEVENTSPAWM& InitDesc)
{
	/* For.Trigger_Spawn */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Sanctum_Spawn");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = InitDesc.vTriggerSize;
	TriggerDesc.vTriggerWorldPos = InitDesc.vTriggerWorldPosition;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn"), reinterpret_cast<CComponent**>(&m_pTrigger_Spawn), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Spawn Failed Add_Components : Trigger_Spawn");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Spawn* CEvent_Spawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Spawn* pInstance = New CEvent_Spawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Spawn* CEvent_Spawn::Clone(void* pArg)
{
	CEvent_Spawn* pInstance = New CEvent_Spawn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Spawn::Free()
{
	__super::Free();

	Safe_Release(m_pTrigger_Spawn);

	for (auto& Pair : m_pMonsters)
		Safe_Release(Pair.second);
}