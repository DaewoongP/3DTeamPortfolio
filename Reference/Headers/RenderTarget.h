#pragma once
/* =============================================== */
// 
//	정 : 박정환
//	부 : 박대웅
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() { return m_pRTV; }
	ID3D11DepthStencilView* Get_DSV() { return m_pShadowMapDSV; }
	ID3D11Texture2D*		Get_Texture2D() { return m_pTexture2D; }
	D3D11_VIEWPORT			Get_ViewPort() { return m_ShadowMapViewPortDesc; }

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor,_bool isShadow);
	HRESULT Initialize_Depth(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor);

	HRESULT Clear(_bool isShadow = false);
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	ID3D11Texture2D*			m_pTexture2D = { nullptr };
	ID3D11Texture2D*			m_pShadowMapTexture2D = { nullptr };
	
	ID3D11RenderTargetView*		m_pRTV = { nullptr };
	ID3D11ShaderResourceView*	m_pSRV = { nullptr };
	ID3D11DepthStencilView*		m_pShadowMapDSV = { nullptr };
	D3D11_VIEWPORT				m_ShadowMapViewPortDesc;

	_float4						m_vClearColor;

#ifdef _DEBUG
private:
	_float4x4					m_WorldMatrix;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor,_bool isShdow = false);
	virtual void Free() override;
};

END