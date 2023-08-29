#include "RootBehavior.h"

#include "BlackBoard.h"

CRootBehavior::CRootBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBehavior(pDevice, pContext)
{
}

CRootBehavior::CRootBehavior(const CRootBehavior& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRootBehavior::Initialize(void* pArg)
{
	m_pBlackBoard = CBlackBoard::Create();
	if (nullptr == m_pBlackBoard)
	{
		MSG_BOX("BlackBoard is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRootBehavior::Tick(const _float& fTimeDelta)
{
	HRESULT hr = (*m_iterCurBehavior)->Tick(fTimeDelta);
	(*m_iterCurBehavior)->Set_Return_Data(hr);

	switch (hr)
	{
	case BEHAVIOR_RUNNING:
		return S_OK;

	case BEHAVIOR_SUCCESS:
		m_iterCurBehavior = m_Behaviors.begin();
		return S_OK;

	case BEHAVIOR_FAIL:
		m_iterCurBehavior = m_Behaviors.begin();
		return S_OK;

	case BEHAVIOR_ERROR:
		return E_FAIL;
	}

	return hr;
}

HRESULT CRootBehavior::Add_Type(const string& strTypename, any Type)
{
	return m_pBlackBoard->Set_Type(strTypename, Type);
}

CRootBehavior* CRootBehavior::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRootBehavior* pInstance = new CRootBehavior(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRootBehavior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRootBehavior* CRootBehavior::Clone(void* pArg)
{
	CRootBehavior* pInstance = new CRootBehavior(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRootBehavior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRootBehavior::Free()
{
	__super::Free();
}
