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

	BEGININSTANCE;

	pGameInstance->Add_Timer(TEXT("Stupefy"), false, 0.2f);

	ENDINSTANCE;

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
		//m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"));
		Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"), false);
		*m_StateMachineDesc.pisFinishAnimation = false;
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
			BEGININSTANCE;

			//����ũ�� ���� ����

			//Ÿ���� ���� ����
			_float3 vAxisForTarget = *pGameInstance->Get_CamLook();

			vAxisForTarget.Normalize();

			//���� Ÿ�Կ� ���� �Ŀ�
			_float fPower = 0.1f;


			//���ϰ� �¾ҳ� ���ϰ� �¾ҳ�
			switch (ProtegoStateDesc->iHitType)
			{
			case HIT_LIGHT:
			{
				//�ִϸ��̼� ����
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"));
				Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"), false);

				// maintimer�� 0.2������� 0.4�ʵ��� ó���ϰڴ�
				pGameInstance->Set_SlowTime(TEXT("MainTimer"), 0.2f, 0.4f);

				pGameInstance->Set_Shake(
					CCamera_Manager::SHAKE_PRIORITY_2,
					CCamera_Manager::SHAKE_TYPE_TRANSLATION,
					CCamera_Manager::SHAKE_AXIS_SET,
					CEase::IN_EXPO,
					12.5f,
					0.08f,
					fPower,
					CCamera_Manager::SHAKE_POWER_DECRECENDO,
					-vAxisForTarget);
			}
			break;
			case HIT_HEABY:
			{
				//�ִϸ��̼� ����
				m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"));
				Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"), false);
				
				fPower *= 5.0f;

				// maintimer�� 0.2������� 0.4�ʵ��� ó���ϰڴ�
				pGameInstance->Set_SlowTime(TEXT("MainTimer"), 0.2f, 1.0f);

				pGameInstance->Set_Shake(
					CCamera_Manager::SHAKE_PRIORITY_2,
					CCamera_Manager::SHAKE_TYPE_TRANSLATION,
					CCamera_Manager::SHAKE_AXIS_SET,
					CEase::IN_EXPO,
					5.0f,
					0.2f,
					fPower,
					CCamera_Manager::SHAKE_POWER_DECRECENDO,
					-vAxisForTarget);
			}
			break;
			default:
				break;
			}

			

			ENDINSTANCE;
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
	//�����װ� �ִϸ��̼��� �ƴ϶�� ���̵�� ����
	else if (false == wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Start_anm")) ||
		false == wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Loop_anm")) ||
		false == wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm")) ||
		false == wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm")) ||
		false == wcscmp(m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm")))
	{
	}
	else
	{
		Set_StateMachine(TEXT("Idle"));
	}

	BEGININSTANCE;
	if (false == pGameInstance->Check_Timer(TEXT("Stupefy")))
	{
		//m_StateMachineDesc.pCameraTransform->LookAt((*m_StateMachineDesc.ppTarget)->Get_Transform()->Get_Position());
	}
	ENDINSTANCE;
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

		pGameInstance->Reset_Timer(TEXT("Stupefy"));

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
