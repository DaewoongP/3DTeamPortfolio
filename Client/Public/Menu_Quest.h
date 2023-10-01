#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_Back;
class CUI_Group_Quest;


class CMenu_Quest final : public CGameObject
{
private:
	explicit CMenu_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMenu_Quest(const CMenu_Quest& rhs);
	virtual ~CMenu_Quest() = default;

public:
	void		Set_Open(_bool isOpen);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Prototype();

private:
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Ready_Offset();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	vector<CUI_Back*>			m_pFonts;
	vector<_float2>			m_fOffset;
	vector<CUI*>				m_pUIs;

private:
	_bool		m_isOpen = { false };

public:
	static CMenu_Quest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END