#include "Pensive_Orb_Attack.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Orb_Attack::CPensive_Orb_Attack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Orb_Attack::CPensive_Orb_Attack(const CPensive_Orb_Attack& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Orb_Attack::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	lstrcpy(m_wszAnimationTag[0], TEXT("Attack_Orb_Start"));
	lstrcpy(m_wszAnimationTag[1], TEXT("Attack_Orb_Loop"));
	lstrcpy(m_wszAnimationTag[2], TEXT("Attack_Orb_End"));
	
	return S_OK;
}

void CPensive_Orb_Attack::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Orb_Attack::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Orb_Attack::OnStateEnter(void* _pArg)
{
	m_iMotionIndex = 0;
}

void CPensive_Orb_Attack::OnStateTick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		if (m_iMotionIndex != 2)
		{
			m_iMotionIndex++;
			Change_Animation(m_wszAnimationTag[m_iMotionIndex]);
		}
		else
		{
			Set_StateMachine(TEXT("Idle"));
		}
	}
}

void CPensive_Orb_Attack::OnStateExit()
{
}

void CPensive_Orb_Attack::Bind_Notify()
{

}

CPensive_Orb_Attack* CPensive_Orb_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Orb_Attack* pInstance = New CPensive_Orb_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Orb_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Orb_Attack::Clone(void* pArg)
{
	CPensive_Orb_Attack* pInstance = New CPensive_Orb_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Orb_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Orb_Attack::Free()
{
	CStateMachine_Enemy::Free();
}
