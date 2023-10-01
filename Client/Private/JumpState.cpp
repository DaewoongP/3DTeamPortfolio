#include "JumpState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CJumpState::CJumpState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CJumpState::CJumpState(const CJumpState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CJumpState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJumpState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CJumpState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CJumpState::Late_Tick(_float fTimeDelta)
{
}

void CJumpState::OnStateEnter(void* _pArg)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Jump_RF_anm"));

}

void CJumpState::OnStateTick()
{
	//착지 처리 되면 바꾸기
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Go_Idle();

		Go_Loop();

		Go_HardLand();
	}
	JumpMoveSpeed();
	LookFront();

	
}

void CJumpState::OnStateExit()
{
}

void CJumpState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Land_2Jog_RU_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Land_2Sprint_v2_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Land_Hard_v2_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_Land_Hard_2Jog_v2_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CJumpState::Go_Idle()
{
	//방향키를 땠다면 Idle로
	if (false == *m_StateMachineDesc.pisDirectionPressed)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Soft_v2_anm"));
		Set_StateMachine(TEXT("Idle"));
	}
	
}

void CJumpState::Go_Loop()
{
	//방향키를 누르고 있다면 Loop로
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_JOGING:
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_2Jog_RU_anm"));
			break;
		case CPlayer::MOVETYPE_SPRINT:
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_2Sprint_v2_anm"));
			break;
		}
		Set_StateMachine(TEXT("Move Loop"));
	}
}

void CJumpState::Go_HardLand()
{
	//현제 애니메이션이 fall loop라면 하드랜드
	if (!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Fall_Loop_v2_anm")))
	{
		if (false == *m_StateMachineDesc.pisDirectionPressed)
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Hard_v2_anm"));
		}
		else if (true == *m_StateMachineDesc.pisDirectionPressed)
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Hard_2Jog_v2_anm"));
		}
		Set_StateMachine(TEXT("Hard Land"));
	}
}

void CJumpState::LookFront()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	BEGININSTANCE;

	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		//지속적으로 회전
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * (*m_StateMachineDesc.pfRotaionSpeed));
		//지속적인 힘으로 이동(일단 깡 이동)
		m_StateMachineDesc.pPlayerTransform->Go_Straight(m_fJumpMoveSpeed * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CJumpState::JumpMoveSpeed()
{
	switch (*m_StateMachineDesc.piMoveType)
	{
	case CPlayer::MOVETYPE_NONE:
	{
		m_fJumpMoveSpeed = 0.0f;
	}
	break;
	case CPlayer::MOVETYPE_WALK:
	{
		m_fJumpMoveSpeed = 0.0f;
	}
	break;
	case CPlayer::MOVETYPE_JOGING:
	{
		m_fJumpMoveSpeed = 4.75f;
	}
	break;
	case CPlayer::MOVETYPE_SPRINT:
	{
		m_fJumpMoveSpeed = 7.0f;
	}
	break;
	default:
		break;
	}
}

CJumpState* CJumpState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJumpState* pInstance = New CJumpState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CJumpState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CJumpState::Clone(void* pArg)
{
	CJumpState* pInstance = New CJumpState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CJumpState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
