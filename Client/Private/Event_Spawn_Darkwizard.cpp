#include "Event_Spawn_Darkwizard.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

#include "Player.h"

CEvent_Spawn_Darkwizard::CEvent_Spawn_Darkwizard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Spawn_Darkwizard::CEvent_Spawn_Darkwizard(const CEvent_Spawn_Darkwizard& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Spawn_Darkwizard::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"));
	
	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;
		// 나는 스폰 관련 데이터만 가져와서 처리할거야.
		if (wstring::npos != wstrObjTag.find(TEXT("Darkwizard")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	//컷씬 준비

	//pGameInstance->Read_CutSceneCamera(TEXT("Sanctum_Egg"), TEXT("../../Resources/GameData/CutScene/Sanctum_Egg.cut"));
	//pGameInstance->Read_CutSceneCamera(TEXT("Sanctum_Darkwizard_Enter"), TEXT("../../Resources/GameData/CutScene/Sanctum_Darkwizard_Enter.cut"));
	//
	//pGameInstance->Add_Timer(TEXT("Sanctum_CutScene_Fade_Out"), false, 1.0f);
	//pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play"), false, 10.1f);
	//pGameInstance->Add_Timer(TEXT("Sanctum_Darkwizard_Enter_CutScene_Play"), false, 8.7f);
	//pGameInstance->Add_Timer(TEXT("Sanctum_Darkwizard_Enter_Scream"), false, 5.333f);

	ENDINSTANCE;
	
	return S_OK;
}

void CEvent_Spawn_Darkwizard::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Event_Spawn_Darkwizard();
}

void CEvent_Spawn_Darkwizard::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Spawn_Darkwizard::Check_Event_Spawn_Darkwizard()
{
	BEGININSTANCE;

	switch (m_eDarkwizard_Spawn_Sequence)
	{
	case Client::CEvent_Spawn_Darkwizard::DARKWIZARDSPAWN_SEQUENCE_FADE_OUT:
		break;
	case Client::CEvent_Spawn_Darkwizard::DARKWIZARDSPAWN_SEQUENCE_PLAYCUTSCENE:
		break;
	case Client::CEvent_Spawn_Darkwizard::DARKWIZARDSPAWN_SEQUENCE_FADE_IN:
		break;
	case Client::CEvent_Spawn_Darkwizard::DARKWIZARDSPAWN_SEQUENCE_END:
	{
		if (true == m_isEnter)
			break;

		if (true == m_pSpawn_Darkwizard->Is_Collision())
		{
			if (m_pSpawn_Darkwizard->isDead())
			{
				m_isEnter = true;
				m_eDarkwizard_Spawn_Sequence = DARKWIZARDSPAWN_SEQUENCE_FADE_OUT;
				//타이머 리셋
				//pGameInstance->Reset_Timer(TEXT("Sanctum_CutScene_Fade_Out"));
			}
		}
	}
		break;
	default:
		break;
	}


	ENDINSTANCE;
}

HRESULT CEvent_Spawn_Darkwizard::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* For.Trigger_Spawn_1 */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Spawn_Darkwizard");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(5.f, 5.f, 5.f);
	TriggerDesc.vTriggerWorldPos = _float3(1.836f, 0.0f, 22.0f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn_Darkwizard"), reinterpret_cast<CComponent**>(&m_pSpawn_Darkwizard), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Spawn_Darkwizard Failed Add_Components : Trigger_Spawn_Darkwizard");
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Spawn_Darkwizard* CEvent_Spawn_Darkwizard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Spawn_Darkwizard* pInstance = New CEvent_Spawn_Darkwizard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Spawn_Darkwizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Spawn_Darkwizard* CEvent_Spawn_Darkwizard::Clone(void* pArg)
{
	CEvent_Spawn_Darkwizard* pInstance = New CEvent_Spawn_Darkwizard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Spawn_Darkwizard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Spawn_Darkwizard::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pSpawn_Darkwizard);
		Safe_Release(m_pRenderer);

		for (auto& Pair : m_pMonsters)
			Safe_Release(Pair.second);
	}
}