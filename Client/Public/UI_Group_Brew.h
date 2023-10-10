#pragma once

#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CUI_Back;
class CUI_Effect_Back;
class CItem;
class CUI_Group_Cursor;
class CInvnetory;

class CUI_Group_Brew final : public CGameObject
{
private:
	enum FONT
	{
		FIRST,
		SECOND,
		FONT_END
	};

private:
	enum BREWPOTION
	{
		WIGEN,
		EDURUS,
		MAXIMA,
		INVISIBLE,
		FOCUS,
		THUNDERBREW,
		BREW_END
	};

private:
	explicit CUI_Group_Brew(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Group_Brew(const CUI_Group_Brew& rhs);
	virtual ~CUI_Group_Brew() = default;

public:
	void		Set_isOpen(_bool isOpen) {
		m_isOpen = isOpen;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT	Add_Prototype();
	HRESULT	Ready_DefaultTexture();
	HRESULT	Ready_ExplainTexture();
	HRESULT Add_Fonts();

private:
	HRESULT Create_Component(const _tchar* pFIlePath, wstring wszTag, BREWPOTION eType);
	HRESULT Add_Components(wstring wszTag, BREWPOTION eType);
	HRESULT Read_File(const _tchar* pFilePath, BREWPOTION iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile, _bool isDDS = true);

private:
	vector<CUI_Effect_Back*>		m_pFrames;
	vector<CUI_Effect_Back*>		m_pIcons;

	CUI_Font*					m_pFirstFont;
	CUI_Font*					m_pSecondFont;

	CUI_Back*					m_pBack;
	CUI_Group_Cursor*				m_pCursor;

	vector<CTexture*>				m_pExplainTextures;

private:
	CInventory*					m_pInventory = { nullptr };
	_bool						m_isOpen = { false };

	BREWPOTION					m_eCurExplain = { BREW_END };

private:
	void			Set_Font();
	void			Set_Font_Text(INGREDIENT eType, FONT eFont);

public:
	static CUI_Group_Brew* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END