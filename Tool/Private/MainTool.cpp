#include "MainTool.h"
#include "GameInstance.h"
#include "Level_Tool.h"
#include "Dummy.h"
#include "MapDummy.h"
#include "MapObject.h"
#include "MapObject_Ins.h"
#include "LightDot.h"
#include "Camera_Point.h"
#include "DummyMeshEffect.h"
#include "DummyTrail.h"
#include "DummyFlipBook.h"
#include "Dummy_Effect.h"
#include "TriangleColMesh.h"

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

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Renderer)");
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);
	m_pRenderer->Get_Dof()->Off();

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

	/* For.Prototype_Component_Shader_VtxMesh */
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

	/* For.Prototype_Component_Shader_MeshInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"),
			VTXMESHINSTANCE_DECL::Elements, VTXMESHINSTANCE_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxMeshInstance)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Navigation"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"),
			VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Line */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Line"),
		CVIBuffer_Line::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Line)");
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Triangle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Triangle"),
		CVIBuffer_Triangle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_GeoSphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_GeoSphere"),
		CVIBuffer_GeoSphere::Create(m_pDevice, m_pContext))))
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

	/* Prototype_Component_Texture_T_Default_Material_Grid_M*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_T_Default_Material_Grid_M"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/T_Default_Material_Grid_M.png")))))
		return E_FAIL;

	/* Prototype_Component_Texture_Ground */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Ground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Texture/Terrain/Ground/Ground%d.dds"), 4))))
		return E_FAIL;

	/* Prototype_Component_Sphere_Collider*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider"),
		CCollider::Create(m_pDevice, m_pContext,CCollider::TYPE_SPHERE))))
		return E_FAIL;
	
	/* Prototype_Component_RigidBody*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* Prototype_Component_Model_SM_SpherePrimitiveRegularNormals_01*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL
		, TEXT("Prototype_Component_Model_SM_SpherePrimitiveRegularNormals_01")
		, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM
		, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat")))))
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
		CDummyParticle::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Test/")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyMeshEffect"),
		CDummyMeshEffect::Create(m_pDevice, m_pContext, TEXT(""), LEVEL_TOOL))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyTrail"),
		CDummyTrail::Create(m_pDevice, m_pContext, TEXT(""), LEVEL_TOOL))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyFlipbook"),
		CDummyFlipBook::Create(m_pDevice, m_pContext, LEVEL_TOOL, nullptr))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_DummyEffect"),
		CDummy_Effect::Create(m_pDevice, m_pContext))))
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

	/* Prototype_GameObject_MapObject_Ins*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_MapObject_Ins"),
		CMapObject_Ins::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	/* Prototype_GameObject_LightDot*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_LightDot"),
		CLightDot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Camera_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Camera_Point"),
		CCamera_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_Camera_Point*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Shape_GeoSphere"),
		CShape_GeoSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Prototype_GameObject_TriangleColMesh*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_TriangleColMesh"),
		CTriangleColMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
#pragma region Load Particle
	{
		std::lock_guard<std::mutex> lock(mtx);

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust01"),
			TEXT("../../Resources/GameData/ParticleData/Misc/Dust01/"), 3)))
			throw TEXT("Reserve Particle : Particle_Dust01");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust02"),
			TEXT("../../Resources/GameData/ParticleData/Misc/Dust02/"), 3)))
			throw TEXT("Reserve Particle : Particle_Dust02");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_RockChunksRough"),
			TEXT("../../Resources/GameData/ParticleData/Misc/RockChunksRough/"), 3)))
			throw TEXT("Reserve Particle : Particle_RockChunksRough");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Dash_Gas_Splash"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Dash/Gas_Splash/"), 3)))
			throw TEXT("Reserve Particle : Particle_Golem_Dash_Gas_Splash");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Dash_Gas_Twinkle"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Dash/Gas_Twinkle/"), 3)))
			throw TEXT("Reserve Particle : Particle_Golem_Dash_Gas_Twinkle");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Hit"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Hit/"), 3)))
			throw TEXT("Reserve Particle : Particle_Golem_Hit");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Slash_Trace_Twinkle"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Slash/Trace_Twinkle/"), 3)))
			throw TEXT("Reserve Particle : Particle_Golem_Slash_Trace_Twinkle");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Hammer_Trace_Rock"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Hammer_Trace_Rock/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Hammer_Trace_Rock");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Hand_Black"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Hand_Black/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Hand_Black");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Dust"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Dust/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Dust");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Stone"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Stone/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Stone");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Dust_Mini"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Dust_Mini/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Dust_Mini");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Stone_Mini"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Stone_Mini/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Stone_Mini");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_StoneRain"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_StoneRain/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_TwoHand_StoneRain");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Foot_Circle_Dust"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Foot_Circle_Dust/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Foot_Circle_Dust");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Landing_Dust"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Landing_Dust/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Landing_Dust");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Toe_Landing_Dust"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Toe_Landing_Dust/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Toe_Landing_Dust");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Weapon_Trace"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Weapon_Trace/"), 3)))
			throw TEXT("Reserve Particle : Particle_Troll_Weapon_Trace");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DugBog_Chim"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DugBog_Chim/"), 3)))
			throw TEXT("Reserve Particle : Particle_DugBog_Chim");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Trace_Rock"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Trace_Rock/"), 3)))
			throw TEXT("Reserve Particle : Particle_DogBog_Trace_Rock");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Chim_Red"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Chim_Red/"), 3)))
			throw TEXT("Reserve Particle : Particle_DogBog_Chim_Red");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Chim_Blue"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Chim_Blue/"), 3)))
			throw TEXT("Reserve Particle : Particle_DogBog_Chim_Blue");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Trace_Water"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Trace_Water/"), 3)))
			throw TEXT("Reserve Particle : Particle_DogBog_Trace_Water");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Water_Foot"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Water_Foot/"), 3)))
			throw TEXT("Reserve Particle : Particle_DogBog_Water_Foot");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Circle"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Circle/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Hit_Circle");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Distotion"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Distotion/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Hit_Distotion");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Spread"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Spread/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Hit_Spread");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Stick_Glow"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Stick_Glow/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Hit_Stick_Glow");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Black_Water"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Black_Water/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Black_Water");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Bubble"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Bubble/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Bubble");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Main"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Main/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Main");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Splash_01"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Splash_01/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Splash_01");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Splash_02"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Splash_02/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Splash_02");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Flare"),
			TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Flare/"), 3)))
			throw TEXT("Reserve Particle : Particle_Pensive_Appear_Flare");

		if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Conjured_Step_Dust"),
			TEXT("../../Resources/GameData/ParticleData/BoneDragon/StepDust/"), 3)))
			throw TEXT("Reserve Particle : Particle_Conjured_Step_Dust");
	}
#pragma endregion

	/*For.Player_BlinkEffect*/
	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Effect"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFog"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Effect");

	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Effect1"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFog1"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Effect1");

	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Effect2"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFog2"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Effect2");


	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Disotrtion"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkDistortion"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Disotrtion");

	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_SubEffect"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFoot"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_SubEffect");

	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Particle"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkParticle"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Particle");

	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Ring"),
		TEXT("../../Resources/GameData/ParticleData/Blink/BlinkRing"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Ring");

	/*For.Player_Hit_Particle*/
	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Hit_Dust"),
		TEXT("../../Resources/GameData/ParticleData/PlayerHit/HitDust"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Player_Hit_Dust");

	/*For.Player_BasicCast_Particle*/
	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_WandLine"),
		TEXT("../../Resources/GameData/ParticleData/BasicCast/WandLine"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_WandLine");
	/*For.Player_DiffindoLine_Particle*/
	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_DiffindoLine"),
		TEXT("../../Resources/GameData/ParticleData/Diffindo/Cast_Line"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_DiffindoLine");
	/*For.Player_Crusio_Particle*/
	if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Finisher_WandLine"),
		TEXT("../../Resources/GameData/ParticleData/Lightning/WandLine"))))
		throw TEXT("Reserve Particle : Prototype_GameObject_Finisher_WandLine");


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
