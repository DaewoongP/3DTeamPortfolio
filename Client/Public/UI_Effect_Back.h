#pragma once
#include "UI.h"
#include "UI_Image.h"
#include "UI_Group_SKillTap.h"
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

class CUI_Effect_Back final : public CUI
{
public:
	enum EFFECTTYPE
	{
		CURSOR,
		FILEDGUIDE,
		TEXT,
		ALPHA,
		MAINSKILL,
		INTERACT,
		SKILLBACK,
		EFFECTTYPE_END
	};

public:
	void Set_Effecttype(EFFECTTYPE eType);
	void Set_ImageComShader(CUI_Image::SHADERTYPE eType) {
		m_pImageCom->Set_ShaderType(eType);
	}
	void Set_Clicked(_bool isClicked = false);
	void Set_Texture(CTexture* pTexture);
	void Set_ImageCom(CUI_Image::IMAGEDESC desc, _bool isChild = false);
	_bool Switch_ImageShow() {
		return m_pImageCom->SwitchShow();
	}
	void Set_Rotation(_float3 vAxis, _float fRadian);
	void	 Set_CoolTime(_float* pCool);
	void	 Set_isCool(_bool isCool) {
		m_isCool = isCool;
		if (nullptr != m_pImageCom)
			m_pImageCom->Set_isCool(isCool);
	}
	void	Set_SkillBackType(CUI_Group_SkillTap::SKILLTAP eType) {
		m_eSkillType = eType;
	}


private:
	explicit CUI_Effect_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Effect_Back(const CUI_Effect_Back& rhs);
	virtual ~CUI_Effect_Back() = default;

public:
	_bool	Get_Clicked();
	_bool	Get_Pressing();
	_bool	Get_Collision();

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CUI_Image* m_pImageCom = { nullptr };
	CUI_Button* m_pButtonCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT Set_Texture(_uint iIndex, CTexture* pTexture);

private:
	EFFECTTYPE		m_eEffecttype = { EFFECTTYPE_END };

private:
	_float			m_fRadian = { 0.f };

private:
	_bool			m_isClicked = { false };
	_bool			m_isInteract = { false };
	_bool			m_isShowImageCom = { false };
private:
	_float* m_fCoolTime = { 0 };
	_bool			m_isCool = { false };

private: // for SkillBack
	CUI_Group_SkillTap::SKILLTAP	m_eSkillType = { CUI_Group_SkillTap::SKILLTAP_END };

public:
	static CUI_Effect_Back* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
