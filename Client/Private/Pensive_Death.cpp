#include "Pensive_Death.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"
#include "Quest_Manager.h"
#include "Enemy.h"

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
	//첫 실행시 등장 애니메이션을 재생합니다.
	Change_Animation(TEXT("Death_Start"));

	CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
	Safe_AddRef(pQuest_Manager);
	pQuest_Manager->Clear_Quest(TEXT("Quest_Secret"));
	Safe_Release(pQuest_Manager);
	
}

void CPensive_Death::OnStateTick()
{
	Action_None_Tick();
}

void CPensive_Death::OnStateExit()
{
}

void CPensive_Death::Action_None_Tick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		if (m_iActionIndex == 0)
		{
			Change_Animation(TEXT("Death_End"));
		}
		else if (m_iActionIndex == 1)
		{
			//사망처리
			dynamic_cast<CGameObject*>(m_pOwner->Get_Owner())->Set_Dead();
		}
		m_iActionIndex++;
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
