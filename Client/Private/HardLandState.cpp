#include "HardLandState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

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
	return S_OK;
}

void CHardLandState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CHardLandState::Late_Tick(_float fTimeDelta)
{
}

void CHardLandState::OnStateEnter()
{
#ifdef _DEBUG
	//cout << "Hard Land Enter" << endl;
#endif // _DEBUG
}

void CHardLandState::OnStateTick()
{
	//애니메이션이 끝났을 경우
	if (m_pOwnerModel->Is_Finish_Animation())
	{
		Go_Idle();

		Go_Loop();
	}
}

void CHardLandState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Hard Land Exit" << endl;
#endif // _DEBUG
}

void CHardLandState::Go_Idle()
{
	if (!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Land_Hard_v2_anm")))
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CHardLandState::Go_Loop()
{
	if (!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Land_Hard_2Jog_v2_anm")))
	{
		//Loop
		if (true == *m_pIsDirectionKeyPressed)
		{
			*m_pIMoveSwitch = CStateContext::MOVETYPE_JOGING;
			Set_StateMachine(TEXT("Move Loop"));
		}
		//idle
		if (false == *m_pIsDirectionKeyPressed)
		{
			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_NONE:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
			}
			break;

			default:
				break;
			}
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
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
