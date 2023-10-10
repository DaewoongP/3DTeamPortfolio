#include "MoveTurnState.h"


#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CMoveTurnState::CMoveTurnState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CMoveTurnState::CMoveTurnState(const CMoveTurnState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CMoveTurnState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveTurnState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Turn to Start Delay"), false, 0.1f);

	ENDINSTANCE;

	return S_OK;
}

void CMoveTurnState::Tick(_float fTimeDelta)
{
	OnStateTick();
	
}

void CMoveTurnState::Late_Tick(_float fTimeDelta)
{
}

void CMoveTurnState::OnStateEnter(void* _pArg)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Reset_Timer(TEXT("Turn to Start Delay"))))
	{
		MSG_BOX("Failed Reset Player");
	}

	ENDINSTANCE;

}

void CMoveTurnState::OnStateTick()
{
	LookFront();

	Go_Idle();

	Go_Start();


}

void CMoveTurnState::OnStateExit()
{
}


void CMoveTurnState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CMoveTurnState::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (false == *m_StateMachineDesc.pisDirectionPressed)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveTurnState::Go_Start()
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

void CMoveTurnState::LookFront()
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

CMoveTurnState* CMoveTurnState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMoveTurnState* pInstance = New CMoveTurnState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMoveTurnState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMoveTurnState::Clone(void* pArg)
{
	CMoveTurnState* pInstance = New CMoveTurnState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMoveTurnState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveTurnState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}

