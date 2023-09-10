#include "RandomChoose.h"

#include "Calculator.h"

CRandomChoose::CRandomChoose(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CRandomChoose::CRandomChoose(const CBehavior& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRandomChoose::Tick(const _float& fTimeDelta)
{
	// 데코레이터 확인
	if (0 == m_Behaviors.size())
		return E_FAIL;

	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	HRESULT hr = (*m_iterCurBehavior)->Tick(fTimeDelta);

#ifdef _DEBUG
	(*m_iterCurBehavior)->Set_ReturnData(hr);
	m_ReturnData = hr;
#endif // _DEBUG

	if (BEHAVIOR_RUNNING != hr)
	{
		CCalculator* pCalculator = CCalculator::GetInstance();
		Safe_AddRef(pCalculator);

		_uint iIndex = pCalculator->RandomChoose(m_ChildWeights, (_uint)m_Behaviors.size());

		Safe_Release(pCalculator);

		if (-1 == iIndex)
			return E_FAIL;

		while (0 < iIndex--)
			++m_iterCurBehavior;
	}

	return hr;
}

HRESULT CRandomChoose::Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight)
{
	if (FAILED(__super::Assemble_Behavior(BehaviorTag, pBehavior)))
		return E_FAIL;

	m_ChildWeights.push_back(fWeight);

	CCalculator* pCalculator = CCalculator::GetInstance();
	Safe_AddRef(pCalculator);

	_uint iIndex = pCalculator->RandomChoose(m_ChildWeights, (_uint)m_Behaviors.size());

	if (-1 == iIndex)
		return E_FAIL;

	while (0 < iIndex--)
		++m_iterCurBehavior;

	Safe_Release(pCalculator);

	return S_OK;
}

CRandomChoose* CRandomChoose::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandomChoose* pInstance = new CRandomChoose(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRandomChoose");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandomChoose* CRandomChoose::Clone(void* pArg)
{
	CRandomChoose* pInstance = new CRandomChoose(*this);

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
}
