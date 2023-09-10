#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Progress final : public CUI
{
protected:
	explicit CUI_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Progress(const CUI_Progress& rhs);
	virtual ~CUI_Progress() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;



private:
	_float	m_MaxGuage = { 0.f };
	_float	m_MinGuage = { 0.f };
	_float	m_CurrentGuage = { 0.f };

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pAlphaTextureCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_AlphaTexture();
	HRESULT SetUp_ShaderResources();

public:
	static CUI_Progress* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
