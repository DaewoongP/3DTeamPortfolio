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
protected:
	explicit CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	class CComposite* Get_Owner() const { return m_pOwner; }
	const _tchar* Get_PrototypeTag() { return m_pPrototypeTag; }
	void Set_PrototypeTag(const _tchar* pPrototypeTag) { lstrcpy(m_pPrototypeTag, pPrototypeTag); }
	void Set_Owner(class CComposite* pOwner) { m_pOwner = pOwner; }

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

private:
	_tchar						m_pPrototypeTag[MAX_PATH] = TEXT("");

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

END