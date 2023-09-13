#pragma once
#include "UI.h"
#include "Client_Defines.h"

#include "CUI_Image.h"
#include "UI_Group_Skill.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Effect_Back final : public CUI
{
private:
	enum TEXTURE { DIFFUSE, EFFECT, TEXTURE_END };

protected:
	explicit CUI_Effect_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Effect_Back(const CUI_Effect_Back& rhs);
	virtual ~CUI_Effect_Back() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom[TEXTURE_END] = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CTexture*			m_pAlphaTextureCom = { nullptr };

	CUI_Image*			m_pImageCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_AlphaTexture();
	HRESULT SetUp_ShaderResources();
	HRESULT Ready_Texture();

public:
	void Set_Texture(CTexture* pTexture);

	CUI_Group_Skill::UISKILLDESC* m_SkillDesc;
	HFILE						 m_hFile;

public:
	static CUI_Effect_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
