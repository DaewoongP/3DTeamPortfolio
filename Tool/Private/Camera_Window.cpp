#include "..\Public\Camera_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
CCamera_Window::CCamera_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}

HRESULT CCamera_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	m_pCutSceneCameraTool = CCutScene_Camera_Tool::Create(m_pDevice, m_pContext);

	return S_OK;
}

void CCamera_Window::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);
	ImGui::Begin("Camera", nullptr, m_WindowFlag);

	m_pCutSceneCameraTool->Tick(_fTimeDelta);

	ImGui::End();
}

HRESULT CCamera_Window::Render()
{
	return S_OK;
}

CCamera_Window* CCamera_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	CCamera_Window* pInstance = New CCamera_Window(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vWindowPos, _vWindowSize)))
	{
		MSG_BOX("Failed Create CCamera_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCamera_Window::Free(void)
{
	__super::Free();

	Safe_Release(m_pCutSceneCameraTool);
	

}