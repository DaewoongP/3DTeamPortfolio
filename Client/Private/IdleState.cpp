#include "..\Public\IdleState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CIdleState::CIdleState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CIdleState::CIdleState(const CIdleState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CIdleState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIdleState::Initialize(void* pArg)
{
	return S_OK;
}

void CIdleState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CIdleState::Late_Tick(_float fTimeDelta)
{
}

void CIdleState::OnStateEnter()
{
#ifdef _DEBUG
	cout << "Idle Enter" << endl;
#endif // _DEBUG
}

void CIdleState::OnStateTick()
{
	Go_Turn();

	Go_Start();

	Go_Roll();

	Go_Jump();

	switch (*m_pIActionSwitch)
	{
	case CStateContext::ACTION_NONE:
	{


	}
	break;
	case CStateContext::ACTION_CASUAL:
	{


	}
	break;
	case CStateContext::ACTION_CMBT:
	{


	}
	break;
	case CStateContext::ACTION_END:
	{


	}
	break;

	default:
		break;
	}

}

void CIdleState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Idle Exit" << endl;
#endif // _DEBUG
}

void CIdleState::Action_None_Tick()
{
	if (m_pOwnerModel->Is_Finish_Animation())
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
	}
}

void CIdleState::Action_Casual_Tick()
{
}

void CIdleState::Action_Cmbt_Tick()
{
}

void CIdleState::Go_Turn()
{
	_float fAngle = *m_pOwnerLookAngle;

	if (true == *m_pIsDirectionKeyPressed)
	{
		//��� ������
		if (m_f135Angle < (*m_pOwnerLookAngle))
		{
			//180���� ����
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
		}
		else if (m_f45Angle < (*m_pOwnerLookAngle))
		{
			//90���� ����
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_45_anm"));
		}

		//���� ����
		if (-m_f135Angle > (*m_pOwnerLookAngle))
		{
			//180���� ����
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
		}
		else if (-m_f45Angle > (*m_pOwnerLookAngle))
		{
			//90���� ����
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_45_anm"));
		}

		Set_StateMachine(TEXT("Move Turn"));
	}
}

void CIdleState::Go_Start()
{
	_float fAngle = *m_pOwnerLookAngle;
	
	//���� ���� ��� �ٷζڴ�.
	if (true == *m_pIsDirectionKeyPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}
}

void CIdleState::Go_Roll()
{
	BEGININSTANCE;
	
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL,CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
	}
	
	ENDINSTANCE;
}

void CIdleState::Go_Jump()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Jump"));
	}

	ENDINSTANCE;
}

CIdleState* CIdleState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIdleState* pInstance = New CIdleState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CIdleState::Clone(void* pArg)
{
	CIdleState* pInstance = New CIdleState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIdleState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
