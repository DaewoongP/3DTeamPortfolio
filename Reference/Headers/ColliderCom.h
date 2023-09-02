#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CColliderCom final : public CComponent
{
private:
	explicit CColliderCom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CColliderCom(const CColliderCom& rhs);
	virtual ~CColliderCom() = default;

public:
	PxShape* Get_Shape() const { return m_pShape; }
	void Set_Shape(PxShape* pShape);
	void Set_Trigger(_bool isTrigger);
	_bool Is_Trigger() const { return m_isTrigger; }
	
public:
	virtual HRESULT Initialize(void* pArg) override;

private:
	PxGeometry*					m_pGeometry = { nullptr };
	const PxMaterial*			m_pMaterial = { nullptr };
	const PxTransform			m_LocalTransform;
	PxShape*					m_pShape = { nullptr };

private:
	_bool						m_isTrigger = { false };

private:
	void Invalidate_TriggerState() const;

public:
	static CColliderCom* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END