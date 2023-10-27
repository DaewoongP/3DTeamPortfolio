#include "Broom_Begin.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_Begin::CBroom_Begin(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_Begin::CBroom_Begin(const CBroom_Begin& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_Begin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Begin::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

void CBroom_Begin::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_Begin::Late_Tick(_float fTimeDelta)
{
}

void CBroom_Begin::OnStateEnter(void* _pArg)
{
	//이동 상태에 따라 다른 탑승모션.
	switch (*m_StateMachineDesc.piMoveType)
	{
	case CPlayer::MOVETYPE_NONE:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Hover_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Hover_anm"));
		break;
	case CPlayer::MOVETYPE_JOGING:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Fly_fWalk_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Fly_fWalk_anm"));
		break;
	case CPlayer::MOVETYPE_SPRINT:
		m_StateMachineDesc.pOwnerModel->Change_Animation(TEXT("Hu_Broom_Mount_Fly_fJog_anm"));
		Change_Animation(TEXT("Hu_Broom_Mount_Fly_fJog_anm"));
		break;
	default:
		break;
	}
	m_isAppearTrigger = true;
	m_fTimeAcc = 0.f;
	m_StateMachineDesc.pRigidBody->Set_LinearDamping(2.f);
	m_StateMachineDesc.pRigidBody->Set_Gravity(false);

	_float4x4 OffsetMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f)
		* XMMatrixRotationY(XMConvertToRadians(100)) * XMMatrixRotationZ(XMConvertToRadians(110))
		* XMMatrixTranslation(0.53f, 0.f, -0.99f);

	_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
		XMQuaternionRotationAxis(XMVector3Normalize(_float3(1, 0, 0)), 1 * -10.f));

	_float3 vRight = OffsetMatrix.Right();
	_float3 vUp = OffsetMatrix.Up();
	_float3 vLook = OffsetMatrix.Look();

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	memcpy(&OffsetMatrix.m[0][0], &vRight, sizeof(_float3));
	memcpy(&OffsetMatrix.m[1][0], &vUp, sizeof(_float3));
	memcpy(&OffsetMatrix.m[2][0], &vLook, sizeof(_float3));

	m_StateMachineDesc.pBroom->Set_Offset_Matrix(OffsetMatrix);
}

void CBroom_Begin::OnStateTick()
{
	if (m_isAppearTrigger)
	{
		BEGININSTANCE;
		_float fTimeDelta = pGameInstance->Get_World_Tick()*3;
		ENDINSTANCE;
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc > 1)
		{
			m_isAppearTrigger = false;
			fTimeDelta = fTimeDelta - (m_fTimeAcc - 1);
			m_fTimeAcc = 1.f;

		}
		
		_float4x4 pMatirx = m_StateMachineDesc.pBroom->Get_Offset_Matrix();
		_float4x4 TempMatrix = pMatirx;
		_float3 vRight = TempMatrix.Right();
		_float3 vUp = TempMatrix.Up();
		_float3 vLook = TempMatrix.Look();

		//회전 해주기
		_float4x4 RotationMatrix = XMMatrixRotationQuaternion(
			XMQuaternionRotationAxis(XMVector3Normalize(_float3(1, 0, 0)), fTimeDelta * 10.f));

		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		//스케일 처리 해주기
		{
			vRight.Normalize();
			vRight = vRight * m_fTimeAcc * 0.5f;
			vUp.Normalize();
			vUp = vUp * m_fTimeAcc * 0.5f;
			vLook.Normalize();
			vLook = vLook * m_fTimeAcc * 0.5f;
			memcpy(&TempMatrix.m[0][0], &vRight, sizeof(_float3));
			memcpy(&TempMatrix.m[1][0], &vUp, sizeof(_float3));
			memcpy(&TempMatrix.m[2][0], &vLook, sizeof(_float3));
		}
		m_StateMachineDesc.pBroom->Set_Offset_Matrix(TempMatrix);
	}
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Go_Hover_Idle();
		Go_Move();
	}
}

void CBroom_Begin::OnStateExit()
{
	//지팡이 탑승 완료니까 flying으로 바까주겟음
	*m_StateMachineDesc.pIsFlying = true;
}

void CBroom_Begin::Go_Hover_Idle()
{
	//방향키를 땠다면 Idle로
	if (false == *m_StateMachineDesc.pisDirectionPressed)
	{
		Set_StateMachine(TEXT("Hover_Idle"));
	}
}

void CBroom_Begin::Go_Move()
{
	//방향키 누르고 있으면, 이동 상태에 따라 호버/플라이로 빠른 진입.
	if (true == *m_StateMachineDesc.pisDirectionPressed)
	{
		switch (*m_StateMachineDesc.piMoveType)
		{
		case CPlayer::MOVETYPE_JOGING:
			Set_StateMachine(TEXT("Hover_Move"));
			break;
		case CPlayer::MOVETYPE_SPRINT:
			Set_StateMachine(TEXT("Hover_Move"));
			break;
		}
	}
}

CBroom_Begin* CBroom_Begin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Begin* pInstance = New CBroom_Begin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_Begin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_Begin::Clone(void* pArg)
{
	CBroom_Begin* pInstance = New CBroom_Begin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_Begin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_Begin::Free()
{
	CStateMachine::Free();
}
