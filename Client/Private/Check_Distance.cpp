#include "Check_Distance.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

CCheck_Distance::CCheck_Distance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CCheck_Distance::CCheck_Distance(const CCheck_Distance& rhs)
	: CBehavior(rhs)
{
}

HRESULT CCheck_Distance::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CCheck_Distance::Tick_Behavior(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	const CGameObject** ppTarget = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("cppTarget", ppTarget)))
	{
		MSG_BOX("[CCheck_Distance] Failed Get_Type cppTarget");
		return E_FAIL;
	}
	if (nullptr == *ppTarget)
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	_float3 vTargetPosition = (*ppTarget)->Get_Transform()->Get_Position();
	_float3 vPosition = m_pOwnerTransform->Get_Position();
	_float fTargetDistance = _float3::Distance(vPosition, vTargetPosition);

	if (FAILED(m_pBlackBoard->Set_Type("fTargetDistance", fTargetDistance)))
	{
		MSG_BOX("[CCheck_Distance] Failed Set_Type fTargetDistance");
		return E_FAIL;
	}

	// 거리를 비교할건데
	if (true == m_isCompareDistance)
	{
		if (true == m_isCloseDistance)
		{
			// 현재 타겟과의 거리가 목표치 보다 가까워진 경우 성공을 반환한다.
			return (m_fTargetDistance > fTargetDistance) ? BEHAVIOR_SUCCESS : BEHAVIOR_RUNNING;
		}
		else
		{
			// 현재 타겟과의 거리가 목표치 보다 멀어진 경우 성공을 반환한다.
			return (m_fTargetDistance < fTargetDistance) ? BEHAVIOR_SUCCESS : BEHAVIOR_RUNNING;
		}
	}
	else // 거리 비교 안할 거면 무조건 성공
		return BEHAVIOR_SUCCESS;
}

CCheck_Distance* CCheck_Distance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCheck_Distance* pInstance = New CCheck_Distance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCheck_Distance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCheck_Distance* CCheck_Distance::Clone(void* pArg)
{
	CCheck_Distance* pInstance = New CCheck_Distance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCheck_Distance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCheck_Distance::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerTransform);
	}
}
