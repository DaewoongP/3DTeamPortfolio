#include "Event_Cliffside.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"
#include "Professor_FIg.h"

CEvent_Cliffside::CEvent_Cliffside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Cliffside::CEvent_Cliffside(const CEvent_Cliffside& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Cliffside::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Cliffside_CutScene_Fade_Out"),false,1.0f);
	pGameInstance->Add_Timer(TEXT("Cliffside_CutScene_Play"),false,4.0f);

	/* Set Monsters */
	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_Monster"));

	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;

		if (wstring::npos != wstrObjTag.find(TEXT("Spawn")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	/* Set NPC */
	auto pNPCLayer = pGameInstance->Find_Components_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_NPC"));

	for (auto Pair : *pNPCLayer)
	{
		wstring wstrObjTag = Pair.first;

		if (wstring::npos != wstrObjTag.find(TEXT("Professor_Fig")))
		{
			CProfessor_Fig* pFig = static_cast<CProfessor_Fig*>(Pair.second);
			if (nullptr == pFig)
				continue;

			m_pFig = pFig;
			Safe_AddRef(m_pFig);
		}
	}

	ENDINSTANCE;

	return S_OK;
}

void CEvent_Cliffside::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Event_Play_CutScene_0_0();
}

void CEvent_Cliffside::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Cliffside::Check_Event_Play_CutScene_0_0()
{
	BEGININSTANCE;

	switch (m_eCliffside_Sequence)
	{
		//페이드 아웃
	case Client::CEvent_Cliffside::CLIFFSIDE_SEQUENCE_FADE_OUT:
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
		if (true == pGameInstance->Check_Timer(TEXT("Cliffside_CutScene_Fade_Out")))
		{
			m_isEnter = true;
			
			m_eCliffside_Sequence = CLIFFSIDE_SEQUENCE_PLAY_CUTSCENE;
			
		}
	}
		break;
		//컷씬 재생 및 페이드 인
	case Client::CEvent_Cliffside::CLIFFSIDE_SEQUENCE_PLAY_CUTSCENE:
	{
		//진입시
		if (true == m_isEnter)
		{
			//페이드 인
			m_pRenderer->FadeIn(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Cliffside_CutScene_Play"));
			//진입 표시
			m_isEnter = false;
			//컷씬 재생
			pGameInstance->Add_CutScene(TEXT("CutScene_0_0"));
		}

		//타이머 종료
		if (true == pGameInstance->Check_Timer(TEXT("Cliffside_CutScene_Play")))
		{
			m_eCliffside_Sequence = CLIFFSIDE_SEQUENCE_FADE_IN;
			
			//페이드 아웃
			m_pRenderer->FadeOut(1.0f);
			//타이머 리셋
			pGameInstance->Reset_Timer(TEXT("Cliffside_CutScene_Fade_Out"));
			
			m_isEnter = true;
		}
	}
	break;
	//페이드 인 
	case Client::CEvent_Cliffside::CLIFFSIDE_SEQUENCE_FADE_IN:
	{
		//진입시
		if (true == m_isEnter)
		{
			//진입 표시
			m_isEnter = false;
		}

		//타이머 체크
		if (true == pGameInstance->Check_Timer(TEXT("Cliffside_CutScene_Fade_Out")))
		{
			m_isEnter = true;

			m_eCliffside_Sequence = CLIFFSIDE_SEQUENCE_END;

			//페이드 인
			m_pRenderer->FadeIn(1.0f);

			for (auto& Pair : m_pMonsters)
				Pair.second->Spawn();

			m_pFig->Spawn();
		}
	}
		break;
	case Client::CEvent_Cliffside::CLIFFSIDE_SEQUENCE_END:
	{
		if (true == m_isEnter)
			break;

		if (true == m_pCutScene_0_0->Is_Collision())
		{
			if (m_pCutScene_0_0->isDead())
			{
				m_isEnter = true;
				m_eCliffside_Sequence = CLIFFSIDE_SEQUENCE_FADE_OUT;
				//타이머 리셋
				pGameInstance->Reset_Timer(TEXT("Cliffside_CutScene_Fade_Out"));
			}
		}
	}
	break;
	default:
		break;
	}





	ENDINSTANCE;
}

HRESULT CEvent_Cliffside::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
	}

	CTrigger::TRIGGERDESC TriggerDesc;

	/* For.Trigger_CutScene_0_0 */
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_CutSceneTest");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(5.f, 5.f, 5.f);
	TriggerDesc.vTriggerWorldPos = _float3(30.0f, 7.0f, 38.0f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_CutScene_0_0"), reinterpret_cast<CComponent**>(&m_pCutScene_0_0), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Cliffside Failed Add_Components : Trigger_CutScene_0_0");
		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Read_CutSceneCamera(TEXT("CutScene_0_0"), TEXT("../../Resources/GameData/CutScene/Map0_0.cut"));

	ENDINSTANCE;


	return S_OK;
}

CEvent_Cliffside* CEvent_Cliffside::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Cliffside* pInstance = New CEvent_Cliffside(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Cliffside");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Cliffside* CEvent_Cliffside::Clone(void* pArg)
{
	CEvent_Cliffside* pInstance = New CEvent_Cliffside(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Cliffside");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Cliffside::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCutScene_0_0);
		Safe_Release(m_pRenderer);

		for (auto& Pair : m_pMonsters)
			Safe_Release(Pair.second);

		Safe_Release(m_pFig);
	}
}