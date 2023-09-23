#include "MoveTurnState.h"


#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

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

void CMoveTurnState::OnStateEnter()
{
#ifdef _DEBUG
	//cout << "Turn Enter" << endl;
#endif // _DEBUG

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

	//Over_135();

	Go_Roll();

	Go_Jump();

	Go_Magic_Cast();

	Go_Protego();
}

void CMoveTurnState::OnStateExit()
{
#ifdef _DEBUG
	//cout << "Turn Exit" << endl;
#endif // _DEBUG
}


void CMoveTurnState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CMoveTurnState::Go_Idle()
{
	//방향키가 눌리지 않았을 경우
	if (false == *m_pIsDirectionKeyPressed)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CMoveTurnState::Go_Start()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;

	//키가 눌려있고 애니메이션이 끝났다면
	if (true == *m_pIsDirectionKeyPressed/* &&
		m_f45Angle > fAngle &&
		-m_f45Angle < fAngle*/
		&& pGameInstance->Check_Timer(TEXT("Turn to Start Delay")))
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Jog_Start_Fwd_anm"));

		Set_StateMachine(TEXT("Move Start"));
	}

	ENDINSTANCE;
}

void CMoveTurnState::LookFront()
{
	_float fAngle = *m_pOwnerLookAngle;

	BEGININSTANCE;

	if (true == *m_pIsDirectionKeyPressed)
	{
		//지속적으로 회전
		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fAngle * pGameInstance->Get_World_Tick());
	}

	ENDINSTANCE;
}

void CMoveTurnState::Go_Roll()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Roll"));
	}

	ENDINSTANCE;
}

void CMoveTurnState::Go_Jump()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Jump"));
	}

	ENDINSTANCE;
}

void CMoveTurnState::Over_135()
{
	//135보다 크다면 오른쪽
	if (m_f135Angle < (*m_pOwnerLookAngle))
	{
		//180도를 실행
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Rht_135_anm"));
	}
	//135보다 작다면 왼쪽
	if (-m_f135Angle > (*m_pOwnerLookAngle))
	{
		//180도를 실행
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_Turn_Lft_135_anm"));
	}
}

void CMoveTurnState::Go_Magic_Cast()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
	{
		if (CStateContext::ACTION_NONE == *m_pIActionSwitch)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_LF_Idle2Cmbt_RF_Wand_Equip_anm"));
		}
		//일단 전투로 보냄
		//포착 기능 생기면 그때 캐주얼이랑 분기
		*m_pIActionSwitch = CStateContext::ACTION_CMBT;
		Set_StateMachine(TEXT("Magic_Cast"));
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

