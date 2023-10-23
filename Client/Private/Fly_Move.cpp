#include "Fly_Move.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"
#include "Player.h"
#include "ParticleSystem.h"

CFly_Move::CFly_Move(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CFly_Move::CFly_Move(const CFly_Move& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CFly_Move::Initialize_Prototype()
{
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(0, TEXT("Particle_Broom_Wind_Screen")))
	{
		if (FAILED(pGameInstance->Add_Prototype(0, TEXT("Particle_Broom_Wind_Screen")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Broom/Wind_Screen/"), 0))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CFly_Move::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Broom_Wind_Screen"),
		TEXT("Com_Broom_Wind_Screen"), reinterpret_cast<CComponent**>(&m_pWindParticle))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Broom_Wind_Screen)");
		__debugbreak();
		return E_FAIL;
	}

	m_pWindParticle->Disable();
	return S_OK;
}

void CFly_Move::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CFly_Move::Late_Tick(_float fTimeDelta)
{
}

void CFly_Move::OnStateEnter(void* _pArg)
{
	m_pWindParticle->Play(static_cast<CPlayer*>(m_pOwner->Get_Owner())->Get_Transform()->Get_Position() + 
		static_cast<CPlayer*>(m_pOwner->Get_Owner())->Get_Transform()->Get_Look()*5);
}

void CFly_Move::OnStateTick()
{
	LookFront();
	Go_Idle();
	Switch_MoveType();

	m_pWindParticle->Get_Transform()->Set_Position(static_cast<CPlayer*>(m_pOwner->Get_Owner())->Get_Transform()->Get_Position() +
		static_cast<CPlayer*>(m_pOwner->Get_Owner())->Get_Transform()->Get_Look() * 5);
	
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
	if (!*m_StateMachineDesc.pIsFlying)
	{
		Set_StateMachine(TEXT("Broom_End"));
	}
}

void CFly_Move::OnStateExit()
{
	m_pWindParticle->Disable();
}

void CFly_Move::LookFront()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;
	_float fAngle_y = *m_StateMachineDesc.pOwnerLookAngle_Y;
	BEGININSTANCE;
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		//키가 눌렸으면 전진
		_float3 vDir = XMVector3Normalize(m_StateMachineDesc.pPlayerTransform->Get_Look());
		m_StateMachineDesc.pRigidBody->Add_Force(vDir * 60.f, PxForceMode::eFORCE, true);

		//턴 해주는듯한 애니메이션 재생
		_float speed = fAngle * pGameInstance->Get_World_Tick() * (*m_StateMachineDesc.pfRotaionSpeed);
		_float speed_y = fAngle_y * pGameInstance->Get_World_Tick() * (*m_StateMachineDesc.pfRotaionSpeed);

		//m_StateMachineDesc.pPlayerTransform->Turn(_float3(1.0f, 0.0f, 0.0f), speed_y);
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), speed);
		
		if (fabsf(speed) > fabsf(speed_y))
		{
			if (speed < 0)
			{
				if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Lft_anm")))
				{
					lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Lft_anm"));
					Change_Animation(TEXT("Hu_Broom_FlyNoStirrups_Fast_Lft_anm"));
				}
			}
			else if (speed > 0)
			{
				if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Rht_anm")))
				{
					lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Rht_anm"));
					Change_Animation(TEXT("Hu_Broom_FlyNoStirrups_Fast_Rht_anm"));
				}
			}
		}
		else
		{
			if (speed_y < 0)
			{
				if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Dn_anm")))
				{
					lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Dn_anm"));
					Change_Animation(TEXT("Hu_Broom_FlyNoStirrups_Fast_Dn_anm"));
				}
				
			}
			else if (speed_y > 0)
			{
				if (lstrcmp(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Up_anm")))
				{
					lstrcpy(m_wszPostAnimationTag, TEXT("Hu_Broom_FlyNoStirrups_Fast_Up_anm"));
					Change_Animation(TEXT("Hu_Broom_FlyNoStirrups_Fast_Up_anm"));
				}
			}
			m_StateMachineDesc.pRigidBody->Add_Force(_float3(0, 1, 0) * speed_y * 400, PxForceMode::eFORCE, true);
		}
	}
	ENDINSTANCE;
}

void CFly_Move::Go_Idle()
{
	//현재 이동속도가 n 이하라면? hover_move로 변경
	if (true != *m_StateMachineDesc.pisDirectionPressed&&
		m_StateMachineDesc.pRigidBody->Get_Current_Velocity().Length() < 8)
	{
		Set_StateMachine(TEXT("Hover_Move"));
	}
}

void CFly_Move::Switch_MoveType()
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
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_SPRINT;
		}
		break;

		case CPlayer::MOVETYPE_SPRINT:
		{
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_JOGING;
		}
		break;
		}
	}

	ENDINSTANCE;
}

CFly_Move* CFly_Move::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFly_Move* pInstance = New CFly_Move(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFly_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CFly_Move::Clone(void* pArg)
{
	CFly_Move* pInstance = New CFly_Move(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFly_Move");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFly_Move::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pWindParticle);
	}
}
