#pragma once

/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CLevel() = default;

public:
	_bool Is_Loading() const { return m_isLoadingLevel; }

public:
	virtual HRESULT Initialize();
	virtual void Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	_bool					m_isLoadingLevel = { false };

public:
	virtual void Free() override;
};

END