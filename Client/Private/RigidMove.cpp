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
	_float3 vForce = m_vForce - (m_fTimeAcc / (m_fTime / 2.f)) * m_vForce;
	
	if (m_fTimeAcc < m_fTime)
	{
		m_pOwnerRigidBody->Add_Force(vForce);
		
		return BEHAVIOR_RUNNING;
	}

	return BEHAVIOR_SUCCESS;
}

void CRigidMove::Reset_Behavior(HRESULT result)
{
	m_isFirst = true;
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
	}
}
