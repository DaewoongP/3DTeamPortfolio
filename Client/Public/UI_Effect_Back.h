#pragma once
#include "UI.h"
#include "Client_Defines.h"

#include "UI_Image.h"
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

private:
	explicit CUI_Effect_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Effect_Back(const CUI_Effect_Back& rhs);
	virtual ~CUI_Effect_Back() = default;

public:
	void Set_Texture(CTexture* pTexture);
	void Set_ImageCom(CUI_Image::IMAGEDESC desc);
	void Set_Rotation(_float3 vAxis, _float fRadian);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*			m_pShaderCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CUI_Image*			m_pImageCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CUI_Group_Skill::UISKILLDESC* m_SkillDesc;

public:
	static CUI_Effect_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
