#include "MoveLoopState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"
#include "Player.h"

CMoveLoopState::CMoveLoopState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveLoopState::CMoveLoopState(const CMoveLoopState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveLoopState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveLoopState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Meter per Sconde"), true, 1.0f);

	ENDINSTANCE;

	return S_OK;
}

void CMoveLoopState::Tick(_float fTimeDelta)
{
	OnStateTick();

	BEGININSTANCE;

	if (pGameInstance->Check_Timer(TEXT("Meter per Sconde")))
	{
	}

	ENDINSTANCE;
}

void CMoveLoopState::Late_Tick(_float fTimeDelta)
{
}

void CMoveLoopState::OnStateEnter(void* _pArg)
{
}

void CMoveLoopState::OnStateTick()
{
	LookFront();

	//Over_135();

	Go_Idle();

	Switch_MoveType();

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
		*m_StateMachineDesc.pisFinishAnimation = false;
	}

	if (*m_StateMachineDesc.piMoveType == CPlayer::MOVETYPE_SPRINT)
	{
	wcout << m_StateMachineDesc.pOwnerModel->Get_Animation(CModel::OTHERBODY)->Get_AnimationName() << endl;

	}



	BEGININSTANCE;

	//m_StateMachineDesc.pPlayerTransform->Go_Straight(7.0f * pGameInstance->Get_World_Tick());

	ENDINSTANCE;

}

void CMoveLoopState::OnStateExit()
{
}

void CMoveLoopState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint2Jog_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog2Sprint_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CMoveLoopState::LookFront()
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

void CMoveLoopState::Go_Idle()
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

void CMoveLoopState::Switch_MoveType()
{
	BEGININSTANCE;

	//Sprint키
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_WALK:
		case CPlayer::MOVETYPE_JOGING:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
			Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
			
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_SPRINT;
		}
		break;

		case CPlayer::MOVETYPE_SPRINT:
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint2Jog_RU_anm"));
			Change_Animation(TEXT("Hu_BM_Sprint2Jog_RU_anm"));
			
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_JOGING;
		}
		break;
		}
	}

	ENDINSTANCE;
}

CMoveLoopState* CMoveLoopState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveLoopState* pInstance = New CMoveLoopState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveLoopState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveLoopState::Clone(void* pArg)
{
	CMoveLoopState* pInstance = New CMoveLoopState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveLoopState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveLoopState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
