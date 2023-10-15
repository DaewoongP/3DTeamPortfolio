#include "Pensive_Physical_Attack.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Physical_Attack::CPensive_Physical_Attack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Physical_Attack::CPensive_Physical_Attack(const CPensive_Physical_Attack& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Physical_Attack::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Physical_Attack::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Physical_Attack::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Physical_Attack::OnStateEnter(void* _pArg)
{
}

void CPensive_Physical_Attack::OnStateTick()
{
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Physical_Attack::OnStateExit()
{
}

void CPensive_Physical_Attack::Bind_Notify()
{

}

CPensive_Physical_Attack* CPensive_Physical_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Physical_Attack* pInstance = New CPensive_Physical_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Physical_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Physical_Attack::Clone(void* pArg)
{
	CPensive_Physical_Attack* pInstance = New CPensive_Physical_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Physical_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Physical_Attack::Free()
{
	CStateMachine_Enemy::Free();
}
