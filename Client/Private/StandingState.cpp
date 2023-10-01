#include "StandingState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CStandingState::CStandingState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CStandingState::CStandingState(const CStandingState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CStandingState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStandingState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CStandingState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CStandingState::Late_Tick(_float fTimeDelta)
{
}

void CStandingState::OnStateEnter(void* _pArg)
{
#ifdef _DEBUG
	cout << "Standing Enter" << endl;
#endif // _DEBUG

	*m_StateMachineDesc.pisFinishAnimation = false;
}

void CStandingState::OnStateTick()
{
	//애니메이션이 끝났을 경우
	Go_Idle();
}

void CStandingState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Standing Exit" << endl;
#endif // _DEBUG
	* m_StateMachineDesc.pisFinishAnimation = false;
}

void CStandingState::Go_Idle()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		*m_StateMachineDesc.pisFinishAnimation = false;
		Set_StateMachine(TEXT("Idle"));
	}
}

CStandingState* CStandingState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStandingState* pInstance = New CStandingState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStandingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CStandingState::Clone(void* pArg)
{
	CStandingState* pInstance = New CStandingState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStandingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStandingState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
