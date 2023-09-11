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

class CUI_Back final : public CUI
{
protected:
	explicit CUI_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Back(const CUI_Back& rhs);
	virtual ~CUI_Back() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pAlphaTextureCom = { nullptr };
	

private:
	HRESULT Add_Components();
	HRESULT Add_AlphaTexture();
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Texture();

public:
	static CUI_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
