#include "Pensive_Groogy.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"
#include "Pensive.h"

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
	lstrcpy(m_wszAnimationTag[0], TEXT("Stun_Start"));
	lstrcpy(m_wszAnimationTag[1], TEXT("Stun_Loop"));
	lstrcpy(m_wszAnimationTag[2], TEXT("Attack_Beast"));
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
	m_iMotionIndex = 0;
}

void CPensive_Groogy::OnStateTick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		m_iMotionIndex++;
		Change_Animation(m_wszAnimationTag[m_iMotionIndex]);
		if (m_iMotionIndex > 1)
		{
			cout << "기지개를 실행합니다." << endl;
			*m_StateMachineDesc.pAttackType = CPensive::ATTACK_SCREAM;
			Change_Animation((m_wszAnimationTag[m_iMotionIndex]));
			Set_StateMachine(TEXT("Physical_Attack"));
		}
	}
}

void CPensive_Groogy::OnStateExit()
{
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
