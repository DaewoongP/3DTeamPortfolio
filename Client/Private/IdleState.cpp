#include "..\Public\IdleState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CIdleState::CIdleState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CIdleState::CIdleState(const CIdleState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CIdleState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIdleState::Initialize(void* pArg)
{
	BEGININSTANCE;

	m_fAction_Change_Duration = 10.0f;

	pGameInstance->Add_Timer(TEXT("Action_Change"), true, m_fAction_Change_Duration);

	ENDINSTANCE;

	return S_OK;
}

void CIdleState::Tick(_float fTimeDelta)
{
	OnStateTick();
	
}

void CIdleState::Late_Tick(_float fTimeDelta)
{
}

void CIdleState::OnStateEnter(void* _pArg)
{
#ifdef _DEBUG
	//cout << "Idle Enter" << endl;
#endif // _DEBUG

	BEGININSTANCE;

	pGameInstance->Reset_Timer(TEXT("Action_Change"));

	ENDINSTANCE;

}

void CIdleState::OnStateTick()
{
	switch (*m_pIActionSwitch)
	{
	case CStateContext::ACTION_NONE:
	{
		Action_None_Tick();
	}
	break;
	case CStateContext::ACTION_CASUAL:
	{
		Action_Casual_Tick();
		ActionType_Change();
	}
	break;
	case CStateContext::ACTION_CMBT:
	{
		Action_Cmbt_Tick();
		ActionType_Change();
	}
	break;
	case CStateContext::ACTION_END:
	{

	}
	break;

	default:
		break;
	}

	Go_Turn();

	Go_Start();

	Go_Roll();

	Go_Jump();

	Go_Magic_Cast();

	Go_Protego();

	Go_Hit();
}

void CIdleState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Idle Exit" << endl;
#endif // _DEBUG
}

void CIdleState::Action_None_Tick()
{
	if (true == *m_pIsFinishAnimation)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));

		*m_pIsFinishAnimation = false;
	}
}

void CIdleState::Action_Casual_Tick()
{
	if (true == *m_pIsFinishAnimation)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_Casual_anm"));

		*m_pIsFinishAnimation = false;
	}
}

void CIdleState::Action_Cmbt_Tick()
{
	if (true == *m_pIsFinishAnimation)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_RF_Idle_anm"));

		*m_pIsFinishAnimation = false;
	}
}

void CIdleState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Idle_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_Casual_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_RF_Idle_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_RF_2Cmbt_Idle_Casual_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_Casual_2BM_Idle_RF_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CIdleState::Go_Turn()
{
	_float fAngle = *m_pOwnerLookAngle;

	if (true == *m_pIsDirectionKeyPressed)
	{
		//양수 오른쪽
		if (m_f135Angle < (*m_pOwnerLookAngle))
		{
			//180도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
		}
		else if (m_f45Angle < (*m_pOwnerLookAngle))
		{
			//90도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_45_anm"));
		}

		//음수 왼쪽
		if (-m_f135Angle > (*m_pOwnerLookAngle))
		{
			//180도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
		}
		else if (-m_f45Angle > (*m_pOwnerLookAngle))
		{
			//90도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_45_anm"));
		}

		Set_StateMachine(TEXT("Move Turn"));
	}
}

void CIdleState::Go_Start()
{
	_float fAngle = *m_pOwnerLookAngle;
	
	//각이 작을 경우 바로뛴다.
	if (true == *m_pIsDirectionKeyPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}
}

void CIdleState::Go_Roll()
{
	BEGININSTANCE;
	
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL,CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
	}
	
	ENDINSTANCE;
}

void CIdleState::Go_Jump()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Jump"));
	}

	ENDINSTANCE;
}

void CIdleState::Go_Magic_Cast()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (CStateContext::ACTION_NONE == *m_pIActionSwitch)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_LF_Idle2Cmbt_RF_Wand_Equip_anm"));
		}
		//일단 전투로 보냄
		//포착 기능 생기면 그때 캐주얼이랑 분기
		*m_pIActionSwitch = CStateContext::ACTION_CMBT;
		Set_StateMachine(TEXT("Magic_Cast"));
	}

	ENDINSTANCE;
}

void CIdleState::ActionType_Change()
{
	BEGININSTANCE;

	if (pGameInstance->Check_Timer(TEXT("Action_Change")))
	{
		switch (*m_pIActionSwitch)
		{
		case CStateContext::ACTION_CMBT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_RF_2Cmbt_Idle_Casual_anm"));
		}
		break;
		case CStateContext::ACTION_CASUAL:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_Casual_2BM_Idle_RF_anm"));
		}
		break;
		}
		--(*m_pIActionSwitch);

	}

	ENDINSTANCE;
}

CIdleState* CIdleState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIdleState* pInstance = New CIdleState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CIdleState::Clone(void* pArg)
{
	CIdleState* pInstance = New CIdleState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIdleState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
