#include "..\Public\Phase.h"

CPhase::CPhase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPhase::CPhase(const CPhase& rhs)
	: CComponent(rhs)
{
}

HRESULT CPhase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhase::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPhase::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	State_Tick(TICK);
}

void CPhase::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	State_Tick(LATE_TICK);
	Exit_Condition(STATE(m_eCurState + 1));
}

void CPhase::Set_Tick(function<void()> function, TICK_STATE eTickState, STATE eState)
{
	if (false == IsTickStateValid(eTickState))
		return;

	if (false == IsStateValid(eState))
		return;

	m_TickState[eTickState][eState] = function;
}

void CPhase::Set_ExitCondition(function<bool()> function, STATE eState)
{
	if (false == IsStateValid(eState))
		return;

	m_TransitionCondition[eState] = function;
}

void CPhase::Set_TransitionCallBack(function<void()> function, STATE eState)
{
	if (false == IsStateValid(eState))
		return;

	m_TransitionCallBack[eState] = function;
}

void CPhase::Play()
{
	Next_State(ENTER);
}

CPhase* CPhase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPhase* pInstance = New CPhase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create CPhase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhase::State_Tick(TICK_STATE eTickState)
{
	if (m_TickState[eTickState][m_eCurState]) // 함수가 들어있는지 검사.
		m_TickState[eTickState][m_eCurState](); // 함수 실행
}

void CPhase::Exit_Condition(STATE eNextState)
{
	if (m_TransitionCondition[m_eCurState]) // 상태 전이 조건이 있는지 검사
	{
		if (true == m_TransitionCondition[m_eCurState]()) // 상태 전이
		{
			Next_State(eNextState);
		}
	}
}

void CPhase::Next_State(STATE eNextState)
{
	if (false == IsStateValid(eNextState))
		return;

	if (m_TransitionCallBack[m_eCurState]) // 상태 전이 콜백이 있는지 검사.
		m_TransitionCallBack[m_eCurState](); // 상태 전이 콜백함수 호출

	m_eCurState = eNextState; // 상태 변경.
}

_bool CPhase::IsStateValid(STATE eState)
{
	return ENTER >= eState && eState <= STATE_END;
}

_bool CPhase::IsTickStateValid(TICK_STATE eState)
{
	return TICK >= eState && eState <= TICK_STATE_END;
}

CComponent* CPhase::Clone(void* pArg)
{
	CPhase* pInstance = New CPhase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone CPhase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhase::Free()
{
	__super::Free();
}
