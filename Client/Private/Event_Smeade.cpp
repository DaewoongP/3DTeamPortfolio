#include "Event_Smeade.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"
#include "Player.h"

#include "Armored_Troll.h"
#include "FireHouse.h"

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
		// ���� ���� ���� �����͸� �����ͼ� ó���Ұž�.
		if (wstring::npos != wstrObjTag.find(TEXT("Troll")))
		{
			if (nullptr == m_pTroll)
			{
				m_pTroll = static_cast<CArmored_Troll*>(Pair.second);
				Safe_AddRef(m_pTroll);
			}
		}
	}

	// ���̾� �Ͽ콺 ó��
	auto pMapLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SMITH, TEXT("Layer_BackGround"));

	for (auto Pair : *pMapLayer)
	{
		wstring wstrObjTag = Pair.first;
		if (wstring::npos != wstrObjTag.find(TEXT("FireHouse")))
		{
			m_pFireHouse.push_back(static_cast<CFireHouse*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	//�ƾ� �غ�
	pGameInstance->Read_CutSceneCamera(TEXT("Troll_Enter"), TEXT("../../Resources/GameData/CutScene/Troll_Enter.cut"));

	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out"), false, 1.0f);
	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play"), false, 14.428f);

	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeOut"), true, 9.466f);
	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeIn"), true, 9.666f);

	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeOut"), true, 10.866f);
	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeIn"), true, 11.066f);

	pGameInstance->Add_Timer(TEXT("Troll_Spawn_CutScene_Play_4_Scream"), true, 13.3f);

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
		//���Խ�
		if (true == m_isEnter)
		{
			_int ChennelNum = pGameInstance->Play_BGM(TEXT("HogSmead_Boss_Bgm.wav"), 0.6f);
			m_pTroll->Set_Boss_BGM_Chennel_Number(ChennelNum);
			//���̵� �ƿ�
			m_pRenderer->FadeOut(1.0f);

			//���� ǥ��
			m_isEnter = false;
		}

		//Ÿ�̸� üũ
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_TROLL_SPAWN_AND_PLAY_CUTSCENE;

		}
	}
	break;
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_TROLL_SPAWN_AND_PLAY_CUTSCENE:
	{
		//���Խ�
		if (true == m_isEnter)
		{
			//���� ó��
					// ���⼭ Ʈ���� ���� ������ ó��
			m_pTroll->Set_Weapon_Render(true);
			m_pTroll->Spawn();
			Safe_Release(m_pTroll);
			m_pTroll = nullptr;

			// �ƽſ� ���� ���� �� 
			for (auto& iter : m_pFireHouse)
			{
				iter->Set_FireOn();
			}

			//���̵� ��
			m_pRenderer->FadeIn(1.0f);
			//Ÿ�̸� ����
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play"));
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeOut"));
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeIn"));
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeOut"));
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeIn"));
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Play_4_Scream"));
			//���� ǥ��
			m_isEnter = false;
			//�ƾ� ���
			pGameInstance->Add_CutScene(TEXT("Troll_Enter"));
		}

		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeOut")))
		{
			m_pRenderer->FadeOut(5.0f);
		}

		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play_2_FadeIn")))
		{
			m_pRenderer->FadeIn(5.0f);
		}

		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeOut")))
		{
			m_pRenderer->FadeOut(5.0f);
		}

		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play_3_FadeIn")))
		{
			m_pRenderer->FadeIn(5.0f);
		}

		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play_4_Scream")))
		{
			m_pRenderer->Set_ScreenRadial(false, 1.0f, 0.2f);
		}

		//Ÿ�̸� ����
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Play")))
		{
			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_FADE_IN;

			//���̵� �ƿ�
			m_pRenderer->FadeOut(1.0f);
			//Ÿ�̸� ����
			pGameInstance->Reset_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out"));

			m_isEnter = true;
		}
	}
	break;
	case Client::CEvent_Smeade::TROLLSPAWN_SEQUENCE_FADE_IN:
	{
		//���Խ�
		if (true == m_isEnter)
		{
			//���� ǥ��
			m_isEnter = false;
		}



		//Ÿ�̸� üũ
		if (true == pGameInstance->Check_Timer(TEXT("Troll_Spawn_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_END;

			//���̵� ��
			m_pRenderer->FadeIn(1.0f);

			//�÷��̾� ��ġ ����
			CPlayer* pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));

			pPlayer->Get_Transform()->Set_Position(_float3(115.0f, 9.0f, 120.0f));
			pPlayer->Get_Transform()->Rotation(_float3(0.0f, 1.0f, 0.0f), -XMConvertToRadians(90.0f));
			pPlayer->Get_Player_Camera_Transform()->Rotation(_float3(0.0f, 1.0f, 0.0f), -XMConvertToRadians(90.0f));
			//pPlayer->Get_Player_Camera_Transform()->Turn(_float3(0.0f, 0.0f, 1.0f), XMConvertToRadians(15.0f));
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
				pGameInstance->Stop_AllSound();
				m_isEnter = true;
				m_eTroll_Spawn_Sequence = TROLLSPAWN_SEQUENCE_FADE_OUT;
				//Ÿ�̸� ����
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
	TriggerDesc.vTriggerSize = _float3(4.f, 1.f, 1.f);
	TriggerDesc.vTriggerWorldPos = _float3(126.f, 8.5f, 96.2f);

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

		Safe_Release(m_pTroll);

		for (auto& iter : m_pFireHouse)
			Safe_Release(iter);
	}
}