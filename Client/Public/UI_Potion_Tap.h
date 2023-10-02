#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "UI.h"
#include "Item.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_Back;
class CUI_Effect_Back;
class CUI_Font;

class CUI_Potion_Tap final : public CGameObject
{
private:
	explicit CUI_Potion_Tap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Potion_Tap(const CUI_Potion_Tap& rhs);
	virtual ~CUI_Potion_Tap() = default;

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
	HRESULT	Ready_DefaultTexture();

public:
	HRESULT	Set_ItemCount(vector<CItem*>& pItems);
	HRESULT	Set_CurTapItem();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	vector<CUI_Back*>				m_pIcons;
	vector<CUI_Effect_Back*>		m_pFrames;
	vector<CUI_Back*>				m_pCountBack;
	vector<CUI_Back*>				m_pItemBack;
	vector<CUI_Font*>				m_pFonts;

	CUI_Back*						m_pBack;

private:
	_bool						m_isOpen = { false };

public:
	static CUI_Potion_Tap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END