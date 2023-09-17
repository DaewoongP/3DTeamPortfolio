#include "MoveTurnState.h"


#include "GameInstance.h"
#include "Client_Defines.h"

CMoveTurnState::CMoveTurnState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveTurnState::CMoveTurnState(const CMoveTurnState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveTurnState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveTurnState::Initialize(void* pArg)
{
	return S_OK;
}

void CMoveTurnState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CMoveTurnState::Late_Tick(_float fTimeDelta)
{
}

void CMoveTurnState::OnStateEnter()
{
#ifdef _DEBUG
	cout << "Turn Enter" << endl;
#endif // _DEBUG
}

void CMoveTurnState::OnStateTick()
{
	LookFront();

	Go_Idle();

	Go_Start();

	Over_135();
}

void CMoveTurnState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Turn Exit" << endl;
#endif // _DEBUG
}


void CMoveTurnState::Go_Idle()
{
	//����Ű�� ������ �ʾ��� ���
	if (true != *m_pIsDirectionKeyPressed)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveTurnState::Go_Start()
{
	_float fAngle = *m_pOwnerLookAngle;

	//Ű�� �����ְ� �ִϸ��̼��� �����ٸ�
	if (true == *m_pIsDirectionKeyPressed &&
		m_f45Angle > fAngle &&
		-m_f45Angle < fAngle)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}
}

void CMoveTurnState::LookFront()
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

void CMoveTurnState::Over_135()
{
	//135���� ũ�ٸ� ������
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
	}
	//135���� �۴ٸ� ����
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180���� ����
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
	}
}

CMoveTurnState* CMoveTurnState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveTurnState* pInstance = New CMoveTurnState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveTurnState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveTurnState::Clone(void* pArg)
{
	CMoveTurnState* pInstance = New CMoveTurnState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveTurnState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveTurnState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}

