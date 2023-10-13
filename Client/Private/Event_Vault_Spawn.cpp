#include "Event_Vault_Spawn.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

CEvent_Vault_Spawn::CEvent_Vault_Spawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Vault_Spawn::CEvent_Vault_Spawn(const CEvent_Vault_Spawn& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Vault_Spawn::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_VAULT, TEXT("Layer_Monster"));
	ENDINSTANCE;
	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;
		// 나는 스폰 관련 데이터만 가져와서 처리할거야.
		if (wstring::npos != wstrObjTag.find(TEXT("Round")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	return S_OK;
}

void CEvent_Vault_Spawn::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Spawn_1();
	Check_Event_Spawn_2();
}

void CEvent_Vault_Spawn::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Vault_Spawn::Check_Event_Spawn_1()
{
	if (true == m_isSpawned_1)
		return;

	if (true == m_pSpawn_Stage_1->Is_Collision())
	{
		for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end(); )
		{
			wstring wstrObjectTag = iter->first;
			if (wstring::npos != wstrObjectTag.find(TEXT("Round1")))
			{
				iter->second->Spawn();
				Safe_Release(iter->second);
				iter = m_pMonsters.erase(iter);
			}
			else
				++iter;
		}

		if (m_pSpawn_Stage_1->isDead())
			m_isSpawned_1 = true;
	}
}

void CEvent_Vault_Spawn::Check_Event_Spawn_2()
{
	if (true == m_isSpawned_2)
		return;

	if (true == m_pSpawn_Stage_2->Is_Collision())
	{
		for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end(); )
		{
			wstring wstrObjectTag = iter->first;
			if (wstring::npos != wstrObjectTag.find(TEXT("Round2")))
			{
				iter->second->Spawn();
				Safe_Release(iter->second);
				iter = m_pMonsters.erase(iter);
			}
			else
				++iter;
		}

		if (m_pSpawn_Stage_2->isDead())
			m_isSpawned_2 = true;
	}
}

HRESULT CEvent_Vault_Spawn::Add_Components()
{
	/* For.Trigger_Spawn_1 */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Vault_Spawn_1");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(15.f, 15.f, 15.f);
	TriggerDesc.vTriggerWorldPos = _float3(50.f, -3.5f, 53.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn_1"), reinterpret_cast<CComponent**>(&m_pSpawn_Stage_1), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Vault_Spawn Failed Add_Components : Trigger_Spawn_1");
		return E_FAIL;
	}

	/* For.Trigger_Spawn_2 */
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Vault_Spawn_2");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(15.f, 15.f, 15.f);
	TriggerDesc.vTriggerWorldPos = _float3(103.f, -3.f, 104.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn_2"), reinterpret_cast<CComponent**>(&m_pSpawn_Stage_2), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Vault_Spawn Failed Add_Components : Trigger_Spawn_2");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Vault_Spawn* CEvent_Vault_Spawn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Vault_Spawn* pInstance = New CEvent_Vault_Spawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Vault_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Vault_Spawn* CEvent_Vault_Spawn::Clone(void* pArg)
{
	CEvent_Vault_Spawn* pInstance = New CEvent_Vault_Spawn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Vault_Spawn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Vault_Spawn::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pSpawn_Stage_1);
		Safe_Release(m_pSpawn_Stage_2);

		for (auto& Pair : m_pMonsters)
			Safe_Release(Pair.second);
	}
}