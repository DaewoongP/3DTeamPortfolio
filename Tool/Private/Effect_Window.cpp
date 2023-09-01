#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
#include "ParticleSystem.h"
#include "DummyParticle.h"
#include "ComboBox.h"

CEffect_Window::CEffect_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}
HRESULT CEffect_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	m_pDummyParticle = CDummyParticle::Create(m_pDevice, m_pContext);
	m_pParticleSystem = m_pDummyParticle->Get_ParticleSystem();

	m_pEmitterVelocity_ComboBox = CComboBox::Create("Emission Velocity", { "RigidBody", "Transform" });
	return S_OK;
}

void CEffect_Window::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	ImGui::Begin("Effect", nullptr, m_WindowFlag);
	BEGININSTANCE;

	MainMoudle_TreeNode();
	ImGui::Separator();
	EmissionModule_TreeNode();
	ImGui::Separator();
	ShapeModule_TreeNode();
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

	ImGui::ShowDemoWindow();
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
	MAIN_MODULE* pMainModuleDesc = &m_pParticleSystem->m_MainModuleDesc;
	if (ImGui::Checkbox("##MainModule_CheckBox", &pMainModuleDesc->isActivate))
	{
		pMainModuleDesc->isActivate = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("MainModule"))
	{
		if (ImGui::BeginTable("MainModuleTable", 2))
		{
			ImGui::TableNextRow();

			TableDragFloat("Duration", &pMainModuleDesc->fDuration);
			TableCheckBox("Looping", &pMainModuleDesc->isLooping);

			if (true == pMainModuleDesc->isLooping)
			{
				TableCheckBox("Prewarm", &pMainModuleDesc->isLooping);
			}

			if (false == pMainModuleDesc->isPrewarm)
			{
				TableDragFloat2Range("Start Delay", &pMainModuleDesc->fStartDelay);
			}
			TableDragFloat2Range("Start Lifetime", &pMainModuleDesc->fStartLifeTime);
			TableDragFloat2Range("Start Speed", &pMainModuleDesc->fStartSpeed);
			TableCheckBox("3D Start Size", &pMainModuleDesc->is3DStartSize);
			if (true == pMainModuleDesc->is3DStartSize)
			{
				TableDragXYZ("3D Size", &pMainModuleDesc->f3DSizeXYZ);
			}
			else
			{
				TableDragFloat2Range("Start Size", &pMainModuleDesc->fStartSize);
			}
			TableCheckBox("3D Start Rotation", &pMainModuleDesc->is3DStartRotation);
			if (true == pMainModuleDesc->is3DStartRotation)
			{
				TableDragXYZ("3D Rotation", &pMainModuleDesc->f3DRotationXYZ);
			}
			else
			{
				TableDragFloat2Range("Start Rotation", &pMainModuleDesc->fStartRotation);
			}

			TableDragFloat("Flip Rotation", &pMainModuleDesc->fFlipRotation);
			TableColorEdit4("Start Color", &pMainModuleDesc->vStartColor);
			TableDragFloat("GravityModifier", &pMainModuleDesc->fGravityModifier);
			TableDragFloat("SimulationSpeed", &pMainModuleDesc->fSimulationSpeed);
			TableCheckBox("Play On Awake*", &pMainModuleDesc->isPlayOnAwake);

			m_pEmitterVelocity_ComboBox->Show();
			m_pEmitterVelocity_ComboBox->Bind_From_Current_Index(reinterpret_cast<_uint*>(&pMainModuleDesc->eEmmiterVelocity));

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CEffect_Window::EmissionModule_TreeNode()
{
	EMMISION_MODULE* pEmissionModuleDesc = &m_pParticleSystem->m_EmissionModuleDesc;
	if (ImGui::Checkbox("##EmissionModule_CheckBox", &pEmissionModuleDesc->isActivate))
	{
		// 이미션 노드는 항상 참이어야 함.
		pEmissionModuleDesc->isActivate = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("EmissionModule"))
	{
		if (ImGui::BeginTable("EmissionModuleTable", 2))
		{
			ImGui::TableNextRow();

			TableDragFloat2Range("RateOverTime", &pEmissionModuleDesc->fRateOverTime);
			TableDragFloat2Range("RateOverDistance", &pEmissionModuleDesc->fRateOverDistance);
			ImGui::EndTable();
		}

		const _uint iRow = 5;

		// Burst 테이블
		ImGui::Text("Bursts");
		if (ImGui::BeginTable("BurstsTable", iRow))
		{
			// 헤더 행 추가
			const _char* Headers[iRow] = { "Time", "Count", "Cycles", " Interval", "Probability" };
			for (_uint iCol = 0; iCol < iRow; ++iCol)
			{
				ImGui::TableSetupColumn(Headers[iCol]); // 각 열의 설정
			}
			ImGui::TableHeadersRow(); // 헤더 행 추가

			// 행 설정
			for (_uint i = 0; i < pEmissionModuleDesc->Bursts.size(); ++i)
			{
				ImGui::TableNextRow();
				string strTag[iRow] = {
					"##BurstTime" + to_string(i)
					, "##BurstCount" + to_string(i)
					, "##BurstCycles" + to_string(i)
					, "##BurstInterval" + to_string(i)
					, "##BurstProbability" + to_string(i)
				};

				ImGui::PushItemWidth(200.f);

				ImGui::TableSetColumnIndex(0);
				ImGui::DragFloat(strTag[0].data(), &pEmissionModuleDesc->Bursts[i].fTime);

				ImGui::TableSetColumnIndex(1);
				ImGui::DragIntRange2(strTag[1].data(), &pEmissionModuleDesc->Bursts[i].iCount.x
					, &pEmissionModuleDesc->Bursts[i].iCount.y, 0.1f, 0, INT_MAX);

				ImGui::TableSetColumnIndex(2);
				ImGui::DragInt(strTag[2].data(), &pEmissionModuleDesc->Bursts[i].iCycles, 0.1f, 0, INT_MAX);

				ImGui::TableSetColumnIndex(3);
				ImGui::DragFloat(strTag[3].data(), &pEmissionModuleDesc->Bursts[i].fInterval, 0.1f, 0, pEmissionModuleDesc->Bursts[i].fTime - 0.001f);

				ImGui::TableSetColumnIndex(4);
				ImGui::DragFloat(strTag[4].data(), &pEmissionModuleDesc->Bursts[i].fProbability, 0.1f, 0, 1.f);
				ImGui::TableNextRow();

				ImGui::PopItemWidth();
			}
			ImGui::EndTable();

			if (ImGui::Button("Create##BurstsPushBack"))
			{
				pEmissionModuleDesc->Bursts.push_back(EMMISION_MODULE::BURST());
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete##BurstsPopBack"))
			{
				if (false == pEmissionModuleDesc->Bursts.empty())
				{
					pEmissionModuleDesc->Bursts.pop_back();
				}
			}

		}

		ImGui::TreePop(); // SubNode의 끝
	}
}

void CEffect_Window::ShapeModule_TreeNode()
{
	SHAPE_MODULE* pShapeModule = &m_pParticleSystem->m_ShapeModuleDesc;
	if (ImGui::Checkbox("##ShapeModule_CheckBox", &pShapeModule->isActivate))
	{
		// 셰이프 노드는 항상 참이어야 함.
		pShapeModule->isActivate = true;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("ShapeModule"))
	{
		if (ImGui::BeginTable("ShapeModuleTable", 2))
		{
			ImGui::TableNextRow();

			ImGui::EndTable();
		}
	}
}

void CEffect_Window::TableDragFloat(string _strTag, _float* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::DragFloat(strTag.data(), _pValue, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

	ImGui::PopItemWidth();
}
void CEffect_Window::TableDragFloat2Range(string _strTag, _float2* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag + "Range";
	ImGui::DragFloatRange2(strTag.data(), &_pValue->x, &_pValue->y, 0.01f, 0.f, FLT_MAX); ImGui::TableNextRow();

	ImGui::PopItemWidth();
}
void CEffect_Window::TableCheckBox(string _strTag, _bool* _pValue)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::Checkbox(strTag.data(), _pValue); ImGui::TableNextRow();
}
void CEffect_Window::TableColorEdit4(string _strTag, _float4* pValue)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::ColorEdit4(strTag.data(), reinterpret_cast<_float*>(pValue)); ImGui::TableNextRow();
}
void CEffect_Window::TableDragXYZ(string _strTag, _float3* pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	string strTagX = "##" + _strTag + "X";
	string strTagY = "##" + _strTag + "Y";
	string strTagZ = "##" + _strTag + "Z";

	ImVec2 textSize = ImGui::CalcTextSize("X");
	float textWidth = textSize.x;

	ImGui::PushItemWidth(m_fWidgetSize - 30.f);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);

	ImGui::Text("X"); ImGui::SameLine();
	ImGui::DragFloat(strTagX.data(), &pValue->x, _fDragSpeed, _fMin, _fMax);

	ImGui::Text("Y"); ImGui::SameLine();
	ImGui::DragFloat(strTagY.data(), &pValue->y, _fDragSpeed, _fMin, _fMax);

	ImGui::Text("Z"); ImGui::SameLine();
	ImGui::DragFloat(strTagZ.data(), &pValue->z, _fDragSpeed, _fMin, _fMax);

	ImGui::PopItemWidth();

	ImGui::TableNextRow();
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
	Safe_Release(m_pEmitterVelocity_ComboBox);
}