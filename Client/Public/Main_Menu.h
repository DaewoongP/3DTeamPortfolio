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
class CUI_Effect_Back;
class CUI_Group_Cursor;
class CMenu_Gear;
class CMenu_Inventory;
class CMenu_Quest;
class CMenu_Setting;

class CMain_Menu final : public CGameObject
{
private:
	explicit CMain_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMain_Menu(const CMain_Menu& rhs);
	virtual ~CMain_Menu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void 	Set_Menu(_uint iIndex);

private:
	HRESULT Add_Prototype();
	HRESULT Add_Components();
	HRESULT Add_TextComponent(wstring wszTag);

	HRESULT Add_Cursor();

private:
	HRESULT Read_File(const _tchar* pFilePath);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pTexture = { nullptr };

private: // Main Frame
	vector <CUI_Effect_Back*>	m_pTexts;
	_int						m_iSelectedText = -1;
	MENU						m_eCurMenu = MENU_END;

private: // Main Group
	CMenu_Gear*			m_pGear = { nullptr };
	CMenu_Inventory*		m_pInventory = { nullptr };
	CMenu_Quest*			m_pQuest = { nullptr };
	CMenu_Setting*		m_pSetting = { nullptr };

private:
	CUI_Group_Cursor*		m_pCursor = { nullptr };

private:
	void		Set_SelectedText();
	HRESULT		Ready_Menus();

public: 
	static CMain_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END