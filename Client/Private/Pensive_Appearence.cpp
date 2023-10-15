#include "Pensive_Appearence.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Appearence::CPensive_Appearence(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Appearence::CPensive_Appearence(const CPensive_Appearence& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Appearence::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Appearence::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Appearence::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Appearence::OnStateEnter(void* _pArg)
{
	//첫 실행시 등장 애니메이션을 재생합니다.
	Change_Animation(TEXT("Spawn"));
}

void CPensive_Appearence::OnStateTick()
{
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Appearence::OnStateExit()
{
}

CPensive_Appearence* CPensive_Appearence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Appearence* pInstance = New CPensive_Appearence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Appearence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Appearence::Clone(void* pArg)
{
	CPensive_Appearence* pInstance = New CPensive_Appearence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Appearence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Appearence::Free()
{
	CStateMachine_Enemy::Free();
}
