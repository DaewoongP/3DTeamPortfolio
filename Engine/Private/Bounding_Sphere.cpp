#include "..\Public\Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBounding(pDevice, pContext)
{
}

CBounding_Sphere::CBounding_Sphere(const CBounding_Sphere& rhs)
	: CBounding(rhs)
	, m_pSphere_Original(New DirectX::BoundingSphere(*rhs.m_pSphere_Original))
	, m_pSphere(New DirectX::BoundingSphere(*m_pSphere_Original))
{
}

void CBounding_Sphere::Set_BoundingDesc(void* pBoundingDesc)
{
	if (nullptr != pBoundingDesc)
	{
		m_pSphere_Original->Radius = static_cast<BOUNDINGSPHEREDESC*>(pBoundingDesc)->fRadius;
		m_pSphere_Original->Center = static_cast<BOUNDINGSPHEREDESC*>(pBoundingDesc)->vPosition;

		*m_pSphere = *m_pSphere_Original;
	}
}

HRESULT CBounding_Sphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pSphere_Original = New DirectX::BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);

	return S_OK;
}

HRESULT CBounding_Sphere::Initialize(void* pBoundingDesc)
{
	Set_BoundingDesc(pBoundingDesc);

	return S_OK;
}

void CBounding_Sphere::Tick(_float4x4 WorldMatrix)
{
	// ���� ��ü���� �ش� �̵������ �޾Ƽ� ó���� ���� �ٸ� ��ü�� �Ѱ� �ְ�
	// �� �ٸ���ü�� ���� �׸�.
	m_pSphere_Original->Transform(*m_pSphere, WorldMatrix);
}

_bool CBounding_Sphere::Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox)
{
	switch (eColliderType)
	{
	case Engine::CCollider::TYPE_SPHERE:
		return m_pSphere->Intersects(*static_cast<CBounding_Sphere*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_AABB:
		return m_pSphere->Intersects(*static_cast<CBounding_AABB*>(pOtherBounding)->Get_Bounding());
	case Engine::CCollider::TYPE_OBB:
		return m_pSphere->Intersects(*static_cast<CBounding_OBB*>(pOtherBounding)->Get_Bounding());
	}

	return false;
}

_bool CBounding_Sphere::RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist)
{
	return m_pSphere->Intersects(vOrigin, XMVector3Normalize(vDirection), fDist);
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(_float4 vColor)
{
	// �θ��� begin�� end���� directX���� �������ִ� �������� ��ü�� ���� �׸���.
	__super::Begin();

	// ��� ��ȯ�� ���� ������ �׸���.
	DX::Draw(m_pPrimitiveBatch, *m_pSphere, vColor);

	__super::End();

	return S_OK;
}
#endif // _DEBUG

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBounding_Sphere* pInstance = New CBounding_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBounding_Sphere* CBounding_Sphere::Clone(void* pBoundingDesc)
{
	CBounding_Sphere* pInstance = New CBounding_Sphere(*this);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX("Failed to Cloned CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);
}
