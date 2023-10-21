#include "Broom_End.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_End::CBroom_End(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_End::CBroom_End(const CBroom_End& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_End::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_End::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CBroom_End::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_End::Late_Tick(_float fTimeDelta)
{
}

void CBroom_End::OnStateEnter(void* _pArg)
{
}

void CBroom_End::OnStateTick()
{
	//애니메이션이 끝났을 경우
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Go_Idle();
		Go_Loop();
	}
}

void CBroom_End::OnStateExit()
{
}

void CBroom_End::Go_Idle()
{
	if (!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Land_Hard_v2_anm")))
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CBroom_End::Go_Loop()
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

CBroom_End* CBroom_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_End* pInstance = New CBroom_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_End::Clone(void* pArg)
{
	CBroom_End* pInstance = New CBroom_End(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_End::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
