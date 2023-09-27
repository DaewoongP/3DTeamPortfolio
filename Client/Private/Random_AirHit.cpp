#include "Random_AirHit.h"

#include "GameInstance.h"
#include "BlackBoard.h"

CRandom_AirHit::CRandom_AirHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRandomChoose(pDevice, pContext)
{
}

CRandom_AirHit::CRandom_AirHit(const CRandom_AirHit& rhs)
	: CRandomChoose(rhs)
{
}

HRESULT CRandom_AirHit::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	if(true == Check_Run_Function_Decorators())
	{
		Reset_Behavior(hr);
	}

	return hr;
}

void CRandom_AirHit::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);
	if (BEHAVIOR_RUNNING == m_ReturnData &&	// 현재 행동이 진행중이었는데
		BEHAVIOR_RUNNING != result)			// 상위 노드에서 상태가 바뀐경우
	{
		m_ReturnData = result;
		Set_Random_Behavior();
	}
}

CRandom_AirHit* CRandom_AirHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandom_AirHit* pInstance = New CRandom_AirHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandom_AirHit* CRandom_AirHit::Clone(void* pArg)
{
	CRandom_AirHit* pInstance = New CRandom_AirHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandom_AirHit::Free()
{
	__super::Free();
}
