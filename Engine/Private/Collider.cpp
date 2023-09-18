#include "..\Public\Collider.h"
#include "Composite.h"
#include "GameObject.h"
#include "Component_Manager.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eColliderType(rhs.m_eColliderType)
#ifdef _DEBUG
	, m_vColor(rhs.m_vColor)
#endif // _DEBUG
{
}

_float3 CCollider::Get_BoundingCenterPosition() const
{
	return m_pBounding->Get_CenterPosition(); 
}

void CCollider::Set_BoundingDesc(void* pBoundingDesc)
{
	m_pBounding->Set_BoundingDesc(pBoundingDesc); 
}

HRESULT CCollider::Initialize_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;
#ifdef _DEBUG
	m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
#endif // _DEBUG

	switch (eColliderType)
	{
	case TYPE_SPHERE:
		m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext);
		break;
	case TYPE_AABB:
		m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext);
		break;
	case TYPE_OBB:
		m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext);
		break;
	}

	return S_OK;
}

HRESULT CCollider::Initialize(CBounding* pBounding, void* pArg)
{
	m_pBounding = pBounding->Clone(pArg);
	
	if (nullptr == m_pBounding)
		return E_FAIL;

	return S_OK;
}

void CCollider::Tick(_float4x4 TransformMatrix)
{
	if (nullptr == m_pBounding)
		return;

	m_pBounding->Tick(TransformMatrix);

	IsExitCollision();
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (nullptr == m_pBounding)
		return E_FAIL;

	m_pBounding->Render(XMLoadFloat4(&m_vColor));

	return S_OK;
}
#endif // _DEBUG

_bool CCollider::RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist)
{
	return m_pBounding->RayIntersects(vOrigin, vDirection, fDist);
}

_bool CCollider::Intersects(CCollider* pOtherCollider, _float3* pCollisionBox)
{
	return m_pBounding->Intersects(pOtherCollider->m_eColliderType, pOtherCollider->m_pBounding, pCollisionBox);
}

void CCollider::OnCollision(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pOtherCollider)
{
	if (nullptr == m_pOwner ||
		nullptr == pOtherCollider)
		return;

	COLLISIONDESC CollisionDesc;

	// ������ �浹�� ������, ��� �浹�� (Enter)
	if (false == IsCollision(pOtherCollider))
	{
		CollisionDesc.ColType = eCollisionType;
		CollisionDesc.pOtherCollider = pOtherCollider;
		CollisionDesc.pMyCollider = this;
		CollisionDesc.pOtherOwner = static_cast<CGameObject*>(pOtherCollider->Get_Owner());
		
		CollisionDesc.pOtherTransform = CollisionDesc.pOtherOwner->Get_Transform();

		m_Collisions.push_back({ false, CollisionDesc });

 		//static_cast<CGameObject*>(m_pOwner)->OnCollisionEnter(CollisionDesc);
	}
}

_bool CCollider::IsCollision(CCollider* pOtherCollider)
{
	_uint iIndex = { 0 };
	auto iter = find_if(m_Collisions.begin(), m_Collisions.end(), [&](pair<_bool, COLLISIONDESC>& pair)
		{
			if (pair.second.pOtherCollider == pOtherCollider)
			{
				pair.first = false;
				return true;
			}
			else
				++iIndex;

			return false;
		});

	if (iter == m_Collisions.end())
		return false;

	// �̹� �ʾȿ� ���־��⶧���� Stay�ΰ� Ȯ��.
	// �������� ������ �ʿ� ����.
	/*if (nullptr != m_pOwner)
		static_cast<CGameObject*>(m_pOwner)->OnCollisionStay(iter->second);*/

	return true;
}

void CCollider::IsExitCollision()
{
	if (0 == m_Collisions.size())
		return;

	auto iter = m_Collisions.begin();

	for (_uint i = 0; i < m_Collisions.size(); ++i)
	{
		if (true == m_Collisions[i].first)
		{
			//static_cast<CGameObject*>(m_pOwner)->OnCollisionExit(m_Collisions[i].second);
			
			m_Collisions.erase(iter + i);

			if (0 == i)
				break;
			--i;
		}	
	}

	for (auto pair : m_Collisions)
	{
		pair.first = true;
	}
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType)
{
	CCollider* pInstance = New CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = New CCollider(*this);

	if (FAILED(pInstance->Initialize(m_pBounding, pArg)))
	{
		MSG_BOX("Failed to Cloned CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

	m_Collisions.clear();
}