#include "MagicCastingState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

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

	_int i = wcscmp(static_cast<CStateContext*>(m_pOwner)->Get_PreStateKey(), TEXT("Magic_Cast"));

	if (false != wcscmp(static_cast<CStateContext*>(m_pOwner)->Get_PreStateKey(), TEXT("Magic_Cast")))
	{
		m_isReadySpell = true;
	}


	pGameInstance->Reset_Timer(TEXT("Fix_Angle_Magic_Cast"));
	pGameInstance->Reset_Timer(TEXT("Fix_Angle_Magic_Cast_Last"));

	//평타
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		switch (*m_pIActionSwitch)
		{
		case CStateContext::ACTION_CASUAL:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"));
		}
		break;
		case CStateContext::ACTION_CMBT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"));
		}
		break;
		default:
			break;
		}

		++m_iBasicSpellCombo;

		m_isReadySpell = false;

		m_iBasicSpellCombo = BASICSPELL_START;
	}

	//스킬(임시... 코드 바꿔야 할 확률 100%)
	if (nullptr != _pArg && true == m_isReadySpell &&
		(
		pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIKeyState(DIK_3, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN) ||
		pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN)
		))
	{
		MAGICCASTINGSTATEDESC* pMagicCastingStateDesc = static_cast<MAGICCASTINGSTATEDESC*>(_pArg);

		//맞는 액션으로 노티파이 바꾸고 실행 애니메이션 실행
		
		//함수 받아와야 겠다.
		m_pOwnerModel->Bind_Notify(m_vecSpellActionTextList[m_iSpellActionIndex], TEXT("Ready_Spell"), pMagicCastingStateDesc->pFuncSpell);

		//애니메이션 재생
		m_pOwnerModel->Change_Animation(m_vecSpellActionTextList[m_iSpellActionIndex]);

		m_isReadySpell = false;

		Spell_Action_Count();
	}



	ENDINSTANCE;

#ifdef _DEBUG
	//cout << "Hard Land Enter" << endl;
#endif // _DEBUG
}

void CMagicCastingState::OnStateTick()
{
	Fix_Angle();

	BasicSpell_Tick();

	Go_Idle();


	Go_Protego();

	Go_Roll();
}

void CMagicCastingState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Hard Land Exit" << endl;
#endif // _DEBUG

	

	*m_pIsFinishAnimation = true;
}

void CMagicCastingState::Bind_Notify()
{
	/*void (Client::CMagicCastingState::*Notify_Pointer)();

	Notify_Pointer = &BasicSpell_Ready;*/

	function<void()> Notify_Pointer = [&] {(*this).BasicSpell_Ready(); };

	//캐주얼 평타
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	//전투 평타
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);

	m_pOwnerModel->Bind_Notify(TEXT("Spell_Action_01"), TEXT("Spell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Spell_Action_02"), TEXT("Spell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Spell_Action_03"), TEXT("Spell_Ready"), Notify_Pointer);
}

void CMagicCastingState::Initialize_BasicSpell_Combo()
{
	if (BASICSPELL_END <= m_iBasicSpellCombo)
	{
		m_iBasicSpellCombo = BASICSPELL_START;
	}
}

void CMagicCastingState::BasicSpell_Ready()
{
	//인덱스 증가 
	++m_iBasicSpellCombo;
	//준비 변수 초기화
	m_isReadySpell = true;

	BEGININSTANCE;

	pGameInstance->Reset_Timer(TEXT("Go_Idle_Key_Delay"));

	ENDINSTANCE;

	if (BASICSPELL_END <= m_iBasicSpellCombo)
	{
		m_iBasicSpellCombo = BASICSPELL_START;
		m_iBasicSpellRandom = rand() % BASICSPELL_RANDOM_END;
	}
}

void CMagicCastingState::BasicSpell_Tick()
{
	BEGININSTANCE;
	switch (m_iBasicSpellCombo)
	{
	case Client::CMagicCastingState::BASICSPELL_START:
	{
		if (true == m_isReadySpell && 
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			Initialize_Spell();

			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"));
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
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_SECOND:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			Initialize_Spell();

			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"));
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
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_BACKSTEP:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			Initialize_Spell();

			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"));
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
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_LAST:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			Initialize_Spell();

			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				switch (m_iBasicSpellRandom)
				{
				case BASICSPELL_RANDOM_FRONT:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"));
				}
				break;
				case BASICSPELL_RANDOM_BACK:
				{
					m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"));
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
			m_isReadySpell = false;
		}
	}
		break;
	default:
		break;
	}
	ENDINSTANCE;
}

void CMagicCastingState::Action_Casual_Tick()
{
}

void CMagicCastingState::Fix_Angle()
{
	BEGININSTANCE;

	//타이머가 전부 돌지 않았다면
	if (false == pGameInstance->Check_Timer(TEXT("Fix_Angle_Magic_Cast")) && BASICSPELL_LAST != m_iBasicSpellCombo)
	{
		_float fAngle = *m_pFTargetAngle;

		//지속적으로 회전
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * m_fFixAngleSpeed);

		m_isLastBasicSpellBack = false;
	}
	else if (true == m_isLastBasicSpellBack && BASICSPELL_RANDOM_BACK == m_iBasicSpellRandom && BASICSPELL_LAST == m_iBasicSpellCombo)
	{
		_float fAngle = *m_pFTargetAngle;

		//지속적으로 회전
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle/* * pGameInstance->Get_World_Tick() * m_fFixAngleSpeed*/);

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

	if (m_pOwnerModel->Is_Finish_Animation()
		||
		(true == m_isReadySpell && 
			true == *m_pIsDirectionKeyPressed && 
			true == pGameInstance->Check_Timer(TEXT("Go_Idle_Key_Delay"))))
	{
		Set_StateMachine(TEXT("Idle"));
	}

	ENDINSTANCE;
}

void CMagicCastingState::Go_Roll()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL,CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
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
