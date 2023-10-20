#include "Pensive_Hit.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"
#include "Pensive.h"

CPensive_Hit::CPensive_Hit(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Hit::CPensive_Hit(const CPensive_Hit& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Hit::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

void CPensive_Hit::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CPensive_Hit::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Hit::OnStateEnter(void* _pArg)
{
	_float4x4 PosMatrix = *dynamic_cast<CEnemy*>(m_pOwner->Get_Owner())->Get_HitBoneMatrix(0) * dynamic_cast<CEnemy*>(m_pOwner->Get_Owner())->Get_Transform()->Get_WorldMatrix();
	ADD_DECREASE_LIGHT(PosMatrix.Translation(), 150.f, 1.f, _float4(1,1,1,1));
}

void CPensive_Hit::OnStateTick()
{
	Action_None_Tick();
}

void CPensive_Hit::OnStateExit()
{
}

void CPensive_Hit::Action_None_Tick()
{
	//애니메이션 끝났으면?
	if (true == m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		//랜덤으로 다음 상태 갱신
		Set_StateMachine(TEXT("Idle"));
	}
}

void CPensive_Hit::Bind_Notify()
{

}

CPensive_Hit* CPensive_Hit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Hit* pInstance = New CPensive_Hit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Hit::Clone(void* pArg)
{
	CPensive_Hit* pInstance = New CPensive_Hit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Hit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Hit::Free()
{
	CStateMachine_Enemy::Free();
}
