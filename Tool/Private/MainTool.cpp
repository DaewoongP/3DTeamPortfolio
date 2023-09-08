#include "MainTool.h"
#include "GameInstance.h"
#include "Level_Tool.h"
#include "Dummy.h"
#include "MapDummy.h"
#include "MapObject.h"
#include "Camera_Point.h"

#ifdef _DEBUG

#include "Camera_Line.h"

#endif

CMainTool::CMainTool()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pWindow_Manager(CWindow_Manager::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pWindow_Manager);

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

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Object()))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(m_pWindow_Manager->Initialize(m_pDevice, m_pContext)))
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

	m_pWindow_Manager->Tick(fTimeDelta);

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

	if (FAILED(m_pWindow_Manager->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Level()))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), m_szFPS, _float2(0.f, 680.f))))
		return E_FAIL;
#endif // _DEBUG

	if (FAILED(Render_ImGui()))
		return E_FAIL;

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
	io.Fonts->AddFontFromFileTTF("../../Resources/Fonts/NEXONLv1GothicBold.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesKorean());

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

HRESULT CMainTool::Render_ImGui()
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

HRESULT CMainTool::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	
	/*m_pGameInstance->Add_Prototype_Textures(LEVEL_TOOL, m_pDevice, m_pContext
		, TEXT("Prototype_Component_Texture"), TEXT(".png"), TEXT("../../Resources/UI/UI/Game/UI/Common"));

	m_pGameInstance->Add_Prototype_Models(LEVEL_TOOL, m_pDevice, m_pContext, CModel::TYPE_NONANIM
		, TEXT("Prototype_Component_Model"), TEXT(".dat"), TEXT("../../Resources/Models/NonAnims/"));*/

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Renderer)");
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Shader_Debug */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Debug"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"),
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"),
			VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_DefaultEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_DefaultEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"),
			VTXPOSCUBE_DECL::Elements, VTXPOSCUBE_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointColInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointColInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointColInstance.hlsl"),
			VTXPOINTCOLORINSTANCE_DECL::Elements, VTXPOINTCOLORINSTANCE_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxRectColInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxRectColInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectColInstance.hlsl"),
			VTXRECTCOLORINSTANCE_DECL::Elements, VTXRECTCOLORINSTANCE_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), 
			VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxAnimMesh)");
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Line)");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 513, 513))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Point_Color_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point_Color_Instance"),
		CVIBuffer_Point_Color_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect_Color_Instance*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance"),
		CVIBuffer_Rect_Color_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* Prototype_Component_Texture_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	/* Prototype_Component_Texture_Burger_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Burger_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Default0.jpg")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Default_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Default_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Particle.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Default_Particle*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_T_Default_Material_Grid_M"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/T_Default_Material_Grid_M.png")))))
		return E_FAIL;

	/* Prototype_Component_Sphere_Collider*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider"),
		CCollider::Create(m_pDevice, m_pContext,CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* Prototype_Component_Model_SM_SpherePrimitiveRegularNormals_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL
		, TEXT("Prototype_Component_Model_SM_SpherePrimitiveRegularNormals_01")
		, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM
		, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat")))))
		return E_FAIL;

	/* Prototype_Component_Default_ParticleSystem*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Default_ParticleSystem"),
		CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Particles/Fire/")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Default_MeshEffect"),
		CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Particles/Fire/")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainTool::Ready_Prototype_Object()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyParticle"),
		CDummyParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Dummy*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy"),
		CDummy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_MapDummy*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_MapDummy"),
		CMapDummy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_MapObject*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	/* Prototype_GameObject_Camera_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera_Point"),
		CCamera_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#ifdef _DEBUG

	/* Prototype_GameObject_Camera_Line*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera_Line"),
		CCamera_Line::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#endif
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

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pImGuiRTV);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pWindow_Manager);

	CWindow_Manager::DestroyInstance();

	CGameInstance::Release_Engine();
}
