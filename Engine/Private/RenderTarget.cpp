#include "..\Public\RenderTarget.h"
#include "Shader.h"

#ifdef _DEBUG
#include "VIBuffer_Rect.h"
#endif // _DEBUG

CRenderTarget::CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor, _bool isShadow)
{
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = vClearColor;
	
	if (isShadow)
	{
		Initialize_Depth(iSizeX, iSizeY, eFormat, vClearColor);
	}

	return S_OK;
}

HRESULT CRenderTarget::Initialize_Depth(_uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor)
{
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	/* DepthStencil */
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	ZEROMEM(&DepthStencilViewDesc);
	DepthStencilViewDesc.Flags = 0;
	DepthStencilViewDesc.Format = TextureDesc.Format;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;

	/* ViewPort */
	m_ShadowMapViewPortDesc.TopLeftX = 0;
	m_ShadowMapViewPortDesc.TopLeftY = 0;
	m_ShadowMapViewPortDesc.Width = _float(iSizeX);
	m_ShadowMapViewPortDesc.Height = _float(iSizeY);
	m_ShadowMapViewPortDesc.MinDepth = 0.f;
	m_ShadowMapViewPortDesc.MaxDepth = 1.f;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pShadowMapTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(m_pShadowMapTexture2D, &DepthStencilViewDesc, &m_pShadowMapDSV)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	_uint		iNumViews = { 1 };
	D3D11_VIEWPORT		ViewportDesc;

	m_pContext->RSGetViewports(&iNumViews, &ViewportDesc);

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;

	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->Bind_ShaderResource("g_DebugTexture", m_pSRV)))
		return E_FAIL;

	if (FAILED(pShader->Begin("Debug")))
		return E_FAIL;

	return pVIBuffer->Render();
}
#endif // _DEBUG

HRESULT CRenderTarget::Clear(_bool isShadow)
{
	if (false == isShadow)
	{
		m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
	}
	else
	{
		m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
		m_pContext->ClearDepthStencilView(m_pShadowMapDSV, D3D11_CLEAR_DEPTH, 1.f, 0);
	}

	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResourceView(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pSRV)
		return E_FAIL;

	return pShader->Bind_ShaderResource(pConstantName, m_pSRV);
}

CRenderTarget* CRenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT eFormat, const _float4& vClearColor,_bool isShadow)
{
	CRenderTarget* pInstance = New CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, eFormat, vClearColor,isShadow)))
	{
		MSG_BOX("Failed to Created CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pTexture2D);
	Safe_Release(m_pShadowMapTexture2D);
	Safe_Release(m_pShadowMapDSV);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
