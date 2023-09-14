#include "Sequence.h"

CSequence::CSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CSequence::CSequence(const CSequence& rhs)
	: CBehavior(rhs)
{
}

HRESULT CSequence::Tick(const _float& fTimeDelta)
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
		++m_iterCurBehavior;

		if (m_iterCurBehavior == m_Behaviors.end())
		{
			m_iterCurBehavior = m_Behaviors.begin();
			return BEHAVIOR_SUCCESS;
		}
		else
			return Tick(fTimeDelta);

	case BEHAVIOR_FAIL:
		(*m_iterCurBehavior)->Reset_Behavior();
		m_iterCurBehavior = m_Behaviors.begin();
		return BEHAVIOR_FAIL;

	case BEHAVIOR_ERROR:
		return BEHAVIOR_ERROR;
	}

	return E_FAIL;
}

CSequence* CSequence::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence* pInstance = new CSequence(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence* CSequence::Clone(void* pArg)
{
	CSequence* pInstance = new CSequence(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSequence");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence::Free()
{
	CBehavior::Free();
}
