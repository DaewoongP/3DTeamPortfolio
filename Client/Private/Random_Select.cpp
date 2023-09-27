#include "Random_Select.h"

#include "GameInstance.h"
#include "BlackBoard.h"

CRandom_Select::CRandom_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRandomChoose(pDevice, pContext)
{
}

CRandom_Select::CRandom_Select(const CRandom_Select& rhs)
	: CRandomChoose(rhs)
{
}

HRESULT CRandom_Select::Initialize(void* pArg)
{
	/* 쿨타임 */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;
			
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CRandom_Select::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	if (BEHAVIOR_SUCCESS == hr)
	{
		Set_Random_Behavior();

		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;
	}
	else if (BEHAVIOR_FAIL == hr)
	{
		_bool* pIsParring = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("isParring", pIsParring)))
			return hr;

		if (true == *pIsParring)
		{
			Set_Random_Behavior();
		}
	}

	return hr;
}

void CRandom_Select::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);
	if (BEHAVIOR_RUNNING == m_ReturnData &&	// 현재 행동이 진행중이었는데
		BEHAVIOR_RUNNING != result)			// 상위 노드에서 상태가 바뀐경우
	{
		m_ReturnData = result;
		Set_Random_Behavior();
	}
}

CRandom_Select* CRandom_Select::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandom_Select* pInstance = New CRandom_Select(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandom_Select* CRandom_Select::Clone(void* pArg)
{
	CRandom_Select* pInstance = New CRandom_Select(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandom_Select::Free()
{
	__super::Free();
}
