#include "MoveStartState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"
#include "Player.h"

CMoveStartState::CMoveStartState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveStartState::CMoveStartState(const CMoveStartState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveStartState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveStartState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CMoveStartState::Tick(_float fTimeDelta)
{
	OnStateTick();

	
}

void CMoveStartState::Late_Tick(_float fTimeDelta)
{
}

void CMoveStartState::OnStateEnter(void* _pArg)
{
	
	//무조건 jog 부터
	*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_JOGING;


}

void CMoveStartState::OnStateTick()
{
	LookFront();

	Go_Idle();

	Go_Loop();

	Switch_Sprint();

}

void CMoveStartState::OnStateExit()
{
}


void CMoveStartState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog2Sprint_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CMoveStartState::LookFront()
{
	
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	BEGININSTANCE;
	
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		//지속적으로 회전
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * (*m_StateMachineDesc.pfRotaionSpeed));
	}

	ENDINSTANCE;
}

void CMoveStartState::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (true != *m_StateMachineDesc.pisDirectionPressed)
	{
		switch (*m_StateMachineDesc.piActionType)
		{
		case CPlayer::ACTION_NONE:
		{
			switch (*m_StateMachineDesc.piMoveType)
			{
			case CPlayer::MOVETYPE_JOGING:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CPlayer::MOVETYPE_SPRINT:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
			}
			break;
			}
		}
		break;
		case CPlayer::ACTION_CASUAL:
		{
			switch (*m_StateMachineDesc.piMoveType)
			{
			case CPlayer::MOVETYPE_JOGING:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CPlayer::MOVETYPE_SPRINT:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
			}
			break;
			}
		}
		break;
		case CPlayer::ACTION_CMBT:
		{
			switch (*m_StateMachineDesc.piMoveType)
			{
			case CPlayer::MOVETYPE_JOGING:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
			}
			break;
			case CPlayer::MOVETYPE_SPRINT:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_Cmbt_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_Cmbt_anm"));
			}
			break;
			}
		}
		break;

		default:
			break;
		}

		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveStartState::Go_Loop()
{
	//키가 눌려있고 애니메이션이 끝났다면
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_JOGING:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog_Loop_Fwd_anm"));
			Change_Animation(TEXT("Hu_BM_Jog_Loop_Fwd_anm"));
		}
		break;

		case CPlayer::MOVETYPE_SPRINT:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"));
			Change_Animation(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"));
		}
		break;
		}
		Set_StateMachine(TEXT("Move Loop"));
		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CMoveStartState::Switch_Sprint()
{
	BEGININSTANCE;

	//키를 눌렀을때
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		//조깅중이라면
		if (*m_StateMachineDesc.piMoveType == (_uint)CPlayer::MOVETYPE_JOGING)
		{
			*m_StateMachineDesc.piMoveType = (_uint)CPlayer::MOVETYPE_SPRINT;
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
			Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
		}
	}

	ENDINSTANCE;
}

CMoveStartState* CMoveStartState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveStartState* pInstance = New CMoveStartState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveStartState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveStartState::Clone(void* pArg)
{
	CMoveStartState* pInstance = New CMoveStartState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveStartState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveStartState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}



