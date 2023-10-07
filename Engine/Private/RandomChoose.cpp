#include "RandomChoose.h"
#include "Timer_Manager.h"

#include "Calculator.h"
#include "BlackBoard.h"
#include "Decorator.h"

CRandomChoose::CRandomChoose(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CRandomChoose::CRandomChoose(const CBehavior& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRandomChoose::Initialize(void* pArg)
{
	/* 쿨타임 */
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

HRESULT CRandomChoose::Tick(const _float& fTimeDelta)
{
	if (0 == m_Behaviors.size())
		return E_FAIL;

	// 데코레이터 확인
	if (false == Check_Decorators())
		return BEHAVIOR_FAIL;

	m_ReturnData = (*m_iterCurBehavior)->Tick(fTimeDelta);

	switch (m_ReturnData)
	{
	case BEHAVIOR_RUNNING:

		if (true == Check_ChangeConditions(BEHAVIOR_RUNNING))
		{
			(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
			Set_Random_Behavior();
		}

		return BEHAVIOR_RUNNING;

	case BEHAVIOR_SUCCESS:
		Check_End_Decorators();
		Check_Success_Decorators();

		if (true == Check_ChangeConditions(BEHAVIOR_SUCCESS))
		{
			(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
			Set_Random_Behavior();
		}

		return BEHAVIOR_SUCCESS;

	case BEHAVIOR_FAIL:
		Check_End_Decorators();
		Check_Fail_Decorators();

		if (true == Check_ChangeConditions(BEHAVIOR_FAIL))
		{
			(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
			Set_Random_Behavior();
		}

		return BEHAVIOR_FAIL;

	case BEHAVIOR_END:
	default:
		return E_FAIL;
	}
}

HRESULT CRandomChoose::Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight)
{
	if (FAILED(__super::Assemble_Behavior(BehaviorTag, pBehavior)))
		return E_FAIL;

	m_ChildWeights.push_back(fWeight);

	CCalculator* pCalculator = CCalculator::GetInstance();
	Safe_AddRef(pCalculator);

	_uint iIndex = pCalculator->RandomChoose(m_ChildWeights, (_uint)m_Behaviors.size());

	Safe_Release(pCalculator);

	if (-1 == iIndex)
		return E_FAIL;

	while (0 < iIndex--)
		++m_iterCurBehavior;

	return S_OK;
}

void CRandomChoose::Reset_Behavior(HRESULT result)
{
	if (BEHAVIOR_RUNNING == m_ReturnData &&	// 현재 행동이 진행중이었는데
		BEHAVIOR_RUNNING != result)			// 상위 노드에서 상태가 바뀐경우
	{
		(*m_iterCurBehavior)->Reset_Behavior(result);
		Set_Random_Behavior();
	}

	if (BEHAVIOR_SUCCESS == result)
	{
		CTimer_Manager* pTimerManager = CTimer_Manager::GetInstance();
		Safe_AddRef(pTimerManager);
		m_fPreWorldTimeAcc = pTimerManager->Get_World_TimeAcc();
		Safe_Release(pTimerManager);
	}

	m_ReturnData = result;
}

void CRandomChoose::Set_Random_Behavior()
{
	m_iterCurBehavior = m_Behaviors.begin();

	CCalculator* pCalculator = CCalculator::GetInstance();
	Safe_AddRef(pCalculator);

	_uint iIndex = pCalculator->RandomChoose(m_ChildWeights, (_uint)m_Behaviors.size());

	Safe_Release(pCalculator);

	if (-1 == iIndex)
		return;

	while (0 < iIndex--)
		++m_iterCurBehavior;
}

HRESULT CRandomChoose::Add_Change_Condition(HRESULT ReturnType, function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func || 
		0 > ReturnType || BEHAVIOR_END <= ReturnType)
		return E_FAIL;

	CDecorator* pDecoration = CDecorator::Create(Func);
	if (nullptr == pDecoration)
		return E_FAIL;

	m_ConditionFunctions[ReturnType].push_back(pDecoration);

	return S_OK;
}

_bool CRandomChoose::Check_ChangeConditions(HRESULT eBehaviorType)
{
	if (0 == m_ConditionFunctions[eBehaviorType].size())
		return false;

	for (auto& Deco : m_ConditionFunctions[eBehaviorType])
	{
		if (false == Deco->Is_Execute(m_pBlackBoard))
			return false;
	}

	return true;
}

CRandomChoose* CRandomChoose::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandomChoose* pInstance = New CRandomChoose(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRandomChoose");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandomChoose* CRandomChoose::Clone(void* pArg)
{
	CRandomChoose* pInstance = New CRandomChoose(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRandomChoose");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandomChoose::Free()
{
	__super::Free();

	for (_uint i = 0; i < BEHAVIOR_END; ++i)
	{
		for (auto pDecorator : m_ConditionFunctions[i])
			Safe_Release(pDecorator);
	}
}
