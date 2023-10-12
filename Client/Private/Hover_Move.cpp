#include "Hover_Move.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"
#include "Player.h"

CHover_Move::CHover_Move(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CHover_Move::CHover_Move(const CHover_Move& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CHover_Move::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHover_Move::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	//pGameInstance->Add_Timer(TEXT("Meter per Sconde"), true, 1.0f);

	ENDINSTANCE;

	return S_OK;
}

void CHover_Move::Tick(_float fTimeDelta)
{
	OnStateTick();

	BEGININSTANCE;

	if (pGameInstance->Check_Timer(TEXT("Meter per Sconde")))
	{
	}

	ENDINSTANCE;
}

void CHover_Move::Late_Tick(_float fTimeDelta)
{
}

void CHover_Move::OnStateEnter(void* _pArg)
{
}

void CHover_Move::OnStateTick()
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

void CHover_Move::OnStateExit()
{
}

void CHover_Move::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog_Loop_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint2Jog_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog2Sprint_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CHover_Move::LookFront()
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

void CHover_Move::Go_Idle()
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

void CHover_Move::Switch_MoveType()
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

CHover_Move* CHover_Move::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHover_Move* pInstance = New CHover_Move(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHover_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CHover_Move::Clone(void* pArg)
{
	CHover_Move* pInstance = New CHover_Move(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHover_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHover_Move::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
