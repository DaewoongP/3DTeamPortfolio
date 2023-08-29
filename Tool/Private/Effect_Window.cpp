#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
CEffect_Window::CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}

HRESULT CEffect_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

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
		ImGui::DragFloat("Duration", &m_MainModuleDesc.fDuration, 0.01f, 0.f, FLT_MAX);
		ImGui::Checkbox("Looping", &m_MainModuleDesc.isLooping);
		ImGui::Checkbox("Prewarm", &m_MainModuleDesc.isPrewarm);
		ImGui::DragFloat("StartDelay", &m_MainModuleDesc.fStartDelay, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartLifeTime", &m_MainModuleDesc.fStartLifeTime, 0.01f, 0.f, FLT_MAX);
		ImGui::DragFloat("StartSpeed", &m_MainModuleDesc.fStartSpeed, 0.01f, 0.f, FLT_MAX);
		ImGui::TreePop(); // SubNode의 끝
	}

	SaveFileDialog();
	LoadFileDialog();
	
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
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			_tchar wszFilePath[MAX_PATH];
			ZeroMemory(wszFilePath, sizeof(_tchar) * MAX_PATH);
			CharToWChar(filePathName.c_str(), wszFilePath);

			HANDLE hFile = CreateFile(wszFilePath,
				GENERIC_WRITE,
				0,
				0,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, 0
			);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				MSG_BOX("The file open has been failed");
				return;
			}

			_ulong dwByte = 0;

			WriteFile(hFile, &m_MainModuleDesc, sizeof(MAIN_MODULE), &dwByte, nullptr);

			MSG_BOX("The file has been saved successfully");

			CloseHandle(hFile);
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
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			_tchar wszFilePath[MAX_PATH];
			ZeroMemory(wszFilePath, sizeof(_tchar) * MAX_PATH);
			CharToWChar(filePathName.c_str(), wszFilePath);

			HANDLE hFile = CreateFile(wszFilePath,
				GENERIC_READ,
				0,
				0,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 0
			);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				MSG_BOX("The file open has been failed");
				return;
			}

			_ulong dwByte = 0;
			
			ZEROMEM(&m_MainModuleDesc);
			ReadFile(hFile, &m_MainModuleDesc, sizeof(MAIN_MODULE), &dwByte, nullptr);
			
			MSG_BOX("The file has been loaded successfully");

			CloseHandle(hFile);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CEffect_Window::CreateFileDialog()
{
	if (ImGui::Button("Create"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseCreateFilePtcKey", "Create File", ".ptc", "../../Resources/Effects/Particles/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseCreateFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			_tchar wszFilePath[MAX_PATH];
			ZeroMemory(wszFilePath, sizeof(_tchar) * MAX_PATH);
			CharToWChar(filePathName.c_str(), wszFilePath);

			_ulong dwByte = 0;
			MAIN_MODULE test;
			ZEROMEM(&test);

			Safe_Release(m_pDummyParticle);
			//m_pDummyParticle = CParticleSystem::Create(m_pDevice, m_pContext, wszFilePath);
			
			MSG_BOX("Created Successfully");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
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