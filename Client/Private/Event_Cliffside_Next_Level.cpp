#include "..\Public\Event_Cliffside_Next_Level.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Cliff_Gate.h"
#include "Trigger.h"

CEvent_Cliffside_Next_Level::CEvent_Cliffside_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Cliffside_Next_Level::CEvent_Cliffside_Next_Level(const CEvent_Cliffside_Next_Level& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Cliffside_Next_Level::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	auto pMapObjectLayer = pGameInstance->Find_Components_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pMapObjectLayer)
	{
		wstring wstrObjTag = Pair.first;
		// Cliff_Gate를 찾는다.
		if (wstring::npos != wstrObjTag.find(TEXT("Cliff_Gate")))
		{
			m_pCliff_Gate = static_cast<CCliff_Gate*>(Pair.second);
			Safe_AddRef(m_pCliff_Gate);
		}
	}

	pGameInstance->Add_Timer(TEXT("Cliffside_Gate_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Cliffside_Gate_CutScene_Play"), false, 7.6f);

	pGameInstance->Add_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_Out"), false, 2.3f);
	pGameInstance->Add_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_In"), false, 2.5f);

	pGameInstance->Read_CutSceneCamera(TEXT("Cliffside_Gate"), TEXT("../../Resources/GameData/CutScene/Cliffside_Gate.cut"));

	return S_OK;
}

void CEvent_Cliffside_Next_Level::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event(fTimeDelta);
}

void CEvent_Cliffside_Next_Level::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEvent_Cliffside_Next_Level::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* For.Trigger_Cliff_Next_Level */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Cliff_Next_Level_Gate");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(1.f, 2.5f, 1.f);
	TriggerDesc.vTriggerWorldPos = _float3(72.5f, 8.75f, 90.5f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Cliff_Next_Level"), reinterpret_cast<CComponent**>(&m_pNext_Level), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Cliffside_Next_Level Failed Add_Components : Trigger_Cliff_Next_Level");
		return E_FAIL;
	}

	return S_OK;
}

void CEvent_Cliffside_Next_Level::Check_Event(_float fTimeDelta)
{
	if (true == m_isCheck)
		return;

	// 여기다가 컷신 작업해주시면 됩니다 현우형
	if (true == m_pCliff_Gate->Get_All_LightStands_On())
	{
		// Cliff_Gate Reparo 실행시켜 주시려면 아래 함수 쓰시면 됩니다.
		// m_pCliff_Gate->Set_Gate_Reparo_On();

		BEGININSTANCE;

		switch (m_eSequence)
		{
		case Client::CEvent_Cliffside_Next_Level::GATECUTSCENE_SEQUENCE_FADE_OUT:
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
			if (true == pGameInstance->Check_Timer(TEXT("Cliffside_Gate_CutScene_Fade_Out")))
			{
				m_isEnter = true;

				m_eSequence = GATECUTSCENE_SEQUENCE_PLAY_CUTSCENE;
			}
		}
		break;
		case Client::CEvent_Cliffside_Next_Level::GATECUTSCENE_SEQUENCE_PLAY_CUTSCENE:
		{
			//진입시
			if (true == m_isEnter)
			{
				//페이드 인
				m_pRenderer->FadeIn(1.0f);
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Cliffside_Gate_CutScene_Play"));
				pGameInstance->Reset_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_Out"));
				pGameInstance->Reset_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_In"));
				//진입 표시
				m_isEnter = false;
				//컷씬 재생
				pGameInstance->Add_CutScene(TEXT("Cliffside_Gate"));

				
			}

			if (false == m_isFade[0] && true == pGameInstance->Check_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_Out")))
			{
				m_isFade[0] = true;
				m_pRenderer->FadeOut(5.0f);
			}

			if (false == m_isFade[1] && true == pGameInstance->Check_Timer(TEXT("Cliffside_Gate_CutScene_Play_Fade_In")))
			{
				m_isFade[1] = true;
				m_pRenderer->FadeIn(5.0f);

				m_pCliff_Gate->Set_Gate_Reparo_On();
			}

			//타이머 종료
			if (true == pGameInstance->Check_Timer(TEXT("Cliffside_Gate_CutScene_Play")))
			{
				m_eSequence = GATECUTSCENE_SEQUENCE_FADE_IN;

				//페이드 아웃
				m_pRenderer->FadeOut(1.0f);
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Cliffside_Gate_CutScene_Fade_Out"));

				m_isEnter = true;
			}
		}
		break;
		case Client::CEvent_Cliffside_Next_Level::GATECUTSCENE_SEQUENCE_FADE_IN:
		{
			//진입시
			if (true == m_isEnter)
			{
				//진입 표시
				m_isEnter = false;
			}

			//타이머 체크
			if (true == pGameInstance->Check_Timer(TEXT("Cliffside_Gate_CutScene_Fade_Out")))
			{
				m_isEnter = true;

				m_eSequence = GATECUTSCENE_SEQUENCE_END;

				//페이드 인
				m_pRenderer->FadeIn(1.0f);
			}
		}
			break;
		case Client::CEvent_Cliffside_Next_Level::GATECUTSCENE_SEQUENCE_END:
		{
			if (true == m_isEnter)
				break;

			if (false == m_isGateCutsceneStart)
			{
				m_isGateCutsceneStart = true;
				m_isEnter = true;
				m_eSequence = GATECUTSCENE_SEQUENCE_FADE_OUT;
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Cliffside_Gate_CutScene_Fade_Out"));
			}
		}
			break;
		default:
			break;
		}

		ENDINSTANCE;
	}

	if (true == m_pNext_Level->Is_Collision())
	{
		if (nullptr == m_pCliff_Gate)
		{
			MSG_BOX("Failed to Find Cliff_Gate");
			return;
		}

		_bool isGateOpen = m_pCliff_Gate->Get_GateOpen();

		if (true == isGateOpen)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			pGameInstance->Get_CurrentLevel()->Set_NextLevel(LEVEL_SMITH);
			Safe_Release(pGameInstance);

			m_isCheck = true;
		}
			
	}
}

CEvent_Cliffside_Next_Level* CEvent_Cliffside_Next_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Cliffside_Next_Level* pInstance = New CEvent_Cliffside_Next_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Cliffside_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEvent_Cliffside_Next_Level::Clone(void* pArg)
{
	CEvent_Cliffside_Next_Level* pInstance = New CEvent_Cliffside_Next_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Cliffside_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Cliffside_Next_Level::Free()
{
	__super::Free();

	Safe_Release(m_pNext_Level);
	Safe_Release(m_pCliff_Gate);
	Safe_Release(m_pRenderer);

}