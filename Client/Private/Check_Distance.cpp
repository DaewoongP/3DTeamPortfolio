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

HRESULT CCheck_Distance::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	CGameObject* pTarget = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("pTarget", pTarget)))
	{
		MSG_BOX("[CCheck_Distance] Failed Get_Type pTarget");
		return E_FAIL;
	}

	_float3 vTargetPosition = pTarget->Get_Transform()->Get_Position();
	_float3 vPosition = m_pOwnerTransform->Get_Position();
	_float fTargetDistance = _float3::Distance(vPosition, vTargetPosition);

	if (FAILED(m_pBlackBoard->Set_Type("fTargetDistance", fTargetDistance)))
	{
		MSG_BOX("[CCheck_Distance] Failed Set_Type fTargetDistance");
		return E_FAIL;
	}

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
