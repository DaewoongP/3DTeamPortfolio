#pragma once
/* =============================================== */
// 
//	정 : 박대웅
//	부 :
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
	// 행렬 변환이 중첩되어 처리될 수 있기 때문에 필요한 원본 객체.
	DirectX::BoundingSphere* m_pSphere_Original = { nullptr };
	// 행렬 변환을 직접적으로 받고 렌더링하는 객체.
	DirectX::BoundingSphere* m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CBounding_Sphere* Clone(void* pBoundingDesc) override;
	virtual void Free() override;
};

END