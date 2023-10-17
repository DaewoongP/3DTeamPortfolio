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
class CUI_Group_Cursor;
class CMain_Menu;
class CField_Guide final : public CGameObject
{
private:
	explicit CField_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CField_Guide(const CField_Guide& rhs);
	virtual ~CField_Guide() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Prototype();
	//	HRESULT Add_Components(const _tchar* wszTag);
	HRESULT Add_Components(wstring wszTag);

	HRESULT Add_Cursor();

	HRESULT Read_File(const _tchar* pFilePath, MENU iIndex);
	CUI::UIDESC Load_File(const HANDLE hFile);

private:
	HRESULT Create_First(void* pArg);
	HRESULT Create_Component(const _tchar* pFIlePath, wstring wszTag, MENU eType);
	HRESULT Create_BackGround();

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pTexture = { nullptr };

private:
	vector<class CUI_Effect_Back*>	m_pFrames;
	vector<class CUI_Effect_Back*>	m_pMains;
	vector<class CUI_Back*>			m_pBacks;
	vector<class CUI_Back*>			m_pLines;

private:
	CMain_Menu* m_pMenu = { nullptr };

	CUI_Group_Cursor* m_pCursor = { nullptr };

private:
	CInventory* m_pPlayerInventory = { nullptr };

private:
	MENU	m_eMenu = { MENU_END };

private:
	void	Set_SelectedText();
	void	FieldGuide_To_Menu(_uint iIndex);

public:
	static CField_Guide* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END