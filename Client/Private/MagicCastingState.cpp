#include "MagicCastingState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CMagicCastingState::CMagicCastingState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CMagicCastingState::CMagicCastingState(const CMagicCastingState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CMagicCastingState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagicCastingState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Fix_Angle_Magic_Cast"), false, 0.2f);

	pGameInstance->Add_Timer(TEXT("Fix_Angle_Magic_Cast_Last"), false, 0.4f);

	pGameInstance->Add_Timer(TEXT("Go_Idle_Key_Delay"), false, 0.2f);

	m_fFixAngleSpeed = 5.0f;

	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_01"));
	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_02"));
	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_03"));

	ENDINSTANCE;

	return S_OK;
}

void CMagicCastingState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CMagicCastingState::Late_Tick(_float fTimeDelta)
{
}

void CMagicCastingState::OnStateEnter(void* _pArg)
{
	BEGININSTANCE;

	//임시용
	*m_StateMachineDesc.piActionType = CPlayer::ACTION_CMBT;

	//타입에 맞는 함수를 사용하겠다.
	MAGICCASTINGSTATEDESC* pMagicCastingStateDesc = static_cast<MAGICCASTINGSTATEDESC*>(_pArg);

	m_pisReadySpell = pMagicCastingStateDesc->pisReadySpell;

	if (false != wcscmp(static_cast<CStateContext*>(m_pOwner)->Get_PreStateKey(), TEXT("Magic_Cast")))
	{
		m_iBasicSpellCombo = BASICSPELL_START;
	}

	Initialize_Spell();

	//평타
	if (true == *m_pisReadySpell && pMagicCastingStateDesc->iSpellType == (_uint)SPELL_BASIC)
	{
		

		switch (m_iBasicSpellCombo)
		{
		case Client::CMagicCastingState::BASICSPELL_START:
		{
			switch (*m_StateMachineDesc.piActionType)
			{
			case CPlayer::ACTION_CASUAL:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"));
			}
			break;
			case CPlayer::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"));
				}
				break;
				case BASICSPELL_RANDOM_END:
				{

				}
				break;

				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case Client::CMagicCastingState::BASICSPELL_SECOND:
		{
			switch (*m_StateMachineDesc.piActionType)
			{
			case CPlayer::ACTION_CASUAL:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"));
			}
			break;
			case CPlayer::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"));
				}
				break;
				case BASICSPELL_RANDOM_END:
				{

				}
				break;

				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case Client::CMagicCastingState::BASICSPELL_BACKSTEP:
		{
			switch (*m_StateMachineDesc.piActionType)
			{
			case CPlayer::ACTION_CASUAL:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"));
			}
			break;
			case CPlayer::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"));
				}
				break;
				case BASICSPELL_RANDOM_END:
				{

				}
				break;

				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		case Client::CMagicCastingState::BASICSPELL_LAST:
		{
			switch (*m_StateMachineDesc.piActionType)
			{
			case CPlayer::ACTION_CASUAL:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"));
			}
			break;
			case CPlayer::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"));
				}
				break;
				case BASICSPELL_RANDOM_END:
				{

				}
				break;

				default:
					break;
				}
			}
			break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}

		*m_pisReadySpell = false;
	}
	else
	{
		m_iBasicSpellCombo = BASICSPELL_START;
	}
	// 일반 마법
	if (true == *m_pisReadySpell && pMagicCastingStateDesc->iSpellType == (_uint)SPELL_NORMAL)
	{
		//맞는 액션으로 노티파이 바꾸고 실행 애니메이션 실행
		
		//함수 받아와야 겠다.
		m_StateMachineDesc.pOwnerModel->Bind_Notify(m_vecSpellActionTextList[m_iSpellActionIndex], TEXT("Ready_Spell"), pMagicCastingStateDesc->pFuncSpell);

		//애니메이션 재생
		m_StateMachineDesc.pOwnerModel->Change_Animation(m_vecSpellActionTextList[m_iSpellActionIndex]);

		*m_pisReadySpell = false;

		Spell_Action_Count();
	}

	if (true == *m_pisReadySpell && pMagicCastingStateDesc->iSpellType == (_uint)SPELL_FINISHER)
	{
		//피니셔 애니메이션 재생
		//m_StateMachineDesc.pOwnerModel->Change_Animation();

		*m_pisReadySpell = false;
	}


	ENDINSTANCE;

}

