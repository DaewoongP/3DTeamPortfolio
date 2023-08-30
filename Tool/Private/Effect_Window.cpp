#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
#include "DummyParticle.h"

CEffect_Window::CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}

HRESULT CEffect_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	BEGININSTANCE;
	
	m_pDummyParticle = CDummyParticle::Create(m_pDevice, m_pContext);

	ENDINSTANCE
	return S_OK;
}

void CEffect_Window::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);
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
		MAIN_MODULE* pMainModuleDesc = { nullptr };
		if (nullptr != m_pDummyParticle)
		{
			CMainModule* pMainModule = static_cast<CMainModule*>(m_pDummyParticle->Get_ParticleSystem()->Get_Module(CParticleSystem::MAIN));
			pMainModuleDesc = pMainModule->Get_DescPtr();
		}

		ImGui::DragFloat("Duration", &(*pMainModuleDesc).fDuration, 0.01f, 0.f, FLT_MAX);
		ImGui::Checkbox("Looping", &(*pMainModuleDesc).isLooping);
		ImGui::Checkbox("Prewarm", &(*pMainModuleDesc).isPrewarm);
		ImGui::DragFloat("StartDelay", &(*pMainModuleDesc).fStartDelay, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartLifeTime", &(*pMainModuleDesc).fStartLifeTime, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartSpeed", &(*pMainModuleDesc).fStartSpeed, 0.01f, 0.f, FLT_MAX);
		ImGui::TreePop(); // SubNode의 끝
	}

	SaveFileDialog();
	LoadFileDialog();
	CreateButton();

	ENDINSTANCE;
	ImGui::End();

	if (nullptr != m_pDummyParticle)
	{
		m_pDummyParticle->Tick(_fTimeDelta);
		m_pDummyParticle->Late_Tick(_fTimeDelta);
	}
}

HRESULT CEffect_Window::Render()
{
	return S_OK;
}

void CEffect_Window::SaveFileDialog()
{
	if (ImGui::Button("Save Particle File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseSaveFilePtcKey", "Save File", ".ptc", "../../Resources/Effects/Particles/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseSaveFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			if (nullptr == m_pDummyParticle)
			{
				MSG_BOX("DummyParticle is a Null-Object. Please create any object.");
				return;
			}

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			_tchar wszFilePath[MAX_PATH];
			ZeroMemory(wszFilePath, sizeof(_tchar) * MAX_PATH);
			CharToWChar(filePathName.c_str(), wszFilePath);

			m_pDummyParticle->Get_ParticleSystem()->Save(wszFilePath);
			
			MSG_BOX("The file has been saved successfully");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CEffect_Window::LoadFileDialog()
{
	if (ImGui::Button("Load Particle File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseLoadFilePtcKey", "Load File", ".ptc", "../../Resources/Effects/Particles/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseLoadFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			if (nullptr == m_pDummyParticle)
			{
				MSG_BOX("DummyParticle is a Null-Object. Please create any object.");
				return;
			}

			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			_tchar wszFilePath[MAX_PATH];
			ZeroMemory(wszFilePath, sizeof(_tchar) * MAX_PATH);
			CharToWChar(filePathName.c_str(), wszFilePath);

			_ulong dwByte = 0;
			
			m_pDummyParticle->LoadParticle(wszFilePath);

			MSG_BOX("The file has been loaded successfully");

		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CEffect_Window::CreateButton()
{
	if (ImGui::Button("New Particle"))
	{
		Safe_Release(m_pDummyParticle);
		m_pDummyParticle = CDummyParticle::Create(m_pDevice, m_pContext);
	}
}

CEffect_Window* CEffect_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	CEffect_Window* pInstance = New CEffect_Window(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vWindowPos, _vWindowSize)))
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

	Safe_Release(m_pDummyParticle);
}