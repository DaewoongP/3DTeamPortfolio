#include "..\Public\Window_Manager.h"
#include "ImWindow.h"

IMPLEMENT_SINGLETON(CWindow_Manager)

HRESULT CWindow_Manager::Initialize()
{
	return S_OK;
}

void CWindow_Manager::Tick(_float fTimeDelta)
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(200.f, 200.f));

	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	_int* iCurMenuList = reinterpret_cast<_int*>(&m_eCurMenuList);
	if (ImGui::RadioButton("Object", iCurMenuList, OBJECT_WINDOW))
		Setup_Current_Window(TEXT("Object_Window"));

	if (ImGui::RadioButton("Mesh", iCurMenuList, MESH_WINDOW))
		Setup_Current_Window(TEXT("Mesh_Window"));

	if (ImGui::RadioButton("NaviMesh", iCurMenuList, NAVIGATION_WINDOW))
		Setup_Current_Window(TEXT("NaviMesh_Window"));

	if (ImGui::RadioButton("Camera", iCurMenuList, CAMERA_WINDOW))
		Setup_Current_Window(TEXT("Camera_Window"));

	if (ImGui::RadioButton("Effect", iCurMenuList, EFFECT_WINDOW))
		Setup_Current_Window(TEXT("Effect_Window"));

	if (ImGui::RadioButton("Animation", iCurMenuList, ANIMATION_WINDOW))
		Setup_Current_Window(TEXT("Animation_Window"));

	if (ImGui::RadioButton("UI", iCurMenuList, UI_WINDOW))
		Setup_Current_Window(TEXT("UI_Window"));

	if (ImGui::RadioButton("Light", iCurMenuList, LIGHT_WIDNOW))
		Setup_Current_Window(TEXT("Light_Window"));

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
}
