#pragma once
#include "Composite.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CComposite
{
public:
	enum OBJ_EVENT { OBJ_NONE, OBJ_DEAD, OBJ_POOL, OBJ_END};

protected:
	explicit CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	const _tchar* Get_PrototypeTag() const { return m_pPrototypeTag; }
	const _tchar* Get_Tag() const { return m_pTag; }
	CTransform* Get_Transform() const { return m_pTransform; }
	const OBJ_EVENT   Get_ObjEvent() const { return m_eObjEvent; }
	void Set_Tag(const _tchar * pTag) { lstrcpy(m_pTag, pTag); }
	void Set_PrototypeTag(const _tchar * pPrototypeTag) { lstrcpy(m_pPrototypeTag, pPrototypeTag); }
	void Set_ObjEvent(OBJ_EVENT eObjEvent) { m_eObjEvent = eObjEvent; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) {}
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) {}
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth();

protected:
	CTransform*		m_pTransform = { nullptr };
	OBJ_EVENT		m_eObjEvent = { OBJ_NONE };

protected:
	_tchar			m_pPrototypeTag[MAX_STR] = TEXT("");
	_tchar			m_pTag[MAX_STR] = TEXT("");

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END