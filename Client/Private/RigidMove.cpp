#include "RigidMove.h"

#include "GameInstance.h"
#include "Check_Degree.h"

CRigidMove::CRigidMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CRigidMove::CRigidMove(const CRigidMove& rhs)
	: CBehavior(rhs)
{
}

HRESULT CRigidMove::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CRigidMove::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	m_fTimeAcc += fTimeDelta;
	_float3 vForce = _float3();

	switch (m_eDirection)
	{
	case Client::CRigidMove::DIR_RIGHT:
		vForce = m_pOwnerTransform->Get_Right() * m_fForce;
		break;

	case Client::CRigidMove::DIR_UP:
		vForce = m_pOwnerTransform->Get_Up() * m_fForce;
		break;

	case Client::CRigidMove::DIR_LOOK:
		vForce = m_pOwnerTransform->Get_Look() * m_fForce;
		break;

	case Client::CRigidMove::DIR_END:
	default:
		return BEHAVIOR_END;
	}

	vForce = vForce - (m_fTimeAcc / (m_fTime / 2.f)) * vForce;

	if (m_fTimeAcc < m_fTime)
	{
		m_pOwnerRigidBody->Add_Force(vForce);
		
		return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_SUCCESS;
}

void CRigidMove::Reset_Behavior(HRESULT result)
{
	m_fTimeAcc = 0.f;
}

CRigidMove* CRigidMove::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidMove* pInstance = New CRigidMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRigidMove");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRigidMove* CRigidMove::Clone(void* pArg)
{
	CRigidMove* pInstance = New CRigidMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRigidMove");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidMove::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerRigidBody);
		Safe_Release(m_pOwnerTransform);
	}
}
