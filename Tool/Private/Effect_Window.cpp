#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
#include "ParticleSystem.h"
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

	MainMoudle_TreeNode();
	//EmissionModule_TreeNode();
	
	ImGui::Separator();
	Save_FileDialog();
	Load_FileDialog();
	Create_Button();

	ENDINSTANCE;
	ImGui::End();

	if (nullptr != m_pDummyParticle)
	{
		m_pDummyParticle->Tick(_fTimeDelta);
		m_pDummyParticle->Late_Tick(_fTimeDelta);
	}
	_float4x4 pMatrix = m_pDummyParticle->Get_Transform()->Get_WorldMatrix();
	__super::MatrixNode(&pMatrix, "Effect_Transform", "Effect_Position", "Effect_Rotation", "Effect_Scale");
	m_pDummyParticle->Get_Transform()->Set_WorldMatrix(pMatrix);
}

void CEffect_Window::Save_FileDialog()
{
	if (ImGui::Button("Save Particle"))
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
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CEffect_Window::Load_FileDialog()
{
	if (ImGui::Button("Load Particle"))
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
			CharToWChar(filePath.c_str(), wszFilePath);

			_ulong dwByte = 0;
			
			m_pDummyParticle->LoadParticle(wszFilePath);

			MSG_BOX("The file has been loaded successfully");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CEffect_Window::Create_Button()
{
	if (ImGui::Button("New Particle"))
	{
		Safe_Release(m_pDummyParticle);
		m_pDummyParticle = CDummyParticle::Create(m_pDevice, m_pContext);
	}
}

void CEffect_Window::MainMoudle_TreeNode()
{
	if (ImGui::Checkbox("##MainModule_CheckBox", &m_bMainCheckBox))
	{
		m_bMainCheckBox = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("MainModule"))
	{
		if (ImGui::BeginTable("MainModuleTable", 2))
		{
			MAIN_MODULE* pMainModuleDesc = { nullptr };
			if (nullptr != m_pDummyParticle)
			{
				pMainModuleDesc = m_pDummyParticle->Get_ParticleSystem()->Get_MainModulePtr();
			}
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Duration"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloat("##Duration", &(*pMainModuleDesc).fDuration, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Looping"); ImGui::TableSetColumnIndex(1);
			ImGui::Checkbox("##Looping", &(*pMainModuleDesc).isLooping); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Prewarm"); ImGui::TableSetColumnIndex(1);
			ImGui::Checkbox("##Prewarm", &(*pMainModuleDesc).isPrewarm); ImGui::TableNextRow();

			ImGui::PushItemWidth(200.0f);

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartDelay"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartDelayRange", &(*pMainModuleDesc).fStartDelay.x, &(*pMainModuleDesc).fStartDelay.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartLifeTime"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartLifeTimeRange", &(*pMainModuleDesc).fStartLifeTime.x, &(*pMainModuleDesc).fStartLifeTime.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartSpeed"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartSpeedRange", &(*pMainModuleDesc).fStartSpeed.x, &(*pMainModuleDesc).fStartSpeed.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}

void CEffect_Window::EmissionModule_TreeNode()
{
	if (ImGui::Checkbox("##EmissionModule_CheckBox", &m_bMainCheckBox))
	{
		m_bMainCheckBox = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("EmissionModule"))
	{
		if (ImGui::BeginTable("EmissionModuleTable", 2))
		{
			MAIN_MODULE* pMainModuleDesc = { nullptr };
			if (nullptr != m_pDummyParticle)
			{
				pMainModuleDesc = m_pDummyParticle->Get_ParticleSystem()->Get_MainModulePtr();
			}
			ImGui::TableNextRow();
			
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Duration"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloat("##Duration", &(*pMainModuleDesc).fDuration, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Looping"); ImGui::TableSetColumnIndex(1);
			ImGui::Checkbox("##Looping", &(*pMainModuleDesc).isLooping); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Prewarm"); ImGui::TableSetColumnIndex(1);
			ImGui::Checkbox("##Prewarm", &(*pMainModuleDesc).isPrewarm); ImGui::TableNextRow();

			ImGui::PushItemWidth(200.0f);

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartDelay"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartDelayRange", &(*pMainModuleDesc).fStartDelay.x, &(*pMainModuleDesc).fStartDelay.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartLifeTime"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartLifeTimeRange", &(*pMainModuleDesc).fStartLifeTime.x, &(*pMainModuleDesc).fStartLifeTime.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("StartSpeed"); ImGui::TableSetColumnIndex(1);
			ImGui::DragFloatRange2("##StartSpeedRange", &(*pMainModuleDesc).fStartSpeed.x, &(*pMainModuleDesc).fStartSpeed.y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
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