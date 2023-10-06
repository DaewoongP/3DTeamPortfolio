#include "Selector.h"

CSelector::CSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSelector::CSelector(const CSelector& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSelector::Tick(const _float& fTimeDelta)
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
		Check_End_Decorators();
		Check_Success_Decorators();

		(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
		m_iterCurBehavior = m_Behaviors.begin();

		return BEHAVIOR_SUCCESS;

	case BEHAVIOR_FAIL:
		(*m_iterCurBehavior)->Reset_Behavior(m_ReturnData);
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_Behaviors.end())
		{
			Check_End_Decorators();
			Check_Fail_Decorators();
			m_iterCurBehavior = m_Behaviors.begin();
			return BEHAVIOR_FAIL;
		}
		else
			return Tick(fTimeDelta);

	case BEHAVIOR_END:
	default:
		return E_FAIL;
	}
}

void CSelector::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);
	m_iterCurBehavior = m_Behaviors.begin();

	m_ReturnData = result;
}

CSelector* CSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector* pInstance = New CSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector* CSelector::Clone(void* pArg)
{
	CSelector* pInstance = New CSelector(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelector::Free()
{
	__super::Free();
}
