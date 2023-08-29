#include "Collision_Manager.h"
#include "Collider.h"

IMPLEMENT_SINGLETON(CCollision_Manager)

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Add_Collider(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pCollider)
{
	if (nullptr == pCollider)
		return E_FAIL;

	m_Colliders[eCollisionType].push_back(pCollider);

	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollision_Manager::Tick()
{
	Collision(COLLISIONDESC::COLTYPE_PLAYER, COLLISIONDESC::COLTYPE_MONSTER);

	ClearColliders();
}

void CCollision_Manager::Collision(COLLISIONDESC::COLTYPE eSourType, COLLISIONDESC::COLTYPE eDestType)
{
	for (auto& pSourCollider : m_Colliders[eSourType])
	{
		for (auto& pDestCollider : m_Colliders[eDestType])
		{
			if (pSourCollider == pDestCollider)
				continue;

			_float3 vCollisionBox = _float3(0.f, 0.f, 0.f);

			// 충돌 체크
			if (true == pSourCollider->Intersects(pDestCollider, &vCollisionBox))
			{
				// 충돌 하였으면 서로 정보 교환
				pSourCollider->OnCollision(eDestType, pDestCollider);
				pDestCollider->OnCollision(eSourType, pSourCollider);
			}
		}
	}
}

void CCollision_Manager::ClearColliders()
{
	for (_uint i = 0; i < COLLISIONDESC::COLTYPE_END; ++i)
	{
		for (auto& pCollider : m_Colliders[i])
		{
			Safe_Release(pCollider);
		}
		m_Colliders[i].clear();
	}
}

void CCollision_Manager::Free()
{
	ClearColliders();
}