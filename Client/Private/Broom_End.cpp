#include "Broom_End.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_End::CBroom_End(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_End::CBroom_End(const CBroom_End& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_End::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_End::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CBroom_End::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_End::Late_Tick(_float fTimeDelta)
{
}

void CBroom_End::OnStateEnter(void* _pArg)
{
	Change_Animation(TEXT("Hu_Broom_Dismount_2Jog_anm"));
	m_StateMachineDesc.pRigidBody->Set_LinearDamping(0.1f);
	m_StateMachineDesc.pRigidBody->Set_Gravity(true);
}

void CBroom_End::OnStateTick()
{
	//애니메이션이 끝났을 경우
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CBroom_End::OnStateExit()
{
}

CBroom_End* CBroom_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_End* pInstance = New CBroom_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_End::Clone(void* pArg)
{
	CBroom_End* pInstance = New CBroom_End(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_End::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
