#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
public:
	enum OBJ_EVENT { OBJ_NONE, OBJ_DEAD, OBJ_POOL, OBJ_END };

protected:
	explicit CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	const OBJ_EVENT Get_ObjEvent() const { return m_eObjEvent; }
	class CComposite* Get_Owner() const { return m_pOwner; }
	const _tchar* Get_PrototypeTag() { return m_pPrototypeTag; }
	const _tchar* Get_Tag() const { return m_pTag; }
	void Set_ObjEvent(OBJ_EVENT eEvent) { m_eObjEvent = eEvent; }
	void Set_Owner(class CComposite* pOwner) { m_pOwner = pOwner; }
	void Set_PrototypeTag(const _tchar* pPrototypeTag) { lstrcpy(m_pPrototypeTag, pPrototypeTag); }
	void Set_Tag(const _tchar* pTag) { lstrcpy(m_pTag, pTag); }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

protected:
	class CComposite*			m_pOwner = { nullptr };
	_bool						m_isCloned = { false };
	OBJ_EVENT					m_eObjEvent = { OBJ_NONE };

private:
	_tchar						m_pPrototypeTag[MAX_PATH] = TEXT("");
	_tchar						m_pTag[MAX_STR] = TEXT("");

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END