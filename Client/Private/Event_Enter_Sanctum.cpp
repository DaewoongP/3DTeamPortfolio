#include "Event_Enter_Sanctum.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Player.h"
#include "Trigger.h"
#include "Card_Fig.h"
#include "LightStand.h"
#include "Sanctum_Door.h"

CEvent_Enter_Sanctum::CEvent_Enter_Sanctum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Enter_Sanctum::CEvent_Enter_Sanctum(const CEvent_Enter_Sanctum& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Enter_Sanctum::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	/* Set Monsters */
	BEGININSTANCE;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	if (nullptr == pPlayer)
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pCard_Fig = pPlayer->m_pCard_Fig;
	Safe_AddRef(m_pCard_Fig);

	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"));

	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;

		// 스폰하는 몬스터 태그입니다 오해 ㄴㄴ
		if (wstring::npos != wstrObjTag.find(TEXT("Torch")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	/* Set Light Stands And Sanctum Doors */
	auto pBackGroundLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_BackGround"));

	for (auto Pair : *pBackGroundLayer)
	{
		wstring wsObjTag = Pair.first;

		if (wstring::npos != wsObjTag.find(TEXT("LightStand")))
		{
			m_pLightStands.push_back(static_cast<CLightStand*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
		else if (wstring::npos != wsObjTag.find(TEXT("Sanctum_Door")))
		{
			m_pDoors.push_back(static_cast<CSanctum_Door*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	pGameInstance->Add_Timer(TEXT("Sanctum_Gate_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Sanctum_Gate_CutScene_Play"), false, 5.2f);
	pGameInstance->Read_CutSceneCamera(TEXT("Sanctum_Gate"), TEXT("../../Resources/GameData/CutScene/Sanctum_Gate.cut"));


	ENDINSTANCE;

	return S_OK;
}

void CEvent_Enter_Sanctum::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(false == m_isSpawn)
		Check_Event_On_Torch_To_Spawn();

	else if (false == m_isDoorOpen)
		Check_Monsters_Are_Death();
}

void CEvent_Enter_Sanctum::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Enter_Sanctum::Check_Event_On_Torch_To_Spawn()
{
	for (auto iter = m_pLightStands.begin(); iter != m_pLightStands.end();)
	{
		if (true == (*iter)->Get_FireOn())
		{
			Safe_Release(*iter);
			iter = m_pLightStands.erase(iter);
		}
		else
			++iter;
	}

	if (0 == m_pLightStands.size())
	{
		for (auto& Pair : m_pMonsters)
			Pair.second->Spawn();
		m_isSpawn = true;
	}
}

void CEvent_Enter_Sanctum::Check_Monsters_Are_Death()
{
	for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end();)
	{
		if (true == iter->second->isDead())
		{
			Safe_Release(iter->second);
			iter = m_pMonsters.erase(iter);
		}
		else
			++iter;
	}

	if (0 == m_pMonsters.size())
	{
		// 여기서 컷신 작업 해주시면 됩니다 현우형

		BEGININSTANCE;

		switch (m_eSequence)
		{
		case Client::CEvent_Enter_Sanctum::GATECUTSCENE_SEQUENCE_FADE_OUT:
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
			if (true == pGameInstance->Check_Timer(TEXT("Sanctum_Gate_CutScene_Fade_Out")))
			{
				m_isEnter = true;

				m_eSequence = GATECUTSCENE_SEQUENCE_PLAY_CUTSCENE;
			}
		}
		break;
		case Client::CEvent_Enter_Sanctum::GATECUTSCENE_SEQUENCE_PLAY_CUTSCENE:
		{
			//진입시
			if (true == m_isEnter)
			{
				//페이드 인
				m_pRenderer->FadeIn(1.0f);
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Sanctum_Gate_CutScene_Play"));
				//진입 표시
				m_isEnter = false;
				//컷씬 재생
				pGameInstance->Add_CutScene(TEXT("Sanctum_Gate"));

				// 이 포문은 문을 여는 부분입니다.
				for (auto iter = m_pDoors.begin(); iter != m_pDoors.end();)
				{
					(*iter)->Open_Door();
					Safe_Release(*iter);
					iter = m_pDoors.erase(iter);
				}
			}

			//타이머 종료
			if (true == pGameInstance->Check_Timer(TEXT("Sanctum_Gate_CutScene_Play")))
			{
				m_eSequence = GATECUTSCENE_SEQUENCE_FADE_IN;

				//페이드 아웃
				m_pRenderer->FadeOut(1.0f);
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Sanctum_Gate_CutScene_Fade_Out"));

				m_isEnter = true;
			}
		}
		break;
		case Client::CEvent_Enter_Sanctum::GATECUTSCENE_SEQUENCE_FADE_IN:
		{
			//진입시
			if (true == m_isEnter)
			{
				//진입 표시
				m_isEnter = false;
			}

			//타이머 체크
			if (true == pGameInstance->Check_Timer(TEXT("Sanctum_Gate_CutScene_Fade_Out")))
			{
				m_isEnter = true;

				m_eSequence = GATECUTSCENE_SEQUENCE_END;

				//페이드 인
				m_pRenderer->FadeIn(1.0f);

				m_isDoorOpen = true; // 이 변수가 true가 되면 이 함수에 더이상 들어오지 않습니다.
				m_pCard_Fig->On_Enter_Sanctum_Script(); // 피그 교수의 스크립트를 출력하는 부분입니다.
			}
		}
		break;
		case Client::CEvent_Enter_Sanctum::GATECUTSCENE_SEQUENCE_END:
		{
			if (true == m_isEnter)
				break;

			if (false == m_isGateCutsceneStart)
			{
				m_isGateCutsceneStart = true;
				m_isEnter = true;
				m_eSequence = GATECUTSCENE_SEQUENCE_FADE_OUT;
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Sanctum_Gate_CutScene_Fade_Out"));
			}
		}
		break;
		default:
			break;
		}

		ENDINSTANCE;

		

		
		
	}
}

HRESULT CEvent_Enter_Sanctum::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

CEvent_Enter_Sanctum* CEvent_Enter_Sanctum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Enter_Sanctum* pInstance = New CEvent_Enter_Sanctum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Enter_Sanctum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Enter_Sanctum* CEvent_Enter_Sanctum::Clone(void* pArg)
{
	CEvent_Enter_Sanctum* pInstance = New CEvent_Enter_Sanctum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Enter_Sanctum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Enter_Sanctum::Free()
{
	__super::Free();

	Safe_Release(m_pCard_Fig);
	Safe_Release(m_pRenderer);

	for (auto& Pair : m_pMonsters)
		Safe_Release(Pair.second);

	for (auto& pLightStand : m_pLightStands)
		Safe_Release(pLightStand);

	for(auto& pDoor : m_pDoors)
		Safe_Release(pDoor);
}