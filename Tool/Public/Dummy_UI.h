#pragma once
#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Tool)

class CDummy_UI final : public CGameObject
{
public:
	enum UI_ID { NONE, BUTTON, PROGRESS, UI_ID_END };

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
	CDummy_UI*	Get_Parent() { return m_pParent; }
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

protected:
	_float2			m_vCombinedXY ={ 0.f, 0.f };
	// 윈도우창의 실제 x좌표
	_float			m_fX = { 650.f };
	// 윈도우창의 실제 y좌표
	_float			m_fY = { 360.f };
	// UI 거리 체크용 z값
	_float			m_fZ = { 0.f };
	// 텍스처 x사이즈 설정
	_float			m_fSizeX = { 100.f };
	// 텍스처 y사이즈 설정
	_float			m_fSizeY = { 100.f };

	_float4x4		m_ViewMatrix;
	_float4x4		m_ProjMatrix;

private:
	_tchar			m_wszTextureName[MAX_STR] = {};
	_tchar			m_wszTexturePath[MAX_STR] = {};

	UI_ID			m_eUIType = { UI_ID_END };
	_bool			m_isParent = { false };
	CDummy_UI*		m_pParent = { nullptr };

private:
	_bool			m_isAlpha = { false };
	_float4			m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_tchar			m_wszAlphaTexturePrototypeTag[MAX_PATH] = TEXT("");
	_tchar			m_wszAlphaTextureFilePath[MAX_PATH] = TEXT("");

public:
	_bool	Is_In_Rect();

public:
	_float2	UIPos_To_WorldPos(_float fX, _float fY);
	_float2 WorldPos_To_UIPos(_float fX, _float fY);

protected:
	HRESULT Change_Position(_float fX, _float fY);
	HRESULT Change_Scale(_float fX, _float fY);

public:
	static CDummy_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


