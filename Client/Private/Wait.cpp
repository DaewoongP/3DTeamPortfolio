#include "Wait.h"

#include "GameInstance.h"

CWait::CWait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CWait::CWait(const CWait& rhs)
	: CBehavior(rhs)
{
}

HRESULT CWait::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CWait::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	BEGININSTANCE;
	_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
	ENDINSTANCE;

	if (fInterval >= m_fLimit)
		return BEHAVIOR_SUCCESS;

	return BEHAVIOR_RUNNING;
}

void CWait::Reset_Behavior(HRESULT result)
{
	BEGININSTANCE;
	m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
	ENDINSTANCE;
}

CWait* CWait::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWait* pInstance = new CWait(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CWait* CWait::Clone(void* pArg)
{
	CWait* pInstance = new CWait(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWait");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWait::Free()
{
	__super::Free();
}
