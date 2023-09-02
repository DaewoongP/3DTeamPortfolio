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

	m_pEmitterVelocity_ComboBox = CComboBox::Create("Com_Emission Velocity", "Emission Velocity", { "RigidBody", "Transform" });
	m_pShapeCombo = CComboBox::Create("##Com_Shape", "Shape", { "Sphere", "HemiSphere", "Cone", "Mesh", "Circle", "Edge" });
	m_pModeCombo = CComboBox::Create("##Com_BurstMode", "Mode", { "Random", "Loop", "Ping-Pong", "Burst_Spread" });
	m_pBurstTypeCombo = CComboBox::Create("##Com_MeshType", "Type", { "Vertex", "Edge", "Triangle" });
	m_pArcModeCombo = CComboBox::Create("##Com_ArcMode", "Mode", { "Random", "Loop", "Ping-Pong" });
	m_pMeshCombo = CComboBox::Create("##Com_Mesh", "Mesh", { "Cube", "Capsule", "Capsule", "Cylinder", "Plane", "Sphere", "Quad" });
	m_pEmitFromCombo = CComboBox::Create("##Com_EmitFrom", "Emit From", { "Base", "Volume" });
	m_pMeshTypeCombo = CComboBox::Create("##Com_MeshType", "Type", { "Vertex", "Edgd", "Triangle"});


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
	// 버튼 누르면 트리노드가 팝업된다.
	// 버튼의 이미지는 현재 이미지 버튼
	// 누르면 파일 경로가 나온다.
	// 
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

			Table_DragFloat("Duration", &pMainModuleDesc->fDuration);
			Table_CheckBox("Looping", &pMainModuleDesc->isLooping);

			if (true == pMainModuleDesc->isLooping)
			{
				Table_CheckBox("Prewarm", &pMainModuleDesc->isLooping);
			}

			if (false == pMainModuleDesc->isPrewarm)
			{
				Table_DragFloat2Range("Start Delay", &pMainModuleDesc->fStartDelay);
			}
			Table_DragFloat2Range("Start Lifetime", &pMainModuleDesc->fStartLifeTime);
			Table_DragFloat2Range("Start Speed", &pMainModuleDesc->fStartSpeed);
			Table_CheckBox("3D Start Size", &pMainModuleDesc->is3DStartSize);
			if (true == pMainModuleDesc->is3DStartSize)
			{
				Table_DragXYZ("3D Size", &pMainModuleDesc->f3DSizeXYZ);
			}
			else
			{
				Table_DragFloat2Range("Start Size", &pMainModuleDesc->fStartSize);
			}
			Table_CheckBox("3D Start Rotation", &pMainModuleDesc->is3DStartRotation);
			if (true == pMainModuleDesc->is3DStartRotation)
			{
				Table_DragXYZ("3D Rotation", &pMainModuleDesc->f3DRotationXYZ);
			}
			else
			{
				Table_DragFloat2Range("Start Rotation", &pMainModuleDesc->fStartRotation);
			}

			Table_DragFloat("Flip Rotation", &pMainModuleDesc->fFlipRotation);
			Table_ColorEdit4("Start Color", &pMainModuleDesc->vStartColor);
			Table_DragFloat("GravityModifier", &pMainModuleDesc->fGravityModifier);
			Table_DragFloat("SimulationSpeed", &pMainModuleDesc->fSimulationSpeed);
			Table_CheckBox("Play On Awake*", &pMainModuleDesc->isPlayOnAwake);

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

			Table_DragFloat2Range("RateOverTime", &pEmissionModuleDesc->fRateOverTime);
			Table_DragFloat2Range("RateOverDistance", &pEmissionModuleDesc->fRateOverDistance);
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

			m_pShapeCombo->Show(CComboBox::FLAG::TABLE);
			m_pModeCombo->Show(CComboBox::FLAG::TABLE);
			m_pBurstTypeCombo->Show(CComboBox::FLAG::TABLE);
			m_pMeshCombo->Show(CComboBox::FLAG::TABLE);
			m_pMeshTypeCombo->Show(CComboBox::FLAG::TABLE);

			Table_CheckBox("SingleMaterial", &pShapeModule->isSingleMaterial);
			Table_DragInt("MaterialNum", reinterpret_cast<_int*>(&pShapeModule->iMaterialNum));
			Table_CheckBox("UseMeshColors", &pShapeModule->isUseMeshColors);
			Table_DragFloat("NormalOffset", &pShapeModule->fNormalOffset);

			m_pEmitFromCombo->Show(CComboBox::FLAG::TABLE);
			Table_DragFloat("Angle", &pShapeModule->fAngle);
			Table_DragFloat("Radius", &pShapeModule->fRadius);
			Table_DragFloat("DonutRadius", &pShapeModule->fDonutRadius);
			Table_DragFloat("RadiusThickness", &pShapeModule->fRadiusThickness, 0.01f, 0.f, 1.f);

			Table_DragFloat("Arc", &pShapeModule->fArc);
			m_pArcModeCombo->Show(CComboBox::FLAG::TABLE);
			Table_DragFloat("Spread", &pShapeModule->fSpread, 0.01f, 0.f, 1.f);
			
			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}

void CEffect_Window::Table_DragFloat(string _strTag, _float* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::DragFloat(strTag.data(), _pValue, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
}
void CEffect_Window::Table_DragFloat2Range(string _strTag, _float2* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag + "Range";
	ImGui::DragFloatRange2(strTag.data(), &_pValue->x, &_pValue->y, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
}
void CEffect_Window::Table_CheckBox(string _strTag, _bool* _pValue)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::Checkbox(strTag.data(), _pValue); ImGui::TableNextRow();
}
void CEffect_Window::Table_ColorEdit4(string _strTag, _float4* pValue)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::ColorEdit4(strTag.data(), reinterpret_cast<_float*>(pValue)); ImGui::TableNextRow();
}
void CEffect_Window::Table_DragXYZ(string _strTag, _float3* pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
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
void CEffect_Window::Table_DragInt(string _strTag, _int* _pValue, _float _fDragSpeed, _int _iMin, _int _iMax)
{
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strTag.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	ImGui::DragInt(strTag.data(), _pValue, _fDragSpeed, _iMin, _iMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
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
	Safe_Release(m_pShapeCombo);
	Safe_Release(m_pModeCombo);
	Safe_Release(m_pBurstTypeCombo);
	Safe_Release(m_pArcModeCombo);
	Safe_Release(m_pMeshCombo);
	Safe_Release(m_pEmitFromCombo);
	Safe_Release(m_pMeshTypeCombo);
}