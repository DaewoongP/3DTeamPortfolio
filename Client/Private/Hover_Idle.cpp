#include "..\Public\Hover_Idle.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CHover_Idle::CHover_Idle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CHover_Idle::CHover_Idle(const CHover_Idle& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CHover_Idle::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}

	//루프상태에서 공격/비공격 상태 전환용도임.
	m_fAction_Change_Duration = 10.0f;
	//ADD_TIMER_SAFELY("Action_Change", true, m_fAction_Change_Duration);
	return S_OK;
}

void CHover_Idle::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CHover_Idle::Late_Tick(_float fTimeDelta)
{
}

void CHover_Idle::OnStateEnter(void* _pArg)
{
	
	switch (*m_StateMachineDesc.piActionType)
	{
	case CPlayer::ACTION_NONE:
	{
		//만약 이전 상태가 아무것도 없었으면?
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		break;
	}
	case CPlayer::ACTION_CMBT:
	{
		//만약 이전 상태가 전투였으면?
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		//한쪽 팔 공격 모션 가져와서 넣어주자.
		//Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"),2);
		break;
	}
	}
	RESET_TIMER_SAFELY("Action_Change");
}

void CHover_Idle::OnStateTick()
{
	switch (*m_StateMachineDesc.piActionType)
	{
	case CPlayer::ACTION_NONE:
	{
		//아이들 애니메이션 재생시켜주는 친구
		Action_None_Tick();
		break;
	}
	//팔만 들어주자.
	case CPlayer::ACTION_CMBT:
	{
		//컴뱃에서 아이들로 상태만 바까주는 친구
		ActionType_Change();
		break;
	}
	default:
		break;
	}
	Go_Start();
}

void CHover_Idle::OnStateExit()
{
}

void CHover_Idle::Action_None_Tick()
{
	//이전에 실행되던 애니메이션이 끝나면 아이들로 바꿔줘
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CHover_Idle::Go_Start()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;
	
	//호버 이동 상태 시작해주세요.
	if (true == *m_StateMachineDesc.pisDirectionPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		Set_StateMachine(TEXT("Hover_Move"));
	}
}


void CHover_Idle::ActionType_Change()
{
	//처음 설정했던 10초 틱이 불렸는지 확인.
	_bool result = {};
	CHECK_TIMER_SAFELY("Action_Change", result);

	//시간이 지났습니다.
	if (result)
	{
		switch (*m_StateMachineDesc.piActionType)
		{
		case CPlayer::ACTION_CMBT:
		{
			//아이들 상태로 돌아가라.
			(*m_StateMachineDesc.piActionType) = CPlayer::ACTION_NONE;
			(*m_StateMachineDesc.pisFinishAnimation) = true;
			break;
		}
		}
	}
}

CHover_Idle* CHover_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHover_Idle* pInstance = New CHover_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHover_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CHover_Idle::Clone(void* pArg)
{
	CHover_Idle* pInstance = New CHover_Idle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHover_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHover_Idle::Free()
{
	CStateMachine::Free();
}
