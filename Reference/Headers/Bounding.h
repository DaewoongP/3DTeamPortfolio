#pragma once
/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Base.h"
#include "Collider.h"

#ifdef _DEBUG
#include "DebugDraw.h"
#endif // _DEBUG

BEGIN(Engine)

class ENGINE_DLL CBounding abstract : public CBase
{
public:
	typedef struct tagBoundingDesc
	{
		_float3		vPosition;
	}BOUNDINGDESC;

protected:
	explicit CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBounding(const CBounding& rhs);
	virtual ~CBounding() = default;

public:
	virtual _float3 Get_CenterPosition() const PURE;
	virtual void Set_BoundingDesc(void* pBoundingDesc) PURE;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pBoundingDesc) PURE;
	virtual void Tick(_float4x4 WorldMatrix) PURE;

public:
	virtual _bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist) PURE;
	virtual _bool Intersects(CCollider::TYPE eColliderType, CBounding* pOtherBounding, _float3* pCollisionBox) PURE;

#ifdef _DEBUG
public:
	// effect, batch ���� ���� ������ �غ�
	HRESULT Begin();
	virtual HRESULT Render(_float4 vColor = _float4(0.f, 1.f, 0.f, 1.f)) PURE;
	HRESULT End();
#endif // _DEBUG

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	PrimitiveBatch<DirectX::VertexPositionColor>* m_pPrimitiveBatch = { nullptr };
	DirectX::BasicEffect*	m_pEffect = { nullptr };
	ID3D11InputLayout*		m_pInputLayout = { nullptr };
	
protected:
	_bool					m_isCloned = { false };

public:
	virtual CBounding* Clone(void* pBoundingDesc) PURE;
	virtual void Free() override;
};

END