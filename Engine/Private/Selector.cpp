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

	if (false == Check_Decorations())
	{
		(*m_iterCurBehavior)->Reset_Behavior();
		return BEHAVIOR_FAIL;
	}

	HRESULT hr = (*m_iterCurBehavior)->Tick(fTimeDelta);

#ifdef _DEBUG
	(*m_iterCurBehavior)->Set_ReturnData(hr);
	m_ReturnData = hr;
#endif // _DEBUG

	switch (hr)
	{
	case BEHAVIOR_RUNNING:
		return BEHAVIOR_RUNNING;

	case BEHAVIOR_SUCCESS:
		(*m_iterCurBehavior)->Reset_Behavior();
		m_iterCurBehavior = m_Behaviors.begin();
		return BEHAVIOR_SUCCESS;

	case BEHAVIOR_FAIL:
		(*m_iterCurBehavior)->Reset_Behavior();
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_Behaviors.end())
		{
			m_iterCurBehavior = m_Behaviors.begin();
			return BEHAVIOR_FAIL;
		}
		else
			return Tick(fTimeDelta);

	case BEHAVIOR_ERROR:
		return BEHAVIOR_ERROR;
	}

	return E_FAIL;
}

CSelector* CSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSelector* pInstance = new CSelector(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSelector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSelector* CSelector::Clone(void* pArg)
{
	CSelector* pInstance = new CSelector(*this);

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
