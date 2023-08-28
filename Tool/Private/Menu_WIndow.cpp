#include "..\Public\Menu_Window.h"
#include "Window_Manager.h"
CMenu_WIndow::CMenu_WIndow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CMenu_WIndow::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CMenu_WIndow::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Menu", nullptr, m_WindowFlag);

	_int* iCurMenuList = reinterpret_cast<_int*>(&m_eCurMenuList);
	if (ImGui::RadioButton("Object", iCurMenuList, OBJECT_WINDOW))
		Setup_Current_Window(L"Object_Window");

	if(ImGui::RadioButton("Mesh", iCurMenuList, MESH_WINDOW))
		Setup_Current_Window(L"Mesh_Window");

	if(ImGui::RadioButton("NaviMesh", iCurMenuList, NAVIMESH_WINDOW))
		Setup_Current_Window(L"NaviMesh_Window");

	if(ImGui::RadioButton("Camera", iCurMenuList, CAMERA_WINDOW))
		Setup_Current_Window(L"Camera_Window");

	if(ImGui::RadioButton("Effect", iCurMenuList, EFFECT_WINDOW))
		Setup_Current_Window(L"Effect_Window");

	if(ImGui::RadioButton("Animation", iCurMenuList, ANIMATION_WINDOW))
		Setup_Current_Window(L"Animation_Window");

	if(ImGui::RadioButton("UI", iCurMenuList, UI_WINDOW))
		Setup_Current_Window(L"UI_Window");

	if(ImGui::RadioButton("Light", iCurMenuList, LIGHT_WIDNOW))
		Setup_Current_Window(L"Light_Window");
	
	ImGui::End();
}

HRESULT CMenu_WIndow::Render()
{
	return S_OK;
}

void CMenu_WIndow::Setup_Current_Window(const _tchar* pTag)
{
	CWindow_Manager* pWindowMgr = CWindow_Manager::GetInstance();
	Safe_AddRef(pWindowMgr);

	pWindowMgr->Set_Cur_Window(pWindowMgr->Find_Window(L"Object_Window"));

	Safe_Release(pWindowMgr);
}

CMenu_WIndow* CMenu_WIndow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CMenu_WIndow* pInstance = New CMenu_WIndow(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CMenu_WIndow");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMenu_WIndow::Free(void)
{
	__super::Free();
}
