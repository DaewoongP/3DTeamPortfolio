#include "Event_Spawn_Dragon.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

#include "Player.h"
#include "Card_Fig.h"

CEvent_Spawn_Dragon::CEvent_Spawn_Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Spawn_Dragon::CEvent_Spawn_Dragon(const CEvent_Spawn_Dragon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Spawn_Dragon::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"));
	
	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;
		// 나는 스폰 관련 데이터만 가져와서 처리할거야.
		if (wstring::npos != wstrObjTag.find(TEXT("Dragon")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	//컷씬 준비

	pGameInstance->Read_CutSceneCamera(TEXT("Sanctum_Egg"), TEXT("../../Resources/GameData/CutScene/Sanctum_Egg.cut"));
	pGameInstance->Read_CutSceneCamera(TEXT("Sanctum_Dragon_Enter"), TEXT("../../Resources/GameData/CutScene/Sanctum_Dragon_Enter.cut"));
	
	pGameInstance->Add_Timer(TEXT("Sanctum_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play"), false, 10.1f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Dragon_Enter_CutScene_Play"), false, 8.7f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Dragon_Enter_Scream"), false, 5.333f);

	pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeOut"), true, 2.8f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeIn"), true, 3.0f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeOut"), true, 5.83f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeIn"), true, 6.03f);

	ENDINSTANCE;
	
	return S_OK;
}

void CEvent_Spawn_Dragon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Event_Spawn_Dragon();
}

void CEvent_Spawn_Dragon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Spawn_Dragon::Check_Event_Spawn_Dragon()
{
	BEGININSTANCE;

	switch (m_eDragon_Spawn_Sequence)
	{
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_EGG_FADE_OUT:
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
		if (true == pGameInstance->Check_Timer(TEXT("Sanctum_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_EGG_PLAY_CUTSCENE;
			//컷씬 재생
			pGameInstance->Add_CutScene(TEXT("Sanctum_Egg"));
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Sanctum_Egg_CutScene_Play"));

			pGameInstance->Reset_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeOut"));
			pGameInstance->Reset_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeIn"));
			pGameInstance->Reset_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeOut"));
			pGameInstance->Reset_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeIn"));
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_EGG_PLAY_CUTSCENE:
	{
		//진입시
		if (true == m_isEnter)
		{
			//페이드 인
			m_pRenderer->FadeIn(1.0f);
			//진입 표시
			m_isEnter = false;
		}

		if (false == m_isEgg[0] && true == pGameInstance->Check_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeOut")))
		{
			m_pRenderer->FadeOut(5.0f);
			m_isEgg[0] = true;
		}

		if (false == m_isEgg[1] && true == pGameInstance->Check_Timer(TEXT("Sanctum_Egg_CutScene_Play_2_FadeIn")))
		{
			m_pRenderer->FadeIn(5.0f);
			m_isEgg[1] = true;
		}

		if (false == m_isEgg[2] && true == pGameInstance->Check_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeOut")))
		{
			m_pRenderer->FadeOut(5.0f);
			m_isEgg[2] = true;
		}

		if (false == m_isEgg[3] && true == pGameInstance->Check_Timer(TEXT("Sanctum_Egg_CutScene_Play_3_FadeIn")))
		{
			m_pRenderer->FadeIn(5.0f);
			m_isEgg[3] = true;
		}

		//타이머 종료
		if (true == pGameInstance->Check_Timer(TEXT("Sanctum_Egg_CutScene_Play")))
		{
			m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_FADE_OUT;

			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Sanctum_CutScene_Fade_Out"));
			
			m_isEnter = true;

			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_FADE_OUT:
	{
		//진입시
		if (true == m_isEnter)
		{
			//진입 표시
			m_isEnter = false;
		}
		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Sanctum_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_PLAY_CUTSCENE;
			//타이머 리셋
			//컷씬 재생
			pGameInstance->Reset_Timer(TEXT("Sanctum_Dragon_Enter_CutScene_Play"));
			//표효 타이머
			pGameInstance->Reset_Timer(TEXT("Sanctum_Dragon_Enter_Scream"));

			m_isScream = false;
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_DRAGON_ENTER_PLAY_CUTSCENE:
	{
		//진입시
		if (true == m_isEnter)
		{
			//스폰 처리
			for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end(); )
			{
				wstring wstrObjectTag = iter->first;
				if (wstring::npos != wstrObjectTag.find(TEXT("Dragon")))
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
			//진입 표시
			m_isEnter = false;
			//컷씬 재생
			pGameInstance->Add_CutScene(TEXT("Sanctum_Dragon_Enter"));
		}

		//표효 타이밍
		if (false == m_isScream && true == pGameInstance->Check_Timer(TEXT("Sanctum_Dragon_Enter_Scream")))
		{
			//레디얼 + 쉐이크 
			m_pRenderer->Set_ScreenRadial(false, 1.0f, 0.2f);

			pGameInstance->Set_Shake(
				CCamera_Manager::SHAKE_PRIORITY_1,
				CCamera_Manager::SHAKE_TYPE_TRANSLATION,
				CCamera_Manager::SHAKE_AXIS_LOOK,
				CEase::IN_EXPO,
				10.0f,
				0.8f,
				1.0f,
				CCamera_Manager::SHAKE_POWER_DECRECENDO);

			pGameInstance->Set_Shake(
				CCamera_Manager::SHAKE_PRIORITY_2,
				CCamera_Manager::SHAKE_TYPE_ROTATION,
				CCamera_Manager::SHAKE_AXIS_LOOK,
				CEase::IN_SINE,
				10.0f,
				0.8f,
				0.1f,
				CCamera_Manager::SHAKE_POWER_DECRECENDO);

			m_isScream = true;
		}

		//타이머 종료
		if (true == pGameInstance->Check_Timer(TEXT("Sanctum_Dragon_Enter_CutScene_Play")))
		{
			m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_DRAGON_FADE_IN;

			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Sanctum_CutScene_Fade_Out"));

			m_isEnter = true;
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_DRAGON_FADE_IN:
	{
		//진입시
		if (true == m_isEnter)
		{
			//진입 표시
			m_isEnter = false;
		}

		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Sanctum_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_END;

			//페이드 인
			m_pRenderer->FadeIn(1.0f);

			//플레이어 위치 변경
			CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
			static_cast<CCard_Fig*>(pPlayer->Find_Component(TEXT("Card_Fig")))->Set_CreateDragon();

			pPlayer->Get_Transform()->Set_Position(_float3(-43.170f,-33.315f,108.198f));
			pPlayer->Get_Transform()->Rotation(_float3(0.0f,1.0f,0.0f), XMConvertToRadians(45.0f));
			pPlayer->Get_Player_Camera_Transform()->Rotation(_float3(0.0f, 1.0f, 0.0f), XMConvertToRadians(60.0f));
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon::DRAGONSPAWN_SEQUENCE_END:
	{
		if (true == m_isEnter)
			break;

		if (true == m_pSpawn_Dragon->Is_Collision())
		{
			if (m_pSpawn_Dragon->isDead())
			{
				m_isEnter = true;
				m_eDragon_Spawn_Sequence = DRAGONSPAWN_SEQUENCE_EGG_FADE_OUT;
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Sanctum_CutScene_Fade_Out"));
			}
		}
	}
		break;
	default:
		break;
	}


	ENDINSTANCE;
}

HRESULT CEvent_Spawn_Dragon::Add_Components()
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
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Spawn_Dragon");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(15.f, 15.f, 15.f);
	TriggerDesc.vTriggerWorldPos = _float3(-3.0f, 2.f, 130.0f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Spawn_Dragon"), reinterpret_cast<CComponent**>(&m_pSpawn_Dragon), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Spawn_Dragon Failed Add_Components : Trigger_Spawn_Dragon");
		return E_FAIL;
	}


	


	return S_OK;
}

CEvent_Spawn_Dragon* CEvent_Spawn_Dragon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Spawn_Dragon* pInstance = New CEvent_Spawn_Dragon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Spawn_Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Spawn_Dragon* CEvent_Spawn_Dragon::Clone(void* pArg)
{
	CEvent_Spawn_Dragon* pInstance = New CEvent_Spawn_Dragon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Spawn_Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Spawn_Dragon::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pSpawn_Dragon);
		Safe_Release(m_pRenderer);

		for (auto& Pair : m_pMonsters)
			Safe_Release(Pair.second);
	}
}