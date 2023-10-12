#include "Fly_Turn.h"


#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CFly_Turn::CFly_Turn(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CFly_Turn::CFly_Turn(const CFly_Turn& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CFly_Turn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFly_Turn::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	//pGameInstance->Add_Timer(TEXT("Turn to Start Delay"), false, 0.1f);

	ENDINSTANCE;

	return S_OK;
}

void CFly_Turn::Tick(_float fTimeDelta)
{
	OnStateTick();
	
}

void CFly_Turn::Late_Tick(_float fTimeDelta)
{
}

void CFly_Turn::OnStateEnter(void* _pArg)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Reset_Timer(TEXT("Turn to Start Delay"))))
	{
		MSG_BOX("Failed Reset Player");
	}

	ENDINSTANCE;

}

void CFly_Turn::OnStateTick()
{
	LookFront();

	Go_Idle();

	Go_Start();


}

void CFly_Turn::OnStateExit()
{
}


void CFly_Turn::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CFly_Turn::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (false == *m_StateMachineDesc.pisDirectionPressed)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CFly_Turn::Go_Start()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	BEGININSTANCE;

	//키가 눌려있고 애니메이션이 끝났다면
	if (true == *m_StateMachineDesc.pisDirectionPressed/* &&
		m_f45Angle > fAngle &&
		-m_f45Angle < fAngle*/
		&& pGameInstance->Check_Timer(TEXT("Turn to Start Delay")))
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));
		Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));
		Set_StateMachine(TEXT("Move Start"));
	}

	ENDINSTANCE;
}

void CFly_Turn::LookFront()
{
	_float fAngle = *m_StateMachineDesc.pOwnerLookAngle;

	BEGININSTANCE;

	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		//지속적으로 회전
		m_StateMachineDesc.pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

CFly_Turn* CFly_Turn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFly_Turn* pInstance = New CFly_Turn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CFly_Turn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CFly_Turn::Clone(void* pArg)
{
	CFly_Turn* pInstance = New CFly_Turn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CFly_Turn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFly_Turn::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}