void CMagicCastingState::OnStateTick()
{
	Fix_Angle();

	Go_Idle();

}

void CMagicCastingState::OnStateExit()
{

	* m_pisReadySpell = true;


	*m_StateMachineDesc.pisFinishAnimation = true;
}

void CMagicCastingState::Bind_Notify()
{
	/*void (Client::CMagicCastingState::*Notify_Pointer)();

	Notify_Pointer = &BasicSpell_Ready;*/

	function<void()> Notify_Pointer = [&] {(*this).BasicSpell_Ready(); };

	//캐주얼 평타
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	//전투 평타
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);

	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Spell_Action_01"), TEXT("Spell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Spell_Action_02"), TEXT("Spell_Ready"), Notify_Pointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Spell_Action_03"), TEXT("Spell_Ready"), Notify_Pointer);
}

void CMagicCastingState::BasicSpell_Ready()
{
	//인덱스 증가 
	++m_iBasicSpellCombo;
	//준비 변수 초기화
	*m_pisReadySpell = true;

	BEGININSTANCE;

	pGameInstance->Reset_Timer(TEXT("Go_Idle_Key_Delay"));

	ENDINSTANCE;

	if (BASICSPELL_END <= m_iBasicSpellCombo)
	{
		m_iBasicSpellCombo = BASICSPELL_START;
		m_iBasicSpellRandom = rand() % BASICSPELL_RANDOM_END;
	}
}
void CMagicCastingState::Fix_Angle()
{
	BEGININSTANCE;

	//타이머가 전부 돌지 않았다면
	if (false == pGameInstance->Check_Timer(TEXT("Fix_Angle_Magic_Cast")) && BASICSPELL_LAST != m_iBasicSpellCombo)
	{
		_float fAngle = *m_StateMachineDesc.pfTargetAngle;

		//지속적으로 회전
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * m_fFixAngleSpeed);

		m_isLastBasicSpellBack = false;
	}
	else if (true == m_isLastBasicSpellBack && BASICSPELL_RANDOM_BACK == m_iBasicSpellRandom && BASICSPELL_LAST == m_iBasicSpellCombo)
	{
		_float fAngle = *m_StateMachineDesc.pfTargetAngle;

		//지속적으로 회전
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle/* * pGameInstance->Get_World_Tick() * m_fFixAngleSpeed*/);

		m_isLastBasicSpellBack = true;
	}
	ENDINSTANCE;
}

void CMagicCastingState::Initialize_Spell()
{
	BEGININSTANCE;

	pGameInstance->Reset_Timer(TEXT("Fix_Angle_Magic_Cast"));
	pGameInstance->Reset_Timer(TEXT("Fix_Angle_Magic_Cast_Last"));

	ENDINSTANCE;
}

void CMagicCastingState::Spell_Action_Count()
{
	++m_iSpellActionIndex;

	if (SPELL_ACTION_END == m_iSpellActionIndex)
	{
		m_iSpellActionIndex = SPELL_ACTION_01;
	}
}

void CMagicCastingState::Action_Cmbt_Tick()
{
}

void CMagicCastingState::Go_Idle()
{
	BEGININSTANCE;

	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation()
		||
		(true == *m_pisReadySpell && 
			true == *m_StateMachineDesc.pisDirectionPressed && 
			true == pGameInstance->Check_Timer(TEXT("Go_Idle_Key_Delay"))))
	{
		Set_StateMachine(TEXT("Idle"));
	}

	ENDINSTANCE;
}

CMagicCastingState* CMagicCastingState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagicCastingState* pInstance = New CMagicCastingState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagicCastingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMagicCastingState::Clone(void* pArg)
{
	CMagicCastingState* pInstance = New CMagicCastingState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagicCastingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagicCastingState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{
		m_vecSpellActionTextList.clear();
	}
}
