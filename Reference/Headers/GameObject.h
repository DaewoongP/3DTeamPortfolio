#pragma once

/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	CTransform* Get_Transform() const { return m_pTransform; }
	void* Get_CollisionData() { return m_pCollisionArg; }
	void Set_CollisionData(void* pArg) { m_pCollisionArg = pArg; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLEVENTDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLEVENTDESC CollisionDesc) {}
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth();

protected:
	CTransform*		m_pTransform = { nullptr };

private:
	void*		m_pCollisionArg = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END