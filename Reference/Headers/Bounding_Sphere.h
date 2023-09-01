#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL CBounding_Sphere final : public CBounding
{
public:
	typedef struct tagBoundingSphereDesc : public CBounding::tagBoundingDesc
	{
		_float		fRadius;

	}BOUNDINGSPHEREDESC;

private:
	explicit CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding_Sphere(const CBounding_Sphere& rhs);
	virtual ~CBounding_Sphere() = default;

public:
	const DirectX::BoundingSphere* Get_Bounding() const {
		return m_pSphere;
	}
	virtual _float3 Get_CenterPosition() const override { return m_pSphere->Center; }
	virtual void Set_BoundingDesc(void* pBoundingDesc) override;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pBoundingDesc);
	virtual void Tick(_float4x4 WorldMatrix) override;

public:
	virtual _bool Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox);
	virtual _bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f)) override;
#endif // _DEBUG

private:
	// ��� ��ȯ�� ��ø�Ǿ� ó���� �� �ֱ� ������ �ʿ��� ���� ��ü.
	DirectX::BoundingSphere* m_pSphere_Original = { nullptr };
	// ��� ��ȯ�� ���������� �ް� �������ϴ� ��ü.
	DirectX::BoundingSphere* m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END