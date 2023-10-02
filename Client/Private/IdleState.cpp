#include "..\Public\IdleState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CIdleState::CIdleState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CIdleState::CIdleState(const CIdleState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CIdleState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}


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
	BEGININSTANCE;

	pGameInstance->Reset_Timer(TEXT("Action_Change"));

	ENDINSTANCE;

}

void CIdleState::OnStateTick()
{
	

	switch (*m_StateMachineDesc.piActionType)
	{
	case CPlayer::ACTION_NONE:
	{
		Action_None_Tick();
	}
	break;
	case CPlayer::ACTION_CASUAL:
	{
		Action_Casual_Tick();
		ActionType_Change();
	}
	break;
	case CPlayer::ACTION_CMBT:
	{
		Action_Cmbt_Tick();
		ActionType_Change();
	}
	break;
	case CPlayer::ACTION_END:
	{

	}
	break;

	default:
		break;
	}

	Go_Turn();

	Go_Start();

}

void CIdleState::OnStateExit()
{
}

void CIdleState::Action_None_Tick()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));

		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CIdleState::Action_Casual_Tick()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_Casual_anm"));

		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CIdleState::Action_Cmbt_Tick()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_RF_Idle_anm"));

		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CIdleState::Bind_Notify()
{
	
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Idle_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_Casual_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_RF_Idle_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_RF_2Cmbt_Idle_Casual_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Idle_Casual_2BM_Idle_RF_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CIdleState::Go_Turn()
{
	
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		//양수 오른쪽
		if (m_f135Angle < (*m_StateMachineDesc.pOwnerLookAngle))
		{
			//180도를 실행
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
		}
		else if (m_f45Angle < (*m_StateMachineDesc.pOwnerLookAngle))
		{
			//90도를 실행
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_45_anm"));
		}

		//음수 왼쪽
		if (-m_f135Angle > (*m_StateMachineDesc.pOwnerLookAngle))
		{
			//180도를 실행
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
		}
		else if (-m_f45Angle > (*m_StateMachineDesc.pOwnerLookAngle))
		{
			//90도를 실행
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_45_anm"));
		}

		Set_StateMachine(TEXT("Move Turn"));
	}
}

void CIdleState::Go_Start()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;
	
	//각이 작을 경우 바로뛴다.
	if (true == *m_StateMachineDesc.pisDirectionPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}
}


void CIdleState::ActionType_Change()
{
	BEGININSTANCE;

	if (pGameInstance->Check_Timer(TEXT("Action_Change")))
	{
		switch (*m_StateMachineDesc.piActionType)
		{
		case CPlayer::ACTION_CMBT:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_RF_2Cmbt_Idle_Casual_anm"));
		}
		break;
		case CPlayer::ACTION_CASUAL:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Idle_Casual_2BM_Idle_RF_anm"));
		}
		break;
		}
		--(*m_StateMachineDesc.piActionType);

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
