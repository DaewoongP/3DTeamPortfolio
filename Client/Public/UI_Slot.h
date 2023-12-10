#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CUI_Button;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI_Group_Skill;
class CUI_Font;

class CUI_Slot final : public CUI
{
public:
	enum SLOT
	{
		BACK,
		ICON,
		FRAME,
		SLOT_END
	};

private:
	explicit CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Slot(const CUI_Slot& rhs);
	virtual ~CUI_Slot() = default;

public:
	void Set_Clicked(_bool isClicked = false);
	void Set_IconTexture(CTexture* pTexture);
	void Set_Font(wstring wstrText);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool	Get_Clicked();
	CTexture* Get_IconTexture() { 
		return m_Textures[ICON]; 
	}

private:
	CShader*		m_pShaderCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CUI_Button*		m_pButtonCom = { nullptr };
	CUI_Font*		m_pFont = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(_uint iIndex);

private:
	_bool			m_isClicked = { false };
	_bool			m_isInFont = { false };

private:
	_tchar m_wszTexturePath[MAX_PATH] = TEXT("");
	_tchar m_wszAlphaTexturePath[MAX_PATH] = TEXT("");

	_tchar m_wszFont[MAX_PATH] = TEXT("");

public:
	static CUI_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END