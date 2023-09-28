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
	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Meter per Sconde"), true, 1.0f);

	ENDINSTANCE;

	return S_OK;
}

void CMoveLoopState::Tick(_float fTimeDelta)
{
	OnStateTick();

	BEGININSTANCE;

	if (pGameInstance->Check_Timer(TEXT("Meter per Sconde")))
	{
	}

	ENDINSTANCE;
}

void CMoveLoopState::Late_Tick(_float fTimeDelta)
{
}

void CMoveLoopState::OnStateEnter(void* _pArg)
{
}

void CMoveLoopState::OnStateTick()
{
	LookFront();

	//Over_135();

	Go_Idle();

	Go_Roll();

	//Go_Jump();

	Switch_MoveType();

	if (true == *m_pIsFinishAnimation)
	{
		switch (*m_pIMoveSwitch)
		{
		case CStateContext::MOVETYPE_JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog_Loop_Fwd_anm"));
		}
		break;

		case CStateContext::MOVETYPE_SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"));
		}
		break;
		}
		*m_pIsFinishAnimation = false;
	}

	BEGININSTANCE;

	//m_pPlayerTransform->Go_Straight(7.0f * pGameInstance->Get_World_Tick());

	ENDINSTANCE;

	Go_Magic_Cast();

	Go_Protego();
}

void CMoveLoopState::OnStateExit()
{
}

void CMoveLoopState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog_Loop_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint2Jog_RU_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog2Sprint_RU_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CMoveLoopState::LookFront()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;
	
	if (true == *m_pIsDirectionKeyPressed)
	{
		//지속적으로 회전
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * (*m_pFRotationSpeed));
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
		switch (*m_pIActionSwitch)
		{
		case CStateContext::ACTION_NONE:
		{
			switch (*m_pIMoveSwitch)
			{
			case CStateContext::MOVETYPE_JOGING:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CStateContext::MOVETYPE_SPRINT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
			}
			break;
			}
		}
		break;
		case CStateContext::ACTION_CASUAL:
		{
			switch (*m_pIMoveSwitch)
			{
			case CStateContext::MOVETYPE_JOGING:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
			}
			break;
			case CStateContext::MOVETYPE_SPRINT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_anm"));
			}
			break;
			}
		}
		break;
		case CStateContext::ACTION_CMBT:
		{
			switch (*m_pIMoveSwitch)
			{
			case CStateContext::MOVETYPE_JOGING:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_2Cmbt_anm"));
			}
			break;
			case CStateContext::MOVETYPE_SPRINT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Sprint_Stop_Fwd_Cmbt_anm"));
			}
			break;
			}
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

void CMoveLoopState::Switch_MoveType()
{
	BEGININSTANCE;

	//Sprint키
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		switch (*m_pIMoveSwitch)
		{
		case CStateContext::MOVETYPE_WALK:
		case CStateContext::MOVETYPE_JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
			
			*m_pIMoveSwitch = CStateContext::MOVETYPE_SPRINT;
		}
		break;

		case CStateContext::MOVETYPE_SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Sprint2Jog_RU_anm"));
			
			*m_pIMoveSwitch = CStateContext::MOVETYPE_JOGING;
		}
		break;
		}
	}

	ENDINSTANCE;
}

void CMoveLoopState::Go_Magic_Cast()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (CStateContext::ACTION_NONE == *m_pIActionSwitch)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_LF_Idle2Cmbt_RF_Wand_Equip_anm"));
		}
		//일단 전투로 보냄
		//포착 기능 생기면 그때 캐주얼이랑 분기
		*m_pIActionSwitch = CStateContext::ACTION_CMBT;
		Set_StateMachine(TEXT("Magic_Cast"));
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
