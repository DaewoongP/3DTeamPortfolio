#include "..\public\Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{
}

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND hWnd, GRAPHICDESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY, _Inout_ ID3D11Device** ppDeviceOut, _Inout_ ID3D11DeviceContext** ppDeviceContextOut)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	D3D_FEATURE_LEVEL			FeatureLV;

	FAILED_CHECK_RETURN_MSG(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, 
			&m_pDevice, &FeatureLV, &m_pDeviceContext), E_FAIL, L"Failed Create Device");

	if (FAILED(Ready_SwapChain(hWnd, eWinMode, iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	// ����Ÿ�� ����
	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV,
	};

	m_pDeviceContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

	// ����Ʈ ����ü �� ����
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));

	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWinCX;
	ViewPortDesc.Height = (_float)iWinCY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;
	
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	NULL_CHECK_RETURN_MSG(m_pDeviceContext, E_FAIL, TEXT("Device Context NULL"));
	
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	NULL_CHECK_RETURN_MSG(m_pDeviceContext, E_FAIL, TEXT("Device Context NULL"));

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	NULL_CHECK_RETURN_MSG(m_pSwapChain, E_FAIL, L"SwapChain NULL");

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Bind_BackBuffer()
{
	NULL_CHECK_RETURN_MSG(m_pDeviceContext, E_FAIL, TEXT("Device Context NULL"));
	NULL_CHECK_RETURN_MSG(m_pBackBufferRTV, E_FAIL, TEXT("BackBufferRTV NULL"));
	NULL_CHECK_RETURN_MSG(m_pDepthStencilView, E_FAIL, TEXT("DepthStencilView NULL"));

	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, GRAPHICDESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC	SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = static_cast<_bool>(eWinMode);
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	FAILED_CHECK_RETURN_MSG(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain), E_FAIL, L"Failed CreateSwapChain");

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	NULL_CHECK_RETURN_MSG(m_pDevice, E_FAIL, TEXT("Device NULL"));

	ID3D11Texture2D* pBackBufferTexture = nullptr;

	FAILED_CHECK_RETURN_MSG(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture), E_FAIL,
		L"Failed GetBuffer");

	FAILED_CHECK_RETURN_MSG(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV), E_FAIL,
		L"Failed Create RenderTarget");

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	NULL_CHECK_RETURN_MSG(m_pDevice, E_FAIL, TEXT("Device NULL"));

	ID3D11Texture2D* pDepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;

	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	FAILED_CHECK_RETURN_MSG(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture), E_FAIL, L"Failed Create Texture2D");
	FAILED_CHECK_RETURN_MSG(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView), E_FAIL,
		L"Failed CreateDepthStencilView");

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDepthStencilView);

	// �İ�ü ������ ����� �뵵.
	/*#if defined(DEBUG) || defined(_DEBUG)
		ID3D11Debug* d3dDebug;
		HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
		if (SUCCEEDED(hr))
		{
			OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
			OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
			OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	
			hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	
			OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
			OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
			OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		}
		if (d3dDebug != nullptr)            d3dDebug->Release();
	#endif*/

	// ������Ҷ� ����̽��� �ʿ��Ͽ� �Ʒ��ʿ��� ����ó��.
	Safe_Release(m_pDevice);
}
