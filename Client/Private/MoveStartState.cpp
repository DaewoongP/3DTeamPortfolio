#include "MoveStartState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

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

void CMoveStartState::OnStateEnter()
{
#ifdef _DEBUG
	//cout << "Start Enter" << endl;
#endif // _DEBUG
}

void CMoveStartState::OnStateTick()
{
	LookFront();

	Over_135();
	
	Go_Idle();

	Go_Loop();

}

void CMoveStartState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Start Exit" << endl;
#endif // _DEBUG
}

void CMoveStartState::LookFront()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;
	
	if (true == *m_pIsDirectionKeyPressed)
	{
		//���������� ȸ��
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CMoveStartState::Go_Idle()
{
	//����Ű�� ������ �ʾ��� ���
	if (true != *m_pIsDirectionKeyPressed)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveStartState::Go_Loop()
{
	//Ű�� �����ְ� �ִϸ��̼��� �����ٸ�
	if (true == *m_pIsDirectionKeyPressed && m_pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Move Loop"));
	}
}

void CMoveStartState::Over_135()
{
	//135���� ũ�ٸ� ������
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Rht_180_anm"));
	}
	//135���� �۴ٸ� ����
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Turn_Start_Lft_180_anm"));
	}
}

void CMoveStartState::None_Angle()
{
	//���¿� ���� ������, ����, ���� ���� ���� ���ؾ� �Ҽ��� �ִ�.
	//�ϴ� �ٴ°͸� ó���Ѵ�.
	BEGININSTANCE;

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON))
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle2Cmbt_RF_Wand_Equip_anm"));
	}

	if (true == *m_pIsDirectionKeyPressed)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));
	}

	ENDINSTANCE;
}

void CMoveStartState::Right_Angle()
{
	//������ ���� ó���� �Ѵ�.

	//135���� ���ٸ�
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_180_anm"));
	}
	//�̿ܶ��
	else if (m_f45Angle < (*m_pOwnerLookAngle))
	{
		//90���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_90_anm"));
	}
}

void CMoveStartState::Left_Angle()
{
	//������ ���� ó���� �Ѵ�.

	//135���� ���ٸ�
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_180_anm"));
	}
	//�̿ܶ��
	else if (-m_f45Angle > (*m_pOwnerLookAngle))
	{
		//90���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_90_anm"));
	}
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



