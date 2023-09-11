#pragma once
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CUI_Progress;
END

BEGIN(Client)

class CUI_Finisher final : public CUI
{
private:
	enum TEXTURETYPE
	{
		DIFFUSE,
		NOISE,
		TEXTURETYPE_END
	};

protected:
	explicit CUI_Finisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Finisher(const CUI_Finisher& rhs);
	virtual ~CUI_Finisher() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Gauge(_float fGauge, CUI_Progress::GAUGE eType);

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom[TEXTURETYPE_END] = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pAlphaTextureCom = { nullptr };
	CUI_Progress* m_pProgressCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_AlphaTexture();
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Texture();


public:
	static CUI_Finisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
