#pragma once
/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComposite abstract : public CComponent
{
protected:
	explicit CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CComposite(const CComposite& rhs);
	virtual ~CComposite() = default;

public:
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	HRESULT		Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg = nullptr);
	CComponent* Find_Component(const _tchar* pComponentTag);
	HRESULT		Delete_Component(const _tchar * pComponentTag);

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

public:
	virtual void Free() override;
};

END