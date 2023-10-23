#include "Broom_Begin.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_Begin::CBroom_Begin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_Begin::CBroom_Begin(const CBroom_Begin& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_Begin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Begin::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CBroom_Begin::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_Begin::Late_Tick(_float fTimeDelta)
{
}

void CBroom_Begin::OnStateEnter(void* _pArg)
{
	//이동 상태에 따라 다른 탑승모션.
	switch (*m_StateMachineDesc.piMoveType)
	{
	case CPlayer::MOVETYPE_NONE:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Hover_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Hover_anm"));
		break;
	case CPlayer::MOVETYPE_JOGING:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Fly_fWalk_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Fly_fWalk_anm"));
		break;
	case CPlayer::MOVETYPE_SPRINT:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Fly_fJog_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Fly_fJog_anm"));
		break;
	default:
		break;
	}
	m_StateMachineDesc.pRigidBody->Set_LinearDamping(2.f);
	m_StateMachineDesc.pRigidBody->Set_Gravity(false);
}

void CBroom_Begin::OnStateTick()
{
	//탑승애니메이션 끝났으면?
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		//다음 상태로 진행해주세요(hover_idle)
		Go_Hover_Idle();
		Go_Move();
	}
}

void CBroom_Begin::OnStateExit()
{
	//지팡이 탑승 완료니까 flying으로 바까주겟음
	*m_StateMachineDesc.pIsFlying = true;
}

void CBroom_Begin::Go_Hover_Idle()
{
	//방향키를 땠다면 Idle로
	if (false == *m_StateMachineDesc.pisDirectionPressed)
	{
		Set_StateMachine(TEXT("Hover_Idle"));
	}
}

void CBroom_Begin::Go_Move()
{
	//방향키 누르고 있으면, 이동 상태에 따라 호버/플라이로 빠른 진입.
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_JOGING:
			Set_StateMachine(TEXT("Hover_Move"));
			break;
		case CPlayer::MOVETYPE_SPRINT:
			Set_StateMachine(TEXT("Hover_Move"));
			break;
		}
	}
}

CBroom_Begin* CBroom_Begin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Begin* pInstance = New CBroom_Begin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_Begin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_Begin::Clone(void* pArg)
{
	CBroom_Begin* pInstance = New CBroom_Begin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_Begin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_Begin::Free()
{
	CStateMachine::Free();
}
