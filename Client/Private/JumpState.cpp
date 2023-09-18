#include "JumpState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CJumpState::CJumpState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CJumpState::CJumpState(const CJumpState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CJumpState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CJumpState::Initialize(void* pArg)
{
	return S_OK;
}

void CJumpState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CJumpState::Late_Tick(_float fTimeDelta)
{
}

void CJumpState::OnStateEnter()
{
	m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Jump_RF_anm"));

#ifdef _DEBUG
	cout << "Jump Enter" << endl;
#endif // _DEBUG
}

void CJumpState::OnStateTick()
{
	//���� ó�� �Ǹ� �ٲٱ�
	if (m_pOwnerModel->Is_Finish_Animation())
	{
		Go_Idle();

		Go_Loop();

		Go_HardLand();
	}
}

void CJumpState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Jump Exit" << endl;
#endif // _DEBUG
}

void CJumpState::Go_Idle()
{
	//����Ű�� ���ٸ� Idle��
	if (false == *m_pIsDirectionKeyPressed)
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Soft_v2_anm"));
	}
	Set_StateMachine(TEXT("Idle"));
}

void CJumpState::Go_Loop()
{
	//����Ű�� ������ �ִٸ� Loop��
	if (true == *m_pIsDirectionKeyPressed)
	{
		switch (*m_pIsSprint)
		{
		case CStateContext::JOGING:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_2Jog_RU_anm"));
		}
		break;

		case CStateContext::SPRINT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_2Sprint_v2_anm"));
		}
		break;

		default:
			break;
		}
		Set_StateMachine(TEXT("Move Loop"));
	}
}

void CJumpState::Go_HardLand()
{
	//���� �ִϸ��̼��� fall loop��� �ϵ巣��
	if (!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_BM_Fall_Loop_v2_anm")))
	{
		if (false == *m_pIsDirectionKeyPressed)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Hard_v2_anm"));
		}
		else if (true == *m_pIsDirectionKeyPressed)
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_BM_Land_Hard_2Jog_v2_anm"));
		}
		Set_StateMachine(TEXT("Hard Land"));
	}
}

CJumpState* CJumpState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJumpState* pInstance = New CJumpState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CJumpState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CJumpState::Clone(void* pArg)
{
	CJumpState* pInstance = New CJumpState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CJumpState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}