#include "RollState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CRollState::CRollState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CRollState::CRollState(const CRollState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CRollState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CRollState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CRollState::Late_Tick(_float fTimeDelta)
{
}

void CRollState::OnStateEnter(void* _pArg)
{
	//전투 상태가 되어야 한다.
	*m_StateMachineDesc.piActionType = CPlayer::ACTION_CMBT;

	m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"));

	//m_isEnterTick = true;

	*m_StateMachineDesc.pisFinishAnimation = false;

	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f,1.0f,0.0f), *m_StateMachineDesc.pOwnerLookAngle);
	}


#ifdef _DEBUG
	cout << "Roll Enter" << endl;
#endif // _DEBUG
}

void CRollState::OnStateTick()
{
	////방향키가 눌려있다면 방향키 방향으로 회전 시켜준다.
	//if (true == m_isEnterTick && true == *m_StateMachineDesc.pisDirectionPressed)
	//{
	//	m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), *m_StateMachineDesc.pOwnerLookAngle);
	//	m_isEnterTick = false;
	//}
	
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
		Set_StateMachine(TEXT("Idle"));
		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CRollState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Roll Exit" << endl;
#endif // _DEBUG
}

void CRollState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CRollState::Go_Idle()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
		Set_StateMachine(TEXT("Idle"));
	}
}

CRollState* CRollState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRollState* pInstance = New CRollState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRollState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CRollState::Clone(void* pArg)
{
	CRollState* pInstance = New CRollState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRollState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRollState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
