#include "Sequence.h"
#include "Timer_Manager.h"

CSequence::CSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSequence::CSequence(const CSequence& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSequence::Initialize(void* pArg)
{
	/* ÄðÅ¸ÀÓ */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			CTimer_Manager* pTimerManager = CTimer_Manager::GetInstance();
			Safe_AddRef(pTimerManager);
			_float fInterval = pTimerManager->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			Safe_Release(pTimerManager);

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CSequence::Tick(const _float& fTimeDelta)
{
	if (0 == m_Behaviors.size())
		return E_FAIL;

	if (false == Check_Decorators())
		return BEHAVIOR_FAIL;

	m_ReturnData = (*m_iterCurBehavior)->Tick(fTimeDelta);

	switch (m_ReturnData)
	{
	case BEHAVIOR_RUNNING:
		return BEHAVIOR_RUNNING;

	case BEHAVIOR_SUCCESS:
		(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_Behaviors.end())
		{
			Check_End_Decorators();
			Check_Success_Decorators();
			m_iterCurBehavior = m_Behaviors.begin();
			return BEHAVIOR_SUCCESS;
		}
		else
			return Tick(fTimeDelta);

	case BEHAVIOR_FAIL:
		Check_End_Decorators();
		Check_Fail_Decorators();

		(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
		m_iterCurBehavior = m_Behaviors.begin();
		return BEHAVIOR_FAIL;

	case BEHAVIOR_END:
	default:
		return E_FAIL;
	}
}

void CSequence::Reset_Behavior(HRESULT result)
{
	if (BEHAVIOR_SUCCESS == result)
	{
		CTimer_Manager* pTimerManager = CTimer_Manager::GetInstance();
		Safe_AddRef(pTimerManager);
		m_fPreWorldTimeAcc = pTimerManager->Get_World_TimeAcc();
		Safe_Release(pTimerManager);
	}

	(*m_iterCurBehavior)->Reset_Behavior(result);
	m_iterCurBehavior = m_Behaviors.begin();

	m_ReturnData = result;
}

CSequence* CSequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence* pInstance = New CSequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence* CSequence::Clone(void* pArg)
{
	CSequence* pInstance = New CSequence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence::Free()
{
	__super::Free();
}
