#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
CEffect_Window::CEffect_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CEffect_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CEffect_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Effect", nullptr, m_WindowFlag);
	BEGININSTANCE;

	if (ImGui::Checkbox("##MainModule_CB", &m_bMainCheckBox))
	{
		// 항상 true로 고정.
		m_bMainCheckBox = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("MainModule"))
	{
		ImGui::DragFloat("Duration", &m_MainModuleDesc.fDuration, 0.01f, 0.f, FLT_MAX);
		ImGui::Checkbox("Looping", &m_MainModuleDesc.isLooping);
		ImGui::Checkbox("Prewarm", &m_MainModuleDesc.isPrewarm);
		ImGui::DragFloat("StartDelay", &m_MainModuleDesc.fStartDelay, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartLifeTime", &m_MainModuleDesc.fStartLifeTime, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartSpeed", &m_MainModuleDesc.fStartSpeed, 0.01f, 0.f, FLT_MAX);
		ImGui::TreePop(); // SubNode의 끝
	}

	// open Dialog Simple
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseFilePtcKey", "Save File", ".ptc", "../../Resources/Effects/Particles/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	ENDINSTANCE;
	ImGui::End();
}

HRESULT CEffect_Window::Render()
{
	return S_OK;
}

CEffect_Window* CEffect_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CEffect_Window* pInstance = New CEffect_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CEffect_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CEffect_Window::Free(void)
{
	__super::Free();
}
