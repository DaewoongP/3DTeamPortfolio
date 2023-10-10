#include "HardLandState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CHardLandState::CHardLandState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CHardLandState::CHardLandState(const CHardLandState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CHardLandState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHardLandState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CHardLandState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CHardLandState::Late_Tick(_float fTimeDelta)
{
}

void CHardLandState::OnStateEnter(void* _pArg)
{
}

void CHardLandState::OnStateTick()
{
	//애니메이션이 끝났을 경우
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Go_Idle();

		Go_Loop();
	}
}

void CHardLandState::OnStateExit()
{
}

void CHardLandState::Go_Idle()
{
	if (!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Land_Hard_v2_anm")))
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CHardLandState::Go_Loop()
{
	if (!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Land_Hard_2Jog_v2_anm")))
	{
		//Loop
		if (true == *m_StateMachineDesc.pisDirectionPressed)
		{
			*m_StateMachineDesc.piMoveType = CPlayer::MOVETYPE_JOGING;
			Set_StateMachine(TEXT("Move Loop"));
		}
		//idle
		if (false == *m_StateMachineDesc.pisDirectionPressed)
		{
			switch (*m_StateMachineDesc.piActionType)
			{
			case CPlayer::ACTION_NONE:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CPlayer::ACTION_CASUAL:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CPlayer::ACTION_CMBT:
			{
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
				Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
			}
			break;

			default:
				break;
			}

			Set_StateMachine(TEXT("Idle"));
		}
	}
}

CHardLandState* CHardLandState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHardLandState* pInstance = New CHardLandState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHardLandState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CHardLandState::Clone(void* pArg)
{
	CHardLandState* pInstance = New CHardLandState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHardLandState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHardLandState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
