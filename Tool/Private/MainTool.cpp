#include "..\Public\MainTool.h"
#include "GameInstance.h"
#include "Level_Tool.h"

CMainTool::CMainTool()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

#ifdef _DEBUG
	ZeroMemory(m_szFPS, sizeof(_tchar) * MAX_STR);
#endif // _DEBUG
}

HRESULT CMainTool::Initialize()
{
	// 그래픽 디바이스를 만들기 위한 구조체 할당
	GRAPHICDESC		GraphicDesc;
	ZEROMEM(&GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Initialize_ImGui()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_TOOL)))
	{
		MSG_BOX("Failed Open LEVEL_TOOL");
		return E_FAIL;
	}

	return S_OK;
}

void CMainTool::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;
	
	Tick_ImGui();

	// 엔진의 Tick 호출
	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef _DEBUG
	Tick_FPS(fTimeDelta);
#endif // _DEBUG
}

HRESULT CMainTool::Render()
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
#endif // _DEBUG
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Initialize_ImGui()
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
	
	return S_OK;
}

HRESULT CMainTool::Render_ImGui()
{


	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Renderer)");
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainTool::Ready_Fonts()
{
	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_135"), TEXT("../../Resources/Default/Fonts/135ex.spritefont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Open_Level(LEVELID eLevelIndex)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return m_pGameInstance->Open_Level(LEVEL_TOOL, CLevel_Tool::Create(m_pDevice, m_pContext));
}
#ifdef _DEBUG
void CMainTool::Tick_FPS(_float fTimeDelta)
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
#endif // _DEBUG

void CMainTool::Tick_ImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
}

CMainTool* CMainTool::Create()
{
	CMainTool* pInstance = new CMainTool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CMainTool");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainTool::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
