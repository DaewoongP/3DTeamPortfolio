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

void CImGui_Manager::NextWindow_LeftBottom()
{
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 500.f), _float(Top) };
	ImGui::SetNextWindowPos(vWinpos);
}

void CImGui_Manager::MatrixImgui(_float4x4& pMatrix, const _char* pNodeName)
{
	if (nullptr == pNodeName)
		return;

	ImGui::Begin(pNodeName);
	// 인자를 통해서 고유한 태그를 만들어줌.
	string strPositionTag = "Position";
	string strRotationTag = "Rotation";
	string strScaleTag = "Scale";
	string strPositionName = strPositionTag + "##" + pNodeName;
	string strRotationName = strRotationTag + "##" + pNodeName;
	string strScaleName = strScaleTag + "##" + pNodeName;

	// 인자로 들어온 매트릭스로부터 이동, 거리, 크기를 만듬.
	_float3 vTranslations = pMatrix.Translation();
	_float3 vAngles = pMatrix.ToEuler();
	_float3 vScales = _float3(pMatrix.Right().Length(), pMatrix.Up().Length(), pMatrix.Look().Length());

	vAngles.x = XMConvertToDegrees(vAngles.x);
	vAngles.y = XMConvertToDegrees(vAngles.y);
	vAngles.z = XMConvertToDegrees(vAngles.z);

	// 좌우가 반전 되어 있었는지 확인.
	_float3 vXAxis = pMatrix.Right();
	_float3 vYAxis = pMatrix.Up();
	_float3 vZAxis = pMatrix.Look();
	if (vXAxis.x < 0.f)
	{
		vScales.x = -vScales.x;
	}
	if (vYAxis.y < 0.f)
	{
		vScales.y = -vScales.y;
	}
	if (vZAxis.z < 0.f)
	{
		vScales.z = -vScales.z;
	}

	// 위젯에 값 대입
	ImGui::Text("Position"); ImGui::SameLine();
	ImGui::DragFloat3(strPositionName.data(), reinterpret_cast<_float*>(&vTranslations), 0.01f);
	ImGui::Text("Rotation"); ImGui::SameLine();
	ImGui::DragFloat3(strRotationName.data(), reinterpret_cast<_float*>(&vAngles), 0.01f);
	ImGui::Text("   Scale"); ImGui::SameLine();
	ImGui::DragFloat3(strScaleName.data(), reinterpret_cast<_float*>(&vScales), 0.01f);

	// 회전행렬을 만들기 위한 라디안 변환
	vAngles.x = XMConvertToRadians(vAngles.x);
	vAngles.y = XMConvertToRadians(vAngles.y);
	vAngles.z = XMConvertToRadians(vAngles.z);

	// SRT연산
	_float4x4 ScaleMatrix = _float4x4::MatrixScale(vScales);
	_float4x4 RotMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(vAngles.x, vAngles.y, vAngles.z));
	_float4x4 TransMatrix = _float4x4::MatrixTranslation(vTranslations);

	// 새로운 월드행렬 만듬.
	pMatrix = ScaleMatrix * RotMatrix * TransMatrix;

	ImGui::End();
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