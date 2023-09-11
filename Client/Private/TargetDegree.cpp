#include "TargetDegree.h"

#include "GameObject.h"
#include "BlackBoard.h"
#include "Transform.h"

CTargetDegree::CTargetDegree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CTargetDegree::CTargetDegree(const CTargetDegree& rhs)
	: CBehavior(rhs)
{
}

HRESULT CTargetDegree::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTargetDegree::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
		return BEHAVIOR_FAIL;

	CGameObject* pTarget = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("pTarget", pTarget)))
	{
		MSG_BOX("[CTargetDegree] Failed Get_Type pTarget");
		return E_FAIL;
	}

	_float3 vTargetPosition = pTarget->Get_Transform()->Get_Position();
	_float3 vPosition = m_pOwnerTransform->Get_Position();

	_float3 vLook = m_pOwnerTransform->Get_Look();
	_float3 vDirection = vTargetPosition - vPosition;
	vLook.Normalize();
	vDirection.Normalize();

	_float fCrossY = vLook.Cross(vDirection).y;
	_bool isLeft = { false };
	isLeft = (0.f < fCrossY);
	
	if (FAILED(m_pBlackBoard->Set_Type("isTargetToLeft", isLeft)))
	{
		MSG_BOX("[CTargetDegree] Failed Set_Type fTargetToLR");
		return E_FAIL;
	}

	_float fDegree = XMConvertToDegrees(acosf(vLook.Dot(vDirection))); 
	
	if (FAILED(m_pBlackBoard->Set_Type("fTargetToDegree", fDegree)))
	{
		MSG_BOX("[CTargetDegree] Failed Set_Type fTargetToLR");
		return E_FAIL;
	}

	return BEHAVIOR_SUCCESS;
}

CTargetDegree* CTargetDegree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTargetDegree* pInstance = new CTargetDegree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTargetDegree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTargetDegree* CTargetDegree::Clone(void* pArg)
{
	CTargetDegree* pInstance = new CTargetDegree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTargetDegree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTargetDegree::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerTransform);
	}
}
