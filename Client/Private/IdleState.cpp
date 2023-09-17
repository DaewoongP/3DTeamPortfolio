#include "..\Public\IdleState.h"
#include "GameInstance.h"
#include "Client_Defines.h"

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

}

void CIdleState::OnStateTick()
{
	Go_Turn();

	Go_Start();

}

void CIdleState::OnStateExit()
{

}

void CIdleState::Go_Turn()
{
	_float fAngle = *m_pOwnerLookAngle;

	if (true == *m_pIsDirectionKeyPressed)
	{
		//양수 오른쪽
		if (m_f135Angle < (*m_pOwnerLookAngle))
		{
			//180도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
		}
		else if (m_f45Angle < (*m_pOwnerLookAngle))
		{
			//90도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_45_anm"));
		}

		//음수 왼쪽
		if (-m_f135Angle > (*m_pOwnerLookAngle))
		{
			//180도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
		}
		else if (-m_f45Angle > (*m_pOwnerLookAngle))
		{
			//90도를 실행
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_45_anm"));
		}

		Set_StateMachine(TEXT("Move Turn"));
	}
}

void CIdleState::Go_Start()
{
	_float fAngle = *m_pOwnerLookAngle;
	
	//각이 작을 경우 바로뛴다.
	if (true == *m_pIsDirectionKeyPressed && 
		m_f45Angle > fAngle && 
		-m_f45Angle < fAngle)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}
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
