#include "MoveStartState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

#include "StateContext.h"

CMoveStartState::CMoveStartState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveStartState::CMoveStartState(const CMoveStartState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveStartState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveStartState::Initialize(void* pArg)
{
	return S_OK;
}

void CMoveStartState::Tick(_float fTimeDelta)
{
	OnStateTick();

	
}

void CMoveStartState::Late_Tick(_float fTimeDelta)
{
}

void CMoveStartState::OnStateEnter(void* _pArg)
{
	//������ jog ����
	*m_pIMoveSwitch = CStateContext::MOVETYPE_JOGING;


#ifdef _DEBUG
	//cout << "Start Enter" << endl;
#endif // _DEBUG
}

void CMoveStartState::OnStateTick()
{
	LookFront();

	Go_Idle();

	Go_Loop();

	Go_Roll();

	Go_Jump();

	Switch_Sprint();

	Go_Magic_Cast();

	Go_Protego();
}

void CMoveStartState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Start Exit" << endl;
#endif // _DEBUG
}


void CMoveStartState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog_Loop_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Sprint_Loop_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_Jog2Sprint_RU_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CMoveStartState::LookFront()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;
	
	if (true == *m_pIsDirectionKeyPressed)
	{
		//���������� ȸ��
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick() * (*m_pFRotationSpeed));
	}

	ENDINSTANCE;
}

void CMoveStartState::Go_Idle()
{
	//����Ű�� ������ �ʾ��� ���
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

void CMoveStartState::Go_Loop()
{
	//Ű�� �����ְ� �ִϸ��̼��� �����ٸ�
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
		Set_StateMachine(TEXT("Move Loop"));
		*m_pIsFinishAnimation = false;
	}
}

void CMoveStartState::Go_Roll()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
	}

	ENDINSTANCE;
}

void CMoveStartState::Go_Jump()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Jump"));
	}

	ENDINSTANCE;
}

void CMoveStartState::Switch_Sprint()
{
	BEGININSTANCE;

	//Ű�� ��������
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_DOWN))
	{
		//�������̶��
		if (*m_pIMoveSwitch == (_uint)CStateContext::MOVETYPE_JOGING)
		{
			*m_pIMoveSwitch = (_uint)CStateContext::MOVETYPE_SPRINT;
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jog2Sprint_RU_anm"));
		}
	}

	ENDINSTANCE;
}

void CMoveStartState::Over_135()
{
	//135���� ũ�ٸ� ������
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Rht_135_anm"));
	}
	//135���� �۴ٸ� ����
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Lft_135_anm"));
	}
}


void CMoveStartState::Go_Magic_Cast()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (CStateContext::ACTION_NONE == *m_pIActionSwitch)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_LF_Idle2Cmbt_RF_Wand_Equip_anm"));
		}
		//�ϴ� ������ ����
		//���� ��� ����� �׶� ĳ�־��̶� �б�
		*m_pIActionSwitch = CStateContext::ACTION_CMBT;
		Set_StateMachine(TEXT("Magic_Cast"));
	}

	ENDINSTANCE;
}

CMoveStartState* CMoveStartState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveStartState* pInstance = New CMoveStartState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveStartState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveStartState::Clone(void* pArg)
{
	CMoveStartState* pInstance = New CMoveStartState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveStartState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveStartState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}



