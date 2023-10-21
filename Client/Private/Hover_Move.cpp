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

	return S_OK;
}

void CHover_Move::Tick(_float fTimeDelta)
{
	OnStateTick();
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
	Go_Idle();
	Switch_MoveType();

	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_JOGING:
		{
			Change_Animation(m_wszPostAnimationTag);
		}
		break;

		case CPlayer::MOVETYPE_SPRINT:
		{
			Change_Animation(m_wszPostAnimationTag);
		}
		break;
		}
		*m_StateMachineDesc.pisFinishAnimation = false;
	}
	BEGININSTANCE;
	m_StateMachineDesc.pPlayerTransform->Go_Straight(7.0f * pGameInstance->Get_World_Tick());
	ENDINSTANCE;

}

void CHover_Move::OnStateExit()
{
}

void CHover_Move::Bind_Notify()
{
}

void CHover_Move::LookFront()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	BEGININSTANCE;
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		_float speed = fAngle * pGameInstance->Get_World_Tick() * (*m_StateMachineDesc.pfRotaionSpeed);
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), speed);
		//Hu_Broom_FlyNoStirrups_Fast_Dn_anm
		if (speed<0)
		{
			if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Lft_anm")))
			{
				lstrcpy(m_wszPostAnimationTag,TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Lft_anm"));
				Change_Animation(TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Lft_anm"));
			}
		}
		else if(speed>0)
		{
			if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Rht_anm")))
			{
				lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Rht_anm"));
				Change_Animation(TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Rht_anm"));
			}
		}
		else 
		{
			if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Fwd_anm")))
			{
				lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Fwd_anm"));
				Change_Animation(TEXT("Hu_Broom_Flynostirrups_Hover_Fly_Fwd_anm"));
			}
		}
	}
	ENDINSTANCE;
}

void CHover_Move::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (true != *m_StateMachineDesc.pisDirectionPressed)
	{
		Set_StateMachine(TEXT("Hover_Idle"));
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
			Change_Animation(m_wszPostAnimationTag);
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_SPRINT;
		}
		break;
		case CPlayer::MOVETYPE_SPRINT:
		{
			Change_Animation(m_wszPostAnimationTag);
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
}
