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

	// �׷��� ����̽��� ����� ���� ����ü �Ҵ�
	GRAPHICDESC		GraphicDesc;
	ZEROMEM(&GraphicDesc);

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWinMode = GRAPHICDESC::WINMODE::WM_WIN;
	
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

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

	// ������ Tick ȣ��
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
#endif // _DEBUG
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Renderer)");
		return E_FAIL;
	}
	Safe_AddRef(m_pRenderer);

	/* Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"),
			VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_Shader_VtxTex)");
		return E_FAIL;
	}

	/* Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_VIBuffer_Rect)");
		return E_FAIL;
	}
	/* Prototype_Component_VIBuffer_Rect_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_Dynamic"),
		CVIBuffer_Rect_Dynamic::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_VIBuffer_Rect_Dynamic)");
		return E_FAIL;
	}

	/* Prototype_Component_PX */
	/*if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_PX"),
		CPX::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype  : (Prototype_Component_PX)");
		return E_FAIL;
	}*/

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
#endif // _DEBUG

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

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

	CGameInstance::Release_Engine();
}
