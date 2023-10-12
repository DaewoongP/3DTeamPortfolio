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

	//�������¿��� ����/����� ���� ��ȯ�뵵��.
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
		//���� ���� ���°� �ƹ��͵� ��������?
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		break;
	}
	case CPlayer::ACTION_CMBT:
	{
		//���� ���� ���°� ����������?
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		//���� �� ���� ��� �����ͼ� �־�����.
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
		//���̵� �ִϸ��̼� ��������ִ� ģ��
		Action_None_Tick();
		break;
	}
	//�ȸ� �������.
	case CPlayer::ACTION_CMBT:
	{
		//�Ĺ�� ���̵�� ���¸� �ٱ��ִ� ģ��
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
	//������ ����Ǵ� �ִϸ��̼��� ������ ���̵�� �ٲ���
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		Change_Animation(TEXT("Hu_Broom_Hover_Idle_anm"));
		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CHover_Idle::Go_Start()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;
	
	//ȣ�� �̵� ���� �������ּ���.
	if (true == *m_StateMachineDesc.pisDirectionPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		Set_StateMachine(TEXT("Hover_Move"));
	}
}


void CHover_Idle::ActionType_Change()
{
	//ó�� �����ߴ� 10�� ƽ�� �ҷȴ��� Ȯ��.
	_bool result = {};
	CHECK_TIMER_SAFELY("Action_Change", result);

	//�ð��� �������ϴ�.
	if (result)
	{
		switch (*m_StateMachineDesc.piActionType)
		{
		case CPlayer::ACTION_CMBT:
		{
			//���̵� ���·� ���ư���.
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
