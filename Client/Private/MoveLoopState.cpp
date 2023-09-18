#include "MoveLoopState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"

CMoveLoopState::CMoveLoopState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveLoopState::CMoveLoopState(const CMoveLoopState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveLoopState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveLoopState::Initialize(void* pArg)
{
	return S_OK;
}

void CMoveLoopState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CMoveLoopState::Late_Tick(_float fTimeDelta)
{
}

void CMoveLoopState::OnStateEnter()
{
#ifdef _DEBUG
	cout << "Loop Enter" << endl;
#endif // _DEBUG
}

void CMoveLoopState::OnStateTick()
{
	LookFront();

	//Over_135();

	Go_Idle();

	Go_Roll();

	Go_Jump();

	Switch_Joging_Sprint();

	if (m_pOwnerModel->Is_Finish_Animation())
	{
		switch (*m_pIsSprint)
		{
		case CStateContext::JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog_Loop_Fwd_anm"));
		}
		break;

		case CStateContext::SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"));
		}
		break;

		default:
			break;
		}
	}

	
}

void CMoveLoopState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Loop Exit" << endl;
#endif // _DEBUG
}

void CMoveLoopState::LookFront()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;
	
	if (true == *m_pIsDirectionKeyPressed)
	{
		//지속적으로 회전
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CMoveLoopState::Over_135()
{
	//135보다 크다면 오른쪽
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180도를 실행
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Rht_135_anm"));
	}
	//135보다 작다면 왼쪽
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180도를 실행
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Lft_135_anm"));
	}
}

void CMoveLoopState::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (true != *m_pIsDirectionKeyPressed)
	{
		switch (*m_pIsSprint)
		{
		case CStateContext::JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
		}
		break;

		case CStateContext::SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
		}
		break;

		default:
			break;
		}
		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveLoopState::Go_Roll()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
	}

	ENDINSTANCE;
}

void CMoveLoopState::Go_Jump()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Jump"));
	}

	ENDINSTANCE;
}

void CMoveLoopState::Switch_Joging_Sprint()
{
	BEGININSTANCE;

	//키를 눌렀을때
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		*m_pIsSprint = !(*m_pIsSprint);

		switch (*m_pIsSprint)
		{
		case CStateContext::JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint2Jog_RU_anm"));
		}
		break;

		case CStateContext::SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
		}
		break;

		default:
			break;
		}
	}

	ENDINSTANCE;
}




CMoveLoopState* CMoveLoopState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveLoopState* pInstance = New CMoveLoopState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveLoopState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveLoopState::Clone(void* pArg)
{
	CMoveLoopState* pInstance = New CMoveLoopState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveLoopState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveLoopState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
