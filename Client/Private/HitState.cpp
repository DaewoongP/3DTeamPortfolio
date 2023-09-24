#include "HitState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CHitState::CHitState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CHitState::CHitState(const CHitState& rhs)
	: CStateMachine(rhs)
{

}

HRESULT CHitState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHitState::Initialize(void* pArg)
{
	return S_OK;
}

void CHitState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CHitState::Late_Tick(_float fTimeDelta)
{
}

void CHitState::OnStateEnter()
{
#ifdef _DEBUG
	cout << "Hit Enter" << endl;
#endif // _DEBUG

	Set_Dir();
}

void CHitState::OnStateTick()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_I, CInput_Device::KEY_DOWN))
	{
		Go_Idle();
	}

	ENDINSTANCE;

	Hit_Tick();

	PowerfulHit_Tick();
}

void CHitState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Hit Exit" << endl;
#endif // _DEBUG
}

void CHitState::Bind_Notify()
{
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Hit_Fwd_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Hit_Bwd_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Hit_Rht_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Hit_Lft_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);

	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_01_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_01_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);

	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_Splat_01_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_Splat_anm"),TEXT("End_Animation"), m_pFuncFinishAnimation);
	
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Knockdown_Fwd_Getup_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Rct_Knockdown_Bwd_Getup_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
}

void CHitState::Go_Idle()
{
	Set_StateMachine(TEXT("Idle"));
}

void CHitState::Set_Dir()
{
#pragma region �����ϱ�
	//�ӽ�
	/*CTransform* pTargetTransform = { nullptr };

	_float3 vTargetPos = pTargetTransform->Get_Position();*/

	_float3 vTargetPos = { _float3(0.0f, 0.0f, 0.0f) };

	_float3 vPlayerPos = m_pPlayerTransform->Get_Position();

	_float3 vTargetDir = vTargetPos = vPlayerPos;

	vTargetDir = XMVectorSetY(vTargetDir, 0.0f);

	vTargetDir.Normalize();

	_float3 vPlayerLook = m_pPlayerTransform->Get_Look();

	vPlayerLook = XMVectorSetY(vPlayerLook, 0.0f);

	vPlayerLook.Normalize();





	_float fTargetAngle = vPlayerLook.Dot(vTargetDir);

	if (1.0f < fTargetAngle)
	{
		fTargetAngle = 1.0f;
	}

	fTargetAngle = acosf(fTargetAngle);

	if (0.0f > vPlayerLook.Cross(vTargetDir).y)
	{
		fTargetAngle *= -1;
	}
#pragma endregion


	_float f45Radian = XMConvertToRadians(45.0f);
	_float f90Radian = XMConvertToRadians(90.0f);
	_float f135Radian = XMConvertToRadians(135.0f);

	if (false == m_isPowerfulHit)
	{
#pragma region HitDir
		if (fTargetAngle <= f45Radian &&
			fTargetAngle >= -f45Radian)
		{
			//����
			m_iHitDir = (_uint)HITDIR_FRONT;
		}
		else if (fTargetAngle >= f135Radian ||
			fTargetAngle <= -f135Radian)
		{
			//�ĸ�
			m_iHitDir = (_uint)HITDIR_BACK;
		}
		else if (0.0f < fTargetAngle)
		{
			//����
			m_iHitDir = (_uint)HITDIR_RIGHT;
		}
		else if (0.0f > fTargetAngle)
		{
			//����
			m_iHitDir = (_uint)HITDIR_LEFT;
		}

		switch (m_iHitDir)
		{
		case Client::CHitState::HITDIR_FRONT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Hit_Fwd_anm"));
		}
		break;
		case Client::CHitState::HITDIR_BACK:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Hit_Bwd_anm"));
		}
		break;
		case Client::CHitState::HITDIR_RIGHT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Hit_Rht_anm"));
		}
		break;
		case Client::CHitState::HITDIR_LEFT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Hit_Lft_anm"));
		}
		break;
		case Client::CHitState::HITDIR_END:
			break;
		default:
			break;
		}
#pragma endregion
	}

	if (true == m_isPowerfulHit)
	{
#pragma region PowerfulHitDir
		if (fTargetAngle <= f90Radian &&
			fTargetAngle >= -f90Radian)
		{
			//����
			m_iPowerfulHitDir = (_uint)POWERFULHITDIR_FRONT;
		}
		else if (fTargetAngle >= f90Radian ||
			fTargetAngle <= -f90Radian)
		{
			//�ĸ�
			m_iPowerfulHitDir = (_uint)POWERFULHITDIR_BACK;
		}

		switch (m_iPowerfulHitDir)
		{
		case Client::CHitState::POWERFULHITDIR_FRONT:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_01_anm"));
		}
		break;
		case Client::CHitState::POWERFULHITDIR_BACK:
		{
			m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_KnckDn_Hvy_01_anm"));
		}
		break;
		case Client::CHitState::POWERFULHITDIR_END:
			break;
		default:
			break;
		}
#pragma endregion
	}


}

void CHitState::Hit_Tick()
{
	if (false == m_isPowerfulHit)
	{
		//�߰�Ÿ ������쿡�� �ٽ� �ִϸ��̼� ����ϰ�,(��Ÿ�� ���� ���� ����)
		
		//������ ��� ���̵�
		if (true == *m_pIsFinishAnimation)
		{
			Go_Idle();
			*m_pIsFinishAnimation = false;
		}
	}
}

void CHitState::PowerfulHit_Tick()
{
	if (true == m_isPowerfulHit)
	{
		//�߰�Ÿ�� �ִ��� ������� ����(�Ǹ� ����)

		//���� �浹�� �ִϸ��̼� ���� �ִϸ��̼� (�浹 ����)
		if (true == *m_pIsFinishAnimation && (
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_KnckDn_Hvy_Fwd_01_anm")) ||
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_KnckDn_Hvy_01_anm"))))
		{
			switch (m_iPowerfulHitDir)
			{
			case Client::CHitState::POWERFULHITDIR_FRONT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_Splat_anm"));
			}
				break;
			case Client::CHitState::POWERFULHITDIR_BACK:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_KnckDn_Hvy_Splat_01_anm"));
			}
				break;
			case Client::CHitState::POWERFULHITDIR_END:
			{

			}
				break;
			default:
				break;
			}

			*m_pIsFinishAnimation = false;
		}
		//�����ִϸ��̼��� ������ ��� �ִϸ��̼� ���
		if (true == *m_pIsFinishAnimation && (
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_KnckDn_Hvy_Splat_01_anm")) ||
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_KnckDn_Hvy_Fwd_Splat_anm"))))
		{
			switch (m_iPowerfulHitDir)
			{
			case Client::CHitState::POWERFULHITDIR_FRONT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Knockdown_Fwd_Getup_anm"));
			}
			break;
			case Client::CHitState::POWERFULHITDIR_BACK:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Rct_Knockdown_Bwd_Getup_anm"));
			}
			break;
			case Client::CHitState::POWERFULHITDIR_END:
			{

			}
			break;
			default:
				break;
			}

			*m_pIsFinishAnimation = false;
		}
		//���ִϸ��̼� ������ ��� ���̵�
		if (true == *m_pIsFinishAnimation && (
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_Knockdown_Fwd_Getup_anm")) ||
			!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Rct_Knockdown_Bwd_Getup_anm"))))
		{
			Go_Idle();
			*m_pIsFinishAnimation = false;
		}
	}
}

CHitState* CHitState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHitState* pInstance = New CHitState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CHitState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CHitState::Clone(void* pArg)
{
	CHitState* pInstance = New CHitState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CHitState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}