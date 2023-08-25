#include "..\Public\Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_OBB::CBounding_OBB(const CBounding_OBB& rhs)
	: CBounding(rhs)
	, m_pOBB_Original(new DirectX::BoundingOrientedBox(*rhs.m_pOBB_Original))
	, m_pOBB(new DirectX::BoundingOrientedBox(*m_pOBB_Original))
{
}

void CBounding_OBB::Set_BoundingDesc(void* pBoundingDesc)
{
	if (nullptr != pBoundingDesc)
	{
		m_pOBB_Original->Extents = static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vExtents;
		m_pOBB_Original->Center = static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vPosition;
		XMStoreFloat4(&m_pOBB_Original->Orientation,
			XMQuaternionRotationRollPitchYaw(
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.x,
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.y,
				static_cast<BOUNDINGOBBDESC*>(pBoundingDesc)->vRotation.z));

		*m_pOBB = *m_pOBB_Original;
	}
}

HRESULT CBounding_OBB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pOBB_Original = new DirectX::BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CBounding_OBB::Initialize(void* pBoundingDesc)
{
	Set_BoundingDesc(pBoundingDesc);

	return S_OK;
}

void CBounding_OBB::Tick(_float4x4 WorldMatrix)
{
	// ���� ��ü���� �ش� �̵������ �޾Ƽ� ó���� ���� �ٸ� ��ü�� �Ѱ� �ְ�
	// �� �ٸ���ü�� ���� �׸�.
	m_pOBB_Original->Transform(*m_pOBB,WorldMatrix);
}

_bool CBounding_OBB::RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist)
{
	return m_pOBB->Intersects(vOrigin, XMVector3Normalize(vDirection), fDist);
}

_bool CBounding_OBB::Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox)
{
	switch (eColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		return m_pOBB->Intersects(*static_cast<CBounding_Sphere*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_AABB:
		return m_pOBB->Intersects(*static_cast<CBounding_AABB*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_OBB:
		return m_pOBB->Intersects(*static_cast<CBounding_OBB*>(pOtherBounding)->Get_Bounding());
	}

	return false;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(_float4 vColor)
{
	// �θ��� begin�� end���� directX���� �������ִ� �������� ��ü�� ���� �׸���.
	__super::Begin();

	// ��� ��ȯ�� ���� ������ �׸���.
	DX::Draw(m_pPrimitiveBatch, *m_pOBB, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding* CBounding_OBB::Clone(void* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_OBB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
}
