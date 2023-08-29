#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_AABB, TYPE_OBB, TYPE_END };
	
private:
	explicit CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	_float3 Get_BoundingCenterPosition() const;
	void Set_BoundingDesc(void* pBoundingDesc);
#ifdef _DEBUG
	void Set_Color(_float4 vColor) { m_vColor = vColor; }
#endif // _DEBUG

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(class CBounding* pBounding, void* pArg);
	virtual void	Tick(_float4x4 TransformMatrix);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

public:
	// 선충돌 : 선 시작위치, 선 방향, 길이 반환
	_bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist);
	_bool Intersects(CCollider* pOtherCollider, _float3* pCollisionBox);
	void OnCollision(COLLISIONDESC::COLTYPE eCollisionType, CCollider* pOtherCollider);
	_bool IsCollision(CCollider* pOtherCollider);
	// collision vector에 현재 충돌중이 아닌데 값이 있을경우 삭제하기 위한 함수.
	void IsExitCollision();

private:
	class CBounding*			m_pBounding = { nullptr };
	TYPE						m_eColliderType = { TYPE_END };

private:
	// first : 틱마다 충돌이 있는지 확인
	// second : 충돌체 정보
	vector<pair<_bool, COLLISIONDESC>>		m_Collisions;

#ifdef _DEBUG
private:
	_float4						m_vColor;
#endif // _DEBUG

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END