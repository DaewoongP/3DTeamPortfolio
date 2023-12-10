#include "..\Public\Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_AABB::CBounding_AABB(const CBounding_AABB& rhs)
	: CBounding(rhs)
	, m_pAABB_Original(New DirectX::BoundingBox(*rhs.m_pAABB_Original))
	, m_pAABB(New DirectX::BoundingBox(*m_pAABB_Original))
{
}

void CBounding_AABB::Set_BoundingDesc(void* pBoundingDesc)
{
	if (nullptr != pBoundingDesc)
	{
		m_pAABB_Original->Extents = static_cast<BOUNDINGAABBDESC*>(pBoundingDesc)->vExtents;
		m_pAABB_Original->Center = static_cast<BOUNDINGAABBDESC*>(pBoundingDesc)->vPosition;

		*m_pAABB = *m_pAABB_Original;
	}
}

HRESULT CBounding_AABB::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pAABB_Original = New DirectX::BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

HRESULT CBounding_AABB::Initialize(void* pBoundingDesc)
{
	Set_BoundingDesc(pBoundingDesc);

	return S_OK;
}

void CBounding_AABB::Tick(_float4x4 WorldMatrix)
{
	// ���� ��ü���� �ش� �̵������ �޾Ƽ� ó���� ���� �ٸ� ��ü�� �Ѱ� �ְ�
	// �� �ٸ���ü�� ���� �׸�.
	m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(WorldMatrix));
}

_bool CBounding_AABB::RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist)
{
	return m_pAABB->Intersects(vOrigin, XMVector3Normalize(vDirection), fDist);
}

_bool CBounding_AABB::Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox)
{
	switch (eColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		return m_pAABB->Intersects(*static_cast<CBounding_Sphere*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_AABB:
		return IntersectAABB(pOtherBounding, pCollisionBox);
	case Engine::CCollider::TYPE_OBB:
		return m_pAABB->Intersects(*static_cast<CBounding_OBB*>(pOtherBounding)->Get_Bounding());
	}

	return false;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(_float4 vColor)
{
	// �θ��� begin�� end���� directX���� �������ִ� �������� ��ü�� ���� �׸���.
	__super::Begin();

	// ��� ��ȯ�� ���� ������ �׸���.
	DX::Draw(m_pPrimitiveBatch, *m_pAABB, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

_float4x4 CBounding_AABB::Remove_Rotation(_float4x4 TransformMatrix)
{
	_float4x4	ResultMatrix = TransformMatrix;

	//_float4 vRight = TransformMatrix.Right();

	//ResultMatrix.m[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	//ResultMatrix.m[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	//ResultMatrix.m[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

_bool CBounding_AABB::IntersectAABB(CBounding* pOtherBounding, _float3* pCollisionBox)
{
	const DirectX::BoundingBox* otherAABB = static_cast<CBounding_AABB*>(pOtherBounding)->m_pAABB;
	_float3 vCenterLine, vRadius;
	ZEROMEM(&vCenterLine);

	vCenterLine = XMVectorAbs(m_pAABB->Center - otherAABB->Center);

	vRadius = m_pAABB->Extents + otherAABB->Extents;
	
	if (true == XMVector3GreaterOrEqual(vRadius, vCenterLine))
	{
		*pCollisionBox = (vRadius - vCenterLine) / vRadius;

		return true;
	}

	return false;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_AABB* pInstance = New CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding* CBounding_AABB::Clone(void* pBoundingDesc)
{
	CBounding_AABB* pInstance = New CBounding_AABB(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_AABB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
}
