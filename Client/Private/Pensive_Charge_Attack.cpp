#include "Pensive_Charge_Attack.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Charge_Attack::CPensive_Charge_Attack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Charge_Attack::CPensive_Charge_Attack(const CPensive_Charge_Attack& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Charge_Attack::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Charge_Attack::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Charge_Attack::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Charge_Attack::OnStateEnter(void* _pArg)
{
	//첫 실행시 등장 애니메이션을 재생합니다.
	Change_Animation(TEXT("Spawn"));
	
}

void CPensive_Charge_Attack::OnStateTick()
{
}

void CPensive_Charge_Attack::OnStateExit()
{
}

void CPensive_Charge_Attack::Action_None_Tick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		//랜덤으로 다음 상태 갱신
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Charge_Attack::Bind_Notify()
{

}

CPensive_Charge_Attack* CPensive_Charge_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Charge_Attack* pInstance = New CPensive_Charge_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Charge_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Charge_Attack::Clone(void* pArg)
{
	CPensive_Charge_Attack* pInstance = New CPensive_Charge_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Charge_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Charge_Attack::Free()
{
	CStateMachine_Enemy::Free();
}
