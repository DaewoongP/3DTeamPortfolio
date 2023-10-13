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
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CProtegoState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CProtegoState::Late_Tick(_float fTimeDelta)
{
}

void CProtegoState::OnStateEnter(void* _pArg)
{
	//�̸� ���� ���̴�.
	if (nullptr == _pArg)
	{
		//���� �ִϸ��̼�
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"));
		Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"), false);
	}
	//�����߿� �¾Ҵ�!! 
	else
	{
		PROTEGOSTATEDESC* ProtegoStateDesc = static_cast<PROTEGOSTATEDESC*>(_pArg);

		if (nullptr == ProtegoStateDesc->pTransform)
			return;

		m_StateMachineDesc.pPlayerTransform->LookAt(ProtegoStateDesc->pTransform->Get_Position(), true);

		m_isHit = ProtegoStateDesc->isHit;

		m_iHittype = ProtegoStateDesc->iHitType;

		//�¾��� �� �ִϸ��̼� ���
		if (true == m_isHit)
		{
			//���ϰ� �¾ҳ� ���ϰ� �¾ҳ�
			switch (ProtegoStateDesc->iHitType)
			{
			case HIT_LIGHT:
			{
				//�ִϸ��̼� ����
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"));
				Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"), false);
			}
			break;
			case HIT_HEABY:
			{
				//�ִϸ��̼� ����
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"));
				Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"), false);
			}
			break;

			default:
				break;
			}
		}
	}
	*m_StateMachineDesc.pLumosOn = false;
}

void CProtegoState::OnStateTick()
{
	//���� ����
	if (true == *m_StateMachineDesc.pisFinishAnimation &&
		!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Start_anm")))
	{
		*m_StateMachineDesc.pisFinishAnimation = false;

		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Loop_anm"));
		Change_Animation(TEXT("Hu_Cmbt_Protego_Loop_anm"), false);
	}
	else if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CProtegoState::OnStateExit()
{
	m_isHit = false;

	m_iHittype = (_uint)HIT_END;
}

void CProtegoState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Start_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Loop_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);

	function<void()> funcPointer = [&] {(*this).Stupefy(); };

	//����������
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"), TEXT("Stupefy"), funcPointer);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"), TEXT("Stupefy"), funcPointer);
	
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm"), TEXT("Spell_Ready"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CProtegoState::Go_Idle()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation &&(
		!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Loop_anm")) ||
		!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm")) ||
		!wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm")))
		)
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

void CProtegoState::Stupefy()
{
	BEGININSTANCE;
	
	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_PRESSING))
	{
		if (nullptr != *m_StateMachineDesc.ppTarget)
		{
			m_StateMachineDesc.pPlayerTransform->LookAt((*m_StateMachineDesc.ppTarget)->Get_Transform()->Get_Position(), true);
		}

		Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm"), false);
	}
	
	ENDINSTANCE;
}

void CProtegoState::Powerful_Stupefy()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_PRESSING))
	{
		//���������� �߻�~ �̰� �÷��̾�� ó���ؾ� ������//Hu_Broom_Hover_Rct_Impact_Hvy_Rht_anm
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
