#include "Event_Spawn_Dragon_2.h"
#include "GameInstance.h"

#include "ConjuredDragon.h"
#include "Trigger.h"
#include "Santum_MapEffects.h"

#include "Player.h"
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

	pGameInstance->Add_Timer(TEXT("Spawn_Dragon_2_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Spawn_Dragon_2_CutScene_Play"), false, 3.0f);

	pGameInstance->Read_CutSceneCamera(TEXT("Spawn_Dragon_2"), TEXT("../../Resources/GameData/CutScene/Sanctum_Dragon_Enter2.cut"));

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
	BEGININSTANCE;

	switch (m_eSequence)
	{
	case Client::CEvent_Spawn_Dragon_2::SPAWN_DRAGON_2_SEQUENCE_FADE_OUT:
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
		if (true == pGameInstance->Check_Timer(TEXT("Spawn_Dragon_2_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eSequence = SPAWN_DRAGON_2_SEQUENCE_PLAY_CUTSCENE;
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon_2::SPAWN_DRAGON_2_SEQUENCE_PLAY_CUTSCENE:
	{
		//진입시
		if (true == m_isEnter)
		{
			//페이드 인
			m_pRenderer->FadeIn(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Spawn_Dragon_2_CutScene_Play"));
			//진입 표시
			m_isEnter = false;
			//컷씬 재생
			pGameInstance->Add_CutScene(TEXT("Spawn_Dragon_2"));

			m_pDragon->Spawn();

			pGameInstance->Play_Particle(TEXT("Particle_CircularRocks02"), _float3(-33.933f, -61.301f, 230.270f));
			CSantum_MapEffects* pSanctumMapEffects = static_cast<CSantum_MapEffects*>(pGameInstance->Find_Component_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Particle")
				, TEXT("GameObject_Sanctum_MapEffets")));
			pSanctumMapEffects->Set_Phase2();
		}
		//타이머 종료
		if (true == pGameInstance->Check_Timer(TEXT("Spawn_Dragon_2_CutScene_Play")))
		{
			m_eSequence = SPAWN_DRAGON_2_SEQUENCE_FADE_IN;

			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Spawn_Dragon_2_CutScene_Fade_Out"));

			m_isEnter = true;
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon_2::SPAWN_DRAGON_2_SEQUENCE_FADE_IN:
	{
		//진입시
		if (true == m_isEnter)
		{
			//진입 표시
			m_isEnter = false;
		}

		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Spawn_Dragon_2_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eSequence = SPAWN_DRAGON_2_SEQUENCE_END;

			//페이드 인
			m_pRenderer->FadeIn(1.0f);

			//플레이어 위치 변경
			CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
			//static_cast<CCard_Fig*>(pPlayer->Find_Component(TEXT("Card_Fig")))->Set_CreateDragon();

			pPlayer->Get_Transform()->Set_Position(_float3(-45.823f, -60.442f, 251.705f));
			pPlayer->Get_Transform()->Rotation(_float3(0.0f, 1.0f, 0.0f), XMConvertToRadians(135.0f));
			pPlayer->Get_Player_Camera_Transform()->Rotation(_float3(0.0f, 1.0f, 0.0f), XMConvertToRadians(150.0f));
		}
	}
		break;
	case Client::CEvent_Spawn_Dragon_2::SPAWN_DRAGON_2_SEQUENCE_END:
	{
		if (true == m_isSpawned)
			return;

		if (true == m_pTrigger_Spawn->Is_Collision())
		{
			

			if (m_pTrigger_Spawn->isDead())
				m_isSpawned = true;

			

			m_isEnter = true;
			m_eSequence = SPAWN_DRAGON_2_SEQUENCE_FADE_OUT;
			pGameInstance->Reset_Timer(TEXT("Spawn_Dragon_2_CutScene_Fade_Out"));
		}
	}
		break;
	default:
		break;
	}


	ENDINSTANCE;
	
}

HRESULT CEvent_Spawn_Dragon_2::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}

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
	Safe_Release(m_pRenderer);
}