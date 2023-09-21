#include "ProtegoState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CProtegoState::CProtegoState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CProtegoState::CProtegoState(const CProtegoState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CProtegoState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProtegoState::Initialize(void* pArg)
{
	return S_OK;
}

void CProtegoState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CProtegoState::Late_Tick(_float fTimeDelta)
{
}

void CProtegoState::OnStateEnter()
{
#ifdef _DEBUG
	cout << "Protego Enter" << endl;
#endif // _DEBUG

	//시작 애니메이션
	m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"));
}

void CProtegoState::OnStateTick()
{
	if (true == *m_pIsFinishAnimation &&
		!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Start_anm")))
	{
		*m_pIsFinishAnimation = false;

		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Loop_anm"));
	}
	
	Go_Idle();
}

void CProtegoState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Protego Exit" << endl;
#endif // _DEBUG
}

void CProtegoState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Start_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Loop_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CProtegoState::Go_Idle()
{
	if (true == *m_pIsFinishAnimation &&
		!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Loop_anm")))
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CProtegoState::Stupefy()
{
	BEGININSTANCE;
	
	//맞았을때
	if (true &&
		pGameInstance->Get_DIKeyState(DIK_Q,CInput_Device::KEY_PRESSING))
	{
		//스투페파이 발사~
	}

	ENDINSTANCE;
}

CProtegoState* CProtegoState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtegoState* pInstance = New CProtegoState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProtegoState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CProtegoState::Clone(void* pArg)
{
	CProtegoState* pInstance = New CProtegoState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProtegoState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProtegoState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
