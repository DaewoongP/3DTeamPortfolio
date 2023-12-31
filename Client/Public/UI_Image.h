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

class CUI_Image : public CGameObject
{
public:
	enum SHADERTYPE
	{
		MINIMAP,
		SKILL,
		CLICK,
		SHADERTYPE_END
	};

public:
	struct IMAGEDESC
	{
		IMAGEDESC()
		{

		}

		_float2			vCombinedXY = { 0.f, 0.f };
		_float			fX = { 0.f };
		_float			fY = { 0.f };
		_float			fZ = { 0.f };
		_float			fSizeX = { 0.f };
		_float			fSizeY = { 0.f };
		SHADERTYPE		eShadertype = SHADERTYPE_END;
	};

private:
	explicit CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI_Image(const CUI_Image& rhs);
	virtual ~CUI_Image() = default;

public:
	void		Set_CoolTime(_float* pCool) {
		m_pCoolTime = pCool;
	}
	void		Set_isCool(_bool isCool) {
		m_isCool = isCool;
	}
	void		Set_ShaderType(SHADERTYPE eType) {
		m_eShadertype = eType;
	}
	_bool		SwitchShow() {
		m_isShow = !m_isShow;
		return m_isShow;
	}
	void		Set_Size(_float2 vXY) {
		m_fSizeX = vXY.x;
		m_fSizeY = vXY.y;
		Change_Scale(m_fSizeX, m_fSizeY);
	}
	void		Set_Z(_float fZ) {
		m_fZ = fZ;
	}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	HRESULT	Set_Texture(CTexture* pTexture);
	HRESULT Set_Desc(IMAGEDESC desc, _bool isChild = false);

protected:
	HRESULT Change_Position(_float fX, _float fY);
	HRESULT Change_Scale(_float fX, _float fY);

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CRenderer*		m_pRendererCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4			m_ViewMatrix;
	_float4x4			m_ProjMatrix;
	vector<_tchar*>		m_wszTags;

private: 
	_float2			m_vCombinedXY = { 0.f, 0.f };
	_float			m_fX = { 0.f };
	_float			m_fY = { 0.f };
	_float			m_fZ = { 0.f };
	_float			m_fSizeX = { 0.f };
	_float			m_fSizeY = { 0.f };
	SHADERTYPE		m_eShadertype = { SHADERTYPE_END };

private:
	CUI* m_pParent = { nullptr };
	
	_float3 vPlayerPos = _float3(1024.f, 0.f, 1024.f);
	_float2 vMiniMapSize = _float2(1024.f, 1024.f);

private: // for skill
	_float* m_pCoolTime = { nullptr };
	_bool	m_isCool = { false };
	
	
private:
	_bool m_isShow = { false };


public:
	static CUI_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Image* Clone(void* pArg);
	virtual void Free() override;
};

END