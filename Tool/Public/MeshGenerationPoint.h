#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CTransform;
class CRigidBody;
END

BEGIN(Tool)

class CMeshGenerationPoint final : public CGameObject
{
private:
	explicit CMeshGenerationPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMeshGenerationPoint(const CMeshGenerationPoint& rhs);
	virtual ~CMeshGenerationPoint() = default;

public:
	_float4 Get_Position();
	void Set_Position(_float4 _vPosition);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual HRESULT Render() override;

public:
	_bool RayIntersects(_float4 vOrigin, _float4 vDirection, _Inout_ _float& fDist);
	void Set_BoundingDesc(void* pBoundingDesc);

#ifdef _DEBUG
	void Set_Collider_Color(_float4 _vColor);
#endif // _DEBUG
private:
	CRenderer* m_pRenderer = { nullptr };
	CCollider* m_pCollider = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	PxScene* m_pScene = { nullptr };

	_bool	m_isLand = { false };
private:
	HRESULT Add_Components();

public:
	static CMeshGenerationPoint* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

END
