#include "Pensive_Death.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Death::CPensive_Death(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Death::CPensive_Death(const CPensive_Death& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Death::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Death::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Death::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Death::OnStateEnter(void* _pArg)
{
	//ù ����� ���� �ִϸ��̼��� ����մϴ�.
	Change_Animation(TEXT("Spawn"));
	
}

void CPensive_Death::OnStateTick()
{
}

void CPensive_Death::OnStateExit()
{
}

void CPensive_Death::Action_None_Tick()
{
	//�ִϸ��̼� ��������?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		//�������� ���� ���� ����
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Death::Bind_Notify()
{

}

CPensive_Death* CPensive_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Death* pInstance = New CPensive_Death(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Death");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Death::Clone(void* pArg)
{
	CPensive_Death* pInstance = New CPensive_Death(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Death");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Death::Free()
{
	CStateMachine_Enemy::Free();
}
