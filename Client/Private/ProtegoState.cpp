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

void CProtegoState::OnStateEnter(void* _pArg)
{
	//미리 시전 중이다.
	if (nullptr == _pArg)
	{
		//시작 애니메이션
		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Start_anm"));
	}
	//시전중에 맞았다!! 
	else
	{
		PROTEGOSTATEDESC* ProtegoStateDesc = static_cast<PROTEGOSTATEDESC*>(_pArg);

#pragma region 바라보기
		//맞았으면 그놈을 바라보자
//나에서 타겟을 향한 벡터
		_float3 vDirTarget{};

		vDirTarget = ProtegoStateDesc->pTransform->Get_Position() - m_pPlayerTransform->Get_Position();

		vDirTarget = XMVectorSetY(vDirTarget, 0.0f);

		vDirTarget.Normalize();

		//내 룩 벡터
		_float3 vLook{};

		vLook = m_pPlayerTransform->Get_Look();

		vLook = XMVectorSetY(vLook, 0.0f);

		vLook.Normalize();

		_float fTargetAngle = vLook.Dot(vDirTarget);

		if (1.0f < fTargetAngle)
		{
			fTargetAngle = 1.0f;
		}

		fTargetAngle = acosf(fTargetAngle);

		if (0.0f > vLook.Cross(vDirTarget).y)
		{
			fTargetAngle *= -1;
		}

		m_pPlayerTransform->Turn(_float3(0.0f, 1.0f, 0.0f), fTargetAngle);
#pragma endregion

		m_isHit = ProtegoStateDesc->isHit;

		m_iHittype = ProtegoStateDesc->iHitType;

		//맞았을 때 애니메이션 재생
		if (true == m_isHit)
		{
			//강하게 맞았냐 약하게 맞았냐
			switch (ProtegoStateDesc->iHitType)
			{
			case HIT_LIGHT:
			{
				//애니메이션 실행
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"));
			}
			break;
			case HIT_HEABY:
			{
				//애니메이션 실행
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"));
			}
			break;

			default:
				break;
			}
		}
	}
}

void CProtegoState::OnStateTick()
{
	//루프 진행
	if (true == *m_pIsFinishAnimation &&
		!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Start_anm")))
	{
		*m_pIsFinishAnimation = false;

		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Protego_Loop_anm"));
	}
	else if (true == *m_pIsFinishAnimation)
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
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Start_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Loop_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_anm"), TEXT("End_Animation"), m_pFuncFinishAnimation);
	//스투페파이
}

void CProtegoState::Go_Idle()
{
	if (true == *m_pIsFinishAnimation &&(
		!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Loop_anm")) ||
		!wcscmp(m_pOwnerModel->Get_Animation()->Get_AnimationName(), TEXT("Hu_Cmbt_Protego_Parry_Fwd_AOE_Slide_anm")))
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
		//스투페파이 발사~ 이건 플레이어에서 처리해야 할지도
	}
	
	ENDINSTANCE;
}

void CProtegoState::Powerful_Stupefy()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_PRESSING))
	{
		//스투페파이 발사~ 이건 플레이어에서 처리해야 할지도
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
