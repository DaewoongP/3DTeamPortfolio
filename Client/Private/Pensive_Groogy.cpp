#include "Pensive_Groogy.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Groogy::CPensive_Groogy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Groogy::CPensive_Groogy(const CPensive_Groogy& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Groogy::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Groogy::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Groogy::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Groogy::OnStateEnter(void* _pArg)
{
	//첫 실행시 등장 애니메이션을 재생합니다.
	Change_Animation(TEXT("Spawn"));
	
}

void CPensive_Groogy::OnStateTick()
{
}

void CPensive_Groogy::OnStateExit()
{
}

void CPensive_Groogy::Action_None_Tick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		//랜덤으로 다음 상태 갱신
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Groogy::Bind_Notify()
{

}

CPensive_Groogy* CPensive_Groogy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Groogy* pInstance = New CPensive_Groogy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Groogy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Groogy::Clone(void* pArg)
{
	CPensive_Groogy* pInstance = New CPensive_Groogy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Groogy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Groogy::Free()
{
	CStateMachine_Enemy::Free();
}
