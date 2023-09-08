#include "..\Public\Window_Manager.h"
#include "ImWindow.h"

IMPLEMENT_SINGLETON(CWindow_Manager)

HRESULT CWindow_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = pContext;
	Safe_AddRef(m_pContext);
	return S_OK;
}

void CWindow_Manager::Tick(_float fTimeDelta)
{
	RECT rc;
	ZEROMEM(&rc);
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(200.f, 200.f));

	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	_int* iCurMenuList = reinterpret_cast<_int*>(&m_eCurMenuList);

	if (ImGui::RadioButton("Object", iCurMenuList, OBJECT_WINDOW))
	{
		if (0 == (m_eLoadingFlag & OBJECT_LOAD))
		{
			if (FAILED(Add_Window(TEXT("Object_Window"),
				CObject_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(g_iWinSizeX), _float(0.f)), ImVec2(400.f, 700.f)))))
				MSG_BOX("Failed Create Object_Window");
		}

		Setup_Current_Window(TEXT("Object_Window"));
		m_eLoadingFlag |= OBJECT_LOAD;
	}

	if (ImGui::RadioButton("NaviMesh", iCurMenuList, NAVIGATION_WINDOW))
	{
		if (0 == (m_eLoadingFlag & NAVIGATION_LOAD))
		{
		}

		Setup_Current_Window(TEXT("NaviMesh_Window"));
		m_eLoadingFlag |= NAVIGATION_LOAD;
	}

	if (ImGui::RadioButton("Camera", iCurMenuList, CAMERA_WINDOW))
	{
		if (0 == (m_eLoadingFlag & CAMERA_LOAD))
		{
			if (FAILED(Add_Window(TEXT("Camera_Window"),
				CCamera_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(g_iWinSizeX), _float(0.f)), ImVec2(446.f, 768.f)))))
				MSG_BOX("Failed Create Camera_Window");
		}

		Setup_Current_Window(TEXT("Camera_Window"));
		m_eLoadingFlag |= CAMERA_LOAD;
	}
		

	if (ImGui::RadioButton("Effect", iCurMenuList, EFFECT_WINDOW))
	{
		if (0 == (m_eLoadingFlag & EFFECT_LOAD))
		{
			if (FAILED(Add_Window(TEXT("Effect_Window"),
				CEffect_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(g_iWinSizeX), _float(0.f)), ImVec2(446.f, 768.f)))))
				MSG_BOX("Failed Create Object_Window");
		}

		Setup_Current_Window(TEXT("Effect_Window"));
		m_eLoadingFlag |= EFFECT_LOAD;
	}
		

	if (ImGui::RadioButton("Animation", iCurMenuList, ANIMATION_WINDOW))
	{
		if (0 == (m_eLoadingFlag & ANIAMTION_LOAD))
		{
			if (FAILED(Add_Window(TEXT("Animation_Window"),
				CAnimation_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(100.f), _float(-50.f)), ImVec2(446.f, 768.f)))))
				MSG_BOX("Failed Create Animation_Window");
		}

		Setup_Current_Window(TEXT("Animation_Window"));
		m_eLoadingFlag |= ANIAMTION_LOAD;
	}
		

	if (ImGui::RadioButton("UI", iCurMenuList, UI_WINDOW))
	{
		if (0 == (m_eLoadingFlag & UI_LOAD))
		{
			if (FAILED(Add_Window(TEXT("UI_Window"),
				CUI_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(g_iWinSizeX), _float(0.f)), ImVec2(446.f, 768.f)))))
				MSG_BOX("Failed Create UI_Window");
		}
		Setup_Current_Window(TEXT("UI_Window"));
		m_eLoadingFlag |= UI_LOAD;
	}
		

	if (ImGui::RadioButton("Light", iCurMenuList, LIGHT_WINDOW))
	{
		if (0 == (m_eLoadingFlag & LIGHT_LOAD))
		{
			if (FAILED(Add_Window(TEXT("Light_Window"),
				CLight_Window::Create(m_pDevice, m_pContext,
					ImVec2(_float(100 + 8), _float(-50.f)), ImVec2(446.f, 768.f)))))
				MSG_BOX("Failed Create Light_Window");
		}

		Setup_Current_Window(TEXT("Light_Window"));
		m_eLoadingFlag |= LIGHT_LOAD;
	}
		

	ImGui::End();

	if (nullptr != m_pCurrrentWindow)
		m_pCurrrentWindow->Tick(fTimeDelta);
}

HRESULT CWindow_Manager::Render()
{
	if (nullptr != m_pCurrrentWindow)
	{
		if (FAILED(m_pCurrrentWindow->Render()))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CWindow_Manager::Add_Window(const _tchar* pWindowTag, CImWindow* pWindow)
{
	if (nullptr == pWindow)
		return E_FAIL;

	m_ImWindows.emplace(pWindowTag, pWindow);

	return S_OK;
}

CImWindow* CWindow_Manager::Find_Window(const _tchar* pWindowTag)
{
	auto pair = find_if(m_ImWindows.begin(), m_ImWindows.end(), CTag_Finder(pWindowTag));

	if (m_ImWindows.end() == pair)
		return nullptr;

	return pair->second;
}

HRESULT CWindow_Manager::Setup_Current_Window(const _tchar* pTag)
{
	m_pCurrrentWindow = Find_Window(pTag);
	
	if (nullptr == m_pCurrrentWindow)
		return E_FAIL;

	return S_OK;
}

void CWindow_Manager::Free(void)
{
	for (auto& pair : m_ImWindows)
		Safe_Release(pair.second);

	m_ImWindows.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
