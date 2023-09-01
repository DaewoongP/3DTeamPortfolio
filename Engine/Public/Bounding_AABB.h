#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL CBounding_AABB final : public CBounding
{
public:
	typedef struct tagBoundingAABBDesc : public CBounding::tagBoundingDesc
	{
		_float3		vExtents;

	}BOUNDINGAABBDESC;

private:
	explicit CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_AABB(const CBounding_AABB& rhs);
	virtual ~CBounding_AABB() = default;

public:
	const DirectX::BoundingBox* Get_Bounding() const {
		return m_pAABB;
	}
	virtual _float3 Get_CenterPosition() const  override { return m_pAABB->Center; }
	virtual void Set_BoundingDesc(void* pBoundingDesc) override;
	void Set_AABB(BOUNDINGAABBDESC AABBDesc) {
		m_pAABB->Center = AABBDesc.vPosition;
		m_pAABB->Extents = AABBDesc.vExtents;
	}

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pBoundingDesc);
	virtual void Tick(_float4x4 WorldMatrix) override;

public:
	virtual _bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist) override;
	virtual _bool Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f)) override;
#endif // _DEBUG

private:
	_float4x4 Remove_Rotation(_float4x4 TransformMatrix);
	_bool IntersectAABB(CBounding* pOtherBounding, _float3* pCollisionBox);

private:
	// ��� ��ȯ�� ��ø�Ǿ� ó���� �� �ֱ� ������ �ʿ��� ���� ��ü.
	DirectX::BoundingBox* m_pAABB_Original = { nullptr };
	// ��� ��ȯ�� ���������� �ް� �������ϴ� ��ü.
	DirectX::BoundingBox* m_pAABB = { nullptr };

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END