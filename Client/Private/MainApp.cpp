#include "..\Public\MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "MagicBallPool.h"
#include "UI_Group_Loading.h"

#ifdef _DEBUG
#include "ImGui_Manager.h"
#endif

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
#ifdef _DEBUG
	, m_pImGui_Manager{ CImGui_Manager::GetInstance() }
#endif // _DEBUG
{
	Safe_AddRef(m_pGameInstance);
	ZeroMemory(m_szFPS, sizeof(_tchar)* MAX_STR);
#ifdef _DEBUG
	Safe_AddRef(m_pImGui_Manager);
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
	
	FAILED_CHECK_RETURN(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext), E_FAIL);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(m_pImGui_Manager->Initialize(m_pDevice, m_pContext), E_FAIL);
#endif // _DEBUG

	FAILED_CHECK_RETURN(Ready_Prototype_Component_For_Static(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Fonts(), E_FAIL);
	
	FAILED_CHECK_RETURN_MSG(Open_Level(LEVEL_LOGO), E_FAIL, TEXT("Failed Open LEVEL_LOGO"));
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

#ifdef _DEBUG
	ShowCursor(true);
#else
	ShowCursor(false);
#endif // _DEBUG

	// 엔진의 Tick 호출
	m_pGameInstance->Tick_Engine(fTimeDelta);

	Tick_FPS(fTimeDelta);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(m_pImGui_Manager->Add_Function([&] { this->Debug_ImGui(); }), );
	FAILED_CHECK_RETURN(m_pImGui_Manager->Render(), );
#endif // _DEBUG
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	std::lock_guard<std::mutex> lock(mtx);

	FAILED_CHECK_RETURN(m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f)), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Clear_DepthStencil_View(), E_FAIL);
	FAILED_CHECK_RETURN(m_pRenderer->Draw_RenderGroup(), E_FAIL);
	FAILED_CHECK_RETURN(m_pGameInstance->Render_Level(), E_FAIL);

#ifdef _DEBUG
	FAILED_CHECK_RETURN(m_pGameInstance->Render_Font(TEXT("Font_135"), m_szFPS, _float2(0.f, 680.f), _float4(0.f, 1.f, 0.f, 1.f)), E_FAIL);
#endif // _DEBUG

	FAILED_CHECK_RETURN(m_pGameInstance->Present(), E_FAIL);
	
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

		/* For.Prototype_Component_Shader_VtxPointColInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointColInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointColInstance.hlsl"),
				VTXPOINTCOLORINSTANCE_DECL::Elements, VTXPOINTCOLORINSTANCE_DECL::iNumElements))))
			return E_FAIL;

		/* For.Prototype_Component_Shader_VtxRectColInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxRectColInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectColInstance.hlsl"),
				VTXRECTCOLORINSTANCE_DECL::Elements, VTXRECTCOLORINSTANCE_DECL::iNumElements))))
			return E_FAIL;

		/* Prototype_Component_VIBuffer_Point_Color_Instance*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Color_Instance"),
			CVIBuffer_Point_Color_Instance::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Point_Color_Instance");

		/* Prototype_Component_VIBuffer_Rect_Color_Instance*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance"),
			CVIBuffer_Rect_Color_Instance::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Rect_Color_Instance");

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
		
		/* For.Prototype_Component_VIBuffer_GgoSphere*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_GeoSphere"),
			CVIBuffer_GeoSphere::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_GeoSphere");

		/* For.Prototype_Component_VIBuffer_GgoSphere*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_UVSphere"),
			CVIBuffer_UVSphere::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_UVSphere");

		/* Prototype_Component_Sphere_Collider*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sphere_Collider"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
			throw TEXT("Prototype_Component_Sphere_Collider");

		/* Prototype_Component_AABB_Collider*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_AABB_Collider"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
			throw TEXT("Prototype_Component_AABB_Collider");

		/* Prototype_Component_OBB_Collider*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_OBB_Collider"),
			CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
			throw TEXT("Prototype_Component_OBB_Collider");

		/* Prototype_Component_CoolTime*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
			CCoolTime::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_CoolTime");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Loading"),
			CUI_Group_Loading::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Loading");
			
		/* Prototype_Component_Defence*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Defence"),
			CDefence::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Defence");

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Ready_Prototype_Component_For_Static : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_135"), TEXT("../../Resources/Default/Fonts/135ex.spritefont")), E_FAIL);

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelIndex)
{
	NULL_CHECK_RETURN(m_pGameInstance, E_FAIL);

	return m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelIndex));
}

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

	//SetWindowText(g_hWnd, m_szFPS);
}

#ifdef _DEBUG

void CMainApp::Debug_ImGui()
{
	RECT rc;
	ZEROMEM(&rc);
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(300.f, 200.f));

	ImGui::Begin("Main Debug");

	ImGui::Text("Change Level");
	_bool isChangedLevel = { false };
	m_eLevelID = (LEVELID)m_pGameInstance->Get_CurrentLevelIndex();
	if (LEVEL_END == m_eLevelID ||
		LEVEL_LOADING == m_eLevelID ||
		LEVEL_STATIC == m_eLevelID)
	{
		ImGui::End();
		return;
	}
	if (ImGui::RadioButton("LEVEL_LOGO", (_int*)(&m_eLevelID), LEVEL_LOGO))
		isChangedLevel = true;
	if (ImGui::RadioButton("LEVEL_CLIFFSIDE", (_int*)(&m_eLevelID), LEVEL_CLIFFSIDE))
		isChangedLevel = true;
	if (ImGui::RadioButton("LEVEL_VAULT", (_int*)(&m_eLevelID), LEVEL_VAULT))
		isChangedLevel = true;
	if (ImGui::RadioButton("LEVEL_SMITH", (_int*)(&m_eLevelID), LEVEL_SMITH))
		isChangedLevel = true;

	if (true == isChangedLevel)
	{
		m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, m_eLevelID, m_isStaticLoaded));

		if (false == m_isStaticLoaded)
		{
			m_isStaticLoaded = true;
		}
	}

	if (LEVEL_LOGO != m_eLevelID)
		m_isStaticLoaded = true;

	ImGui::End();
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
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

#ifdef _DEBUG
	Safe_Release(m_pImGui_Manager);
	CImGui_Manager::GetInstance()->DestroyInstance();
#endif // _DEBUG

	CMagicBallPool::GetInstance()->DestroyInstance();
	CGameInstance::Release_Engine();
}
