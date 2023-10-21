#include "Event_Smeade.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

CEvent_Smeade::CEvent_Smeade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Smeade::CEvent_Smeade(const CEvent_Smeade& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Smeade::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SMITH, TEXT("Layer_Monster"));
	
	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;
		// 나는 스폰 관련 데이터만 가져와서 처리할거야.
		if (wstring::npos != wstrObjTag.find(TEXT("Troll")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	//컷씬 준비
	pGameInstance->Read_CutSceneCamera(TEXT("Troll_Enter"), TEXT("../../Resources/GameData/CutScene/Troll_Enter.cut"));

	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play"), false, 14.428);

	ENDINSTANCE;
	
	return S_OK;
}

void CEvent_Smeade::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event_Spawn_Troll();

	//Check_Event_Play_Test_CutScene();
}

void CEvent_Smeade::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Smeade::Check_Event_Spawn_Troll()
{
	BEGININSTANCE;

	switch (m_eTroll_Spawn_Sequence)
	{
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_FADE_OUT:
	{
		//진입시
		if (true == m_isEnter)
		{
			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
			
			//진입 표시
			m_isEnter = false;
		}

		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_TROLL_SPAWN_AND_PLAY_CUTSCENE;

		}
	}
		break;
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_TROLL_SPAWN_AND_PLAY_CUTSCENE:
	{	
		//진입시
		if (true == m_isEnter)
		{
			//스폰 처리
			for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end(); )
			{
				wstring wstrObjectTag = iter->first;
				if (wstring::npos != wstrObjectTag.find(TEXT("Troll")))
				{
					iter->second->Spawn();
					Safe_Release(iter->second);
					iter = m_pMonsters.erase(iter);
				}
				else
					++iter;
			}
			//페이드 인
			m_pRenderer->FadeIn(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play"));
			//진입 표시
			m_isEnter = false;
			//컷씬 재생
			pGameInstance->Add_CutScene(TEXT("Troll_Enter"));
		}

		//타이머 종료
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play")))
		{
			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_FADE_IN;

			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out"));

			m_isEnter = true;
		}
	}	
	break;
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_FADE_IN:
	{
		//진입시
		if (true == m_isEnter)
		{
			//진입 표시
			m_isEnter = false;
		}

		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_END;

			//페이드 인
			m_pRenderer->FadeIn(1.0f);
		}
	}
		break;
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_END:
	{
		if (true == m_isEnter)
			break;

		if (true == m_pSpawn_Troll->Is_Collision())
		{
			if (m_pSpawn_Troll->isDead())
			{
				m_isEnter = true;
				m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_FADE_OUT;
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out"));
			}
		}
	}
		break;
	default:
		break;
	}

	ENDINSTANCE;
}

void CEvent_Smeade::Check_Event_Play_Test_CutScene()
{
	if (true == m_isPlayTestCutScene)
		return;

	if (true == m_pCutSceneTest->Is_Collision())
	{
		BEGININSTANCE;

		pGameInstance->Add_CutScene(TEXT("CutSceneTest"));

		ENDINSTANCE;

		if (m_pCutSceneTest->isDead())
			m_isPlayTestCutScene = true;
	}
}

HRESULT CEvent_Smeade::Add_Components()
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
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Spawn_Troll");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(15.f, 15.f, 15.f);
	TriggerDesc.vTriggerWorldPos = _float3(125.f, 2.f, 110.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn_Troll"), reinterpret_cast<CComponent**>(&m_pSpawn_Troll), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Smeade Failed Add_Components : Trigger_Spawn_Troll");
		return E_FAIL;
	}


	/* For.CutSceneTest */
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_CutSceneTest");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(15.f, 15.f, 15.f);
	TriggerDesc.vTriggerWorldPos = _float3(50.0f, 5.f, 60.f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_CutSceneTest"), reinterpret_cast<CComponent**>(&m_pCutSceneTest), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Smeade Failed Add_Components : Trigger_CutSceneTest");
		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Read_CutSceneCamera(TEXT("CutSceneTest"), TEXT("../../Resources/GameData/CutScene/DurationTest2.cut"));

	ENDINSTANCE;


	return S_OK;
}

CEvent_Smeade* CEvent_Smeade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Smeade* pInstance = New CEvent_Smeade(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Smeade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Smeade* CEvent_Smeade::Clone(void* pArg)
{
	CEvent_Smeade* pInstance = New CEvent_Smeade(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Smeade");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Smeade::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pSpawn_Troll);
		Safe_Release(m_pCutSceneTest);
		Safe_Release(m_pRenderer);

		for (auto& Pair : m_pMonsters)
			Safe_Release(Pair.second);
	}
}