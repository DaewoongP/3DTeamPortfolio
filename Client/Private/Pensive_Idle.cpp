#include "Pensive_Idle.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext_Enemy.h"

CPensive_Idle::CPensive_Idle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine_Enemy(_pDevice,_pContext)
{

}

CPensive_Idle::CPensive_Idle(const CPensive_Idle& rhs)
	:CStateMachine_Enemy(rhs)
{

}

HRESULT CPensive_Idle::Initialize(void* pArg)
{
	if (FAILED(CStateMachine_Enemy::Initialize(pArg)))
	{
		__debugbreak();
		return E_FAIL;
	}
	//0~2초 사이의 랜덤한 휴식기간
	m_fMinChangeTimer = 0.f;
	m_fMaxChangeTimer = 2.f;
	m_fActionSetTimer = Random_Generator(m_fMinChangeTimer, m_fMaxChangeTimer);
	m_fActionTimer = m_fActionSetTimer;
	return S_OK;
}

void CPensive_Idle::Tick(_float fTimeDelta)
{
	OnStateTick(fTimeDelta);
}

void CPensive_Idle::Late_Tick(_float fTimeDelta)
{
}

void CPensive_Idle::OnStateEnter(void* _pArg)
{
	//첫 실행시 등장 애니메이션을 재생합니다.
	Change_Animation(TEXT("Idle"));
	*m_StateMachineDesc.pisAttackable = true;
	m_fActionSetTimer = Random_Generator(m_fMinChangeTimer, m_fMaxChangeTimer);
	m_fActionTimer = m_fActionSetTimer;
}

void CPensive_Idle::OnStateTick(_float fTimeDelta)
{
	m_fActionTimer -= fTimeDelta;
	//애니메이션 끝났으면?
	_bool result = (m_fActionTimer < 0);
	
	if (result)
	{
		/*패턴*/
		// 1. 땅밟기 << 단발
		// 2. 해머 << 단발
		// 3. 소드 << 단발
		// 4. 차징 << 단발
		// 5. 원기옥 
		
		_uint AttackAction = _uint(m_fActionSetTimer / m_fMaxChangeTimer * 4);
		cout << AttackAction << endl;
		*m_StateMachineDesc.pAttackType = AttackAction;
		switch (AttackAction)
		{
		//case 0: //해머
		//	if (*m_StateMachineDesc.pPhase == 2)
		//	{
		//		cout << "해머를 시전합니다" << endl;
		//		Change_Animation(TEXT("Attack_Mace"));
		//		Set_StateMachine(TEXT("Physical_Attack"));
		//		break;
		//	}
		//	break;
		//case 1: // 땅밟기
		//	cout << "땅밟기를 시전합니다." << endl;
		//	Change_Animation(TEXT("Attack_Ground"));
		//	Set_StateMachine(TEXT("Physical_Attack"));
		//	break;
		//case 2: // 소드
		//	if (*m_StateMachineDesc.pPhase == 2)
		//	{
		//		cout << "칼 공격을 실행합니다" << endl;
		//		Change_Animation(TEXT("Attack_Meteor"));
		//		Set_StateMachine(TEXT("Physical_Attack"));
		//		break;
		//	}
		//case 3: // 원기옥
		//	cout << "원기옥을 실행합니다." << endl;
		//	Change_Animation(TEXT("Attack_Orb_Start"));
		//	Set_StateMachine(TEXT("Orb_Attack"));
		//	break;
		default:
			cout << "칼 공격을 실행합니다" << endl;
			Change_Animation(TEXT("Attack_Meteor"));
			Set_StateMachine(TEXT("Physical_Attack"));
			break;

			cout << "안할건데?." << endl;
			break;
		}
		m_fActionSetTimer = Random_Generator(m_fMinChangeTimer, m_fMaxChangeTimer);
		m_fActionTimer = m_fActionSetTimer;
	}
}

void CPensive_Idle::OnStateExit()
{
}

CPensive_Idle* CPensive_Idle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive_Idle* pInstance = New CPensive_Idle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CPensive_Idle::Clone(void* pArg)
{
	CPensive_Idle* pInstance = New CPensive_Idle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive_Idle::Free()
{
	CStateMachine_Enemy::Free();
}
