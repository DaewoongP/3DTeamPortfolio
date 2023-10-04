#ifdef _DEBUG
#include "..\Public\ImGui_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

HRESULT CImGui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	ImGui_ImplDX11_Init(pDevice, pContext);

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

	FAILED_CHECK_RETURN(pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D), E_FAIL);

	FAILED_CHECK_RETURN(pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pImGuiRTV), E_FAIL);

	m_vImGuiClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	return S_OK;
}

HRESULT CImGui_Manager::Render()
{
	if (false == m_isRender)
	{
		m_Functions.clear();
		m_isRender = true;
		return S_OK;
	}
	else
		m_isRender = false;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto& Function : m_Functions)
	{
		Function();
	}

	m_Functions.clear();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 원래의 백버퍼를 다시 장치에 바인딩 해준다.
	FAILED_CHECK_RETURN(pGameInstance->Bind_BackBuffer(), E_FAIL);
	Safe_Release(pGameInstance);
	
	return S_OK;
}

HRESULT CImGui_Manager::Add_Function(function<void()> Function)
{
	m_Functions.push_back(Function);

	return S_OK;
}

void CImGui_Manager::Clear_Function()
{
	m_Functions.clear();
}

void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pImGuiRTV);
}
#endif // _DEBUG