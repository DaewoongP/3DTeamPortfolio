#include "MoveLoopState.h"

#include "GameInstance.h"
#include "Client_Defines.h"

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

	Over_135();

	Go_Idle();

	if (m_pOwnerModel->Is_Finish_Animation())
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
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
		//���������� ȸ��
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CMoveLoopState::Over_135()
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

void CMoveLoopState::Go_Idle()
{
	//����Ű�� ������ �ʾ��� ���
	if (true != *m_pIsDirectionKeyPressed)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Stop_Fwd_anm"));
		Set_StateMachine(TEXT("Idle"));
	}
}


void CMoveLoopState::None_Angle()
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

void CMoveLoopState::Right_Angle()
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

void CMoveLoopState::Left_Angle()
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
