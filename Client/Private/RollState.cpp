#include "RollState.h"
#include "GameInstance.h"
#include "Client_Defines.h"

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
	return S_OK;
}

void CRollState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CRollState::Late_Tick(_float fTimeDelta)
{
}

void CRollState::OnStateEnter()
{

	//전투 상태가 되어야 한다.
	m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"));

#ifdef _DEBUG
	cout << "Roll Enter" << endl;
#endif // _DEBUG
}

void CRollState::OnStateTick()
{
	if (m_pOwnerModel->Is_Finish_Animation())
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
		Set_StateMachine(TEXT("Idle"));
	}
}

void CRollState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Roll Exit" << endl;
#endif // _DEBUG
}

void CRollState::Go_Idle()
{
	Set_StateMachine(TEXT("Idle"));
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
