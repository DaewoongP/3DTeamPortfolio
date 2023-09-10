#pragma once
#include "UI.h"
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CDummy_UI final : public CUI
{
protected:
	explicit CDummy_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDummy_UI(const CDummy_UI& rhs);
	virtual ~CDummy_UI() = default;

public:
	_float2		Get_fXY() { return _float2(m_fX, m_fY); }
	_float		Get_fZ() { return m_fZ; }
	_float2		Get_fSize() { return _float2(m_fSizeX, m_fSizeY); }
	_float2		Get_vCombinedXY() { return m_vCombinedXY; }
	_bool		Get_bParent() { return m_isParent; }
	CUI*		Get_Parent() { return m_pParent; }
	_tchar*		Get_TextureName() { return m_wszTextureName; }
	_tchar*		Get_TexturePath() { return m_wszTexturePath; }
	UI_ID		Get_UI_ID() { return m_eUIType; }
	_float4		Get_vColor() { return m_vColor; }
	_bool		Get_bAlpha() { return m_isAlpha; }
	_tchar*		Get_AlphaTexturePath() { return m_wszAlphaTextureFilePath; }
	_tchar*		Get_AlphaPrototypeTag() { return m_wszAlphaTexturePrototypeTag; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Tick_Imgui();
public:
	void Set_fXY(_float fX, _float fY) 
	{ 
		m_fX = fX;
		m_fY = fY;
	}
	void Set_fZ(_float fZ) { m_fZ = fZ; }
	void Set_Size(_float fX, _float fY)
	{
		m_fSizeX = fX;
		m_fSizeY = fY;
	}
	
	void Set_bParent() { m_isParent = true; }
	void Set_Parent(CDummy_UI* pParent, _bool isSave = false) 
	{ 
		if (nullptr == m_pParent)
		{
			if (isSave)
			{
				m_pParent = pParent;
			}
			else
			{
				m_pParent = pParent;
				Set_fXY(0.f, 0.f);
			}
		}
	}
	void	Set_vColor(_float4 vColor) { m_vColor = vColor; }
	void	Set_bAlpha(_bool bAlpha) { m_isAlpha = bAlpha; }
	HRESULT	Set_AlphaTexture(_tchar* pFilePath);
	void	Set_AlphaTextureTag(_tchar* pTag)
	{
		lstrcpy(m_wszAlphaTexturePrototypeTag, pTag);
	}
	void	Set_AlphaTexturePath(_tchar* pPath)
	{
		lstrcpy(m_wszAlphaTextureFilePath, pPath);
	}
	void	Set_eUIID(UI_ID eType) { m_eUIType = eType; }


protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

	CTexture*			m_pAlphaTextureCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Add_AlphaTexture();
	HRESULT SetUp_ShaderResources();
	HRESULT	Ready_Texture();

public:
	static CDummy_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


