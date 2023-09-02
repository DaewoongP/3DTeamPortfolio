#include "..\Public\ColliderCom.h"

CColliderCom::CColliderCom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CColliderCom::CColliderCom(const CColliderCom& rhs)
	: CComponent(rhs)
{
}

void CColliderCom::Set_Trigger(_bool isTrigger)
{
	m_isTrigger = isTrigger;
	Invalidate_TriggerState();
}

HRESULT CColliderCom::Initialize(void* pArg)
{
	//rigidBody->AddCollider(this);
	return S_OK;
}

void CColliderCom::Set_Shape(PxShape* pShape)
{
	if (nullptr == m_pShape)
		return;

	m_pShape = pShape;
	Invalidate_TriggerState();
}

void CColliderCom::Invalidate_TriggerState() const
{
	if (nullptr == m_pShape)
		return;

	m_pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !m_isTrigger);
	m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, m_isTrigger);
}

CColliderCom* CColliderCom::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CColliderCom* pInstance = new CColliderCom(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CColliderCom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CColliderCom::Clone(void* pArg)
{
	CColliderCom* pInstance = new CColliderCom(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CColliderCom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColliderCom::Free()
{
	__super::Free();
}
