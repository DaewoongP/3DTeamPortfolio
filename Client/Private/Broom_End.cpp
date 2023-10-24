#include "Broom_End.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_End::CBroom_End(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_End::CBroom_End(const CBroom_End& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_End::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_End::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CBroom_End::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_End::Late_Tick(_float fTimeDelta)
{
}

void CBroom_End::OnStateEnter(void* _pArg)
{
	Change_Animation(TEXT("Hu_Broom_Dismount_2Jog_anm"));

	m_isAppearTrigger = true;
	m_fTimeAcc = 0.f;
	//m_StateMachineDesc.pRigidBody->Set_LinearDamping(0.1f);
	m_StateMachineDesc.pRigidBody->Set_Gravity(true);

	_float4x4 OffsetMatrix = XMMatrixScaling(0.5001f, 0.5001f, 0.5001f)
		* XMMatrixRotationY(XMConvertToRadians(100)) * XMMatrixRotationZ(XMConvertToRadians(110))
		* XMMatrixTranslation(0.53f, 0.f, -0.99f);

	m_StateMachineDesc.pBroom->Set_Offset_Matrix(OffsetMatrix);
}

void CBroom_End::OnStateTick()
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
			XMQuaternionRotationAxis(XMVector3Normalize(_float3(1, 0, 0)), fTimeDelta * -30.f));

		vRight = XMVector3TransformNormal(vRight, RotationMatrix);
		vUp = XMVector3TransformNormal(vUp, RotationMatrix);
		vLook = XMVector3TransformNormal(vLook, RotationMatrix);

		//스케일 처리 해주기
		{
			_float3 vTempScale = {0.50001f,0.50001f ,0.50001f };
			vRight.Normalize();
			vRight = vRight * (vTempScale.x - m_fTimeAcc * 0.5f);
			vUp.Normalize();
			vUp = vUp * (vTempScale.x - m_fTimeAcc * 0.5f);
			vLook.Normalize();
			vLook = vLook * (vTempScale.x - m_fTimeAcc * 0.5f);
			memcpy(&TempMatrix.m[0][0], &vRight, sizeof(_float3));
			memcpy(&TempMatrix.m[1][0], &vUp, sizeof(_float3));
			memcpy(&TempMatrix.m[2][0], &vLook, sizeof(_float3));
		}
		m_StateMachineDesc.pBroom->Set_Offset_Matrix(TempMatrix);
	}

	//애니메이션이 끝났을 경우
	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Idle"));
		Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
	}
}

void CBroom_End::OnStateExit()
{
}

CBroom_End* CBroom_End::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_End* pInstance = New CBroom_End(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_End::Clone(void* pArg)
{
	CBroom_End* pInstance = New CBroom_End(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_End");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_End::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
