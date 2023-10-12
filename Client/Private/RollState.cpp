#include "RollState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CRollState::CRollState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CRollState::CRollState(const CRollState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CRollState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRollState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CRollState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CRollState::Late_Tick(_float fTimeDelta)
{
}

void CRollState::OnStateEnter(void* _pArg)
{
	if (nullptr == _pArg)
	{
		return;
	}

	BEGININSTANCE;
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		_float3 vNextLook = { _float3() };

		_float4x4 matCam = *pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

		_float3 vRight = matCam.Right();
		vRight.y = 0.0f;
		vRight.Normalize();

		_float3 vUp = { _float3(0.0f,1.0f,0.0f) };

		_float3 vLook = matCam.Look();
		vLook.y = 0.0f;
		vLook.Normalize();


		if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING) ||
			pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_DOWN))
		{
			vNextLook = vLook;
		}
		else if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING) ||
			pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_DOWN))
		{
			vNextLook = -vLook;
		}

		else if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING) ||
			pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_DOWN))
		{
			vNextLook = -vRight;
		}

		else if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING) ||
			pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_DOWN))
		{
			vNextLook = vRight;
		}

		_float4x4 matDirectionPressed = XMMatrixLookAtLH(_float3(), vNextLook, vUp);

		vRight = matDirectionPressed.Right();
		vRight.y = 0.0f;
		vLook = matDirectionPressed.Look();
		vLook.y = 0.0f;

		vRight.Normalize();
		vLook.Normalize();

		_float3 vScale = m_StateMachineDesc.pPlayerTransform->Get_Scale();

		m_StateMachineDesc.pPlayerTransform->Set_Right(vRight * vScale.x);
		m_StateMachineDesc.pPlayerTransform->Set_Up(vUp * vScale.y);
		m_StateMachineDesc.pPlayerTransform->Set_Look(vLook * vScale.z);
	}
	ENDINSTANCE;
	CRollState::tagRollStateDesc* pRollStateDesc = static_cast<CRollState::tagRollStateDesc*>(_pArg);

	m_isBlink = pRollStateDesc->IsBlink;

	//전투 상태가 되어야 한다.
	*m_StateMachineDesc.piActionType = CPlayer::ACTION_CMBT;

	if (false == m_isBlink)
	{
		Change_Animation(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"));
	}
	else if (true == m_isBlink)
	{
		Change_Animation(TEXT("Blink_Start"));
		m_eBlink = BLINK_START;
	}

	*m_StateMachineDesc.pisFinishAnimation = false;
}

void CRollState::OnStateTick()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		if (false == m_isBlink)
		{
			Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
			Set_StateMachine(TEXT("Idle"));
			*m_StateMachineDesc.pisFinishAnimation = false;
		}
		else if (true == m_isBlink)
		{
			if (m_eBlink == BLINK_START)
			{
				Change_Animation(TEXT("Blink_End"));
				*m_StateMachineDesc.pisFinishAnimation = false;
				m_eBlink = BLINK_END;
			}
			else if (m_eBlink == BLINK_END)
			{
				Set_StateMachine(TEXT("Idle"));
				*m_StateMachineDesc.pisFinishAnimation = false;
			}
		}
	}
	BEGININSTANCE;
	if (m_eBlink == BLINK_START)
	{
		m_StateMachineDesc.pPlayerTransform->Move_Direction(XMVector3Normalize(m_StateMachineDesc.pPlayerTransform->Get_Look()), 14.0f * pGameInstance->Get_World_Tick());
	}
	ENDINSTANCE;
}

void CRollState::OnStateExit()
{
}

void CRollState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Blink_Start"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Blink_End"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CRollState::Go_Idle()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
		Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
		Set_StateMachine(TEXT("Idle"));
	}
}

CRollState* CRollState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRollState* pInstance = New CRollState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRollState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CRollState::Clone(void* pArg)
{
	CRollState* pInstance = New CRollState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRollState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRollState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
