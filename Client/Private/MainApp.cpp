#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
#ifdef _DEBUG
	ZeroMemory(m_szFPS, sizeof(_tchar) * MAX_STR);
#endif // _DEBUG
}

HRESULT CMainApp::Initialize()
{
	srand((_uint)time(NULL));

	// 그래픽 디바이스를 만들기 위한 구조체 할당
	GRAPHICDESC		GraphicDesc;
	ZEROMEM(&GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;
	
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Initialize_ImGui()))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Object_For_Loading()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;
	
	if (FAILED(Open_Level(LEVEL_LOGO)))
	{
		MSG_BOX("Failed Open LEVEL_LOGO");
		return E_FAIL;
	}

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	// 엔진의 Tick 호출
	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	Tick_FPS(fTimeDelta);
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;
	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Level()))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), m_szFPS, _float2(0.f, 680.f))))
		return E_FAIL;

	if (FAILED(Render_ImGui()))
		return E_FAIL;
#endif // _DEBUG
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		/* Prototype_Component_Renderer */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Renderer");
		Safe_AddRef(m_pRenderer);

		/* Prototype_Component_Shader_VtxTex */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
				VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxTex");

		/* Prototype_Component_VIBuffer_Rect */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
			CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Rect");

		/* Prototype_Component_VIBuffer_Rect_Dynamic */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Dynamic"),
			CVIBuffer_Rect_Dynamic::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Rect_Dynamic");

		/* For.Prototype_Component_VIBuffer_Line */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Line"),
			CVIBuffer_Line::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Line");

		/* For.Prototype_Component_VIBuffer_Triangle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Triangle"),
			CVIBuffer_Triangle::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Triangle");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Ready_Prototype_Component_For_Static : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Object_For_Loading()
{

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_135"), TEXT("../../Resources/Default/Fonts/135ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

#ifdef _DEBUG
void CMainApp::Tick_FPS(_float fTimeDelta)
{
	m_fFpsTime += fTimeDelta;

	++m_iFps;

	if (1.f <= m_fFpsTime)
	{
		swprintf_s(m_szFPS, L"FPS: %d", m_iFps);
		m_iFps = 0;
		m_fFpsTime = 0.f;
	}
}
HRESULT CMainApp::Initialize_ImGui()
{
	// Show the window
	::ShowWindow(g_hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(g_hWnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.f;
	}

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iWinSizeX;
	TextureDesc.Height = g_iWinSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pImGuiRTV)))
		return E_FAIL;

	m_vImGuiClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	return S_OK;
}
void CMainApp::Tick_ImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
HRESULT CMainApp::Render_ImGui()
{
	// 여기서 내 백버퍼를 빼고 무슨 작업을 쳐놓고 그렸는데,
	// 내백버퍼를 다시 장치에 바인딩하기위해 렌더 
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	// 원래의 백버퍼를 다시 장치에 바인딩 해준다.
	if (FAILED(m_pGameInstance->Bind_BackBuffer()))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = New CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
#ifdef _DEBUG
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pImGuiRTV);
#endif // _DEBUG
	
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
