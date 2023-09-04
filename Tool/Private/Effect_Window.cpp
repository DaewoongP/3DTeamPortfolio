#include "..\Public\Effect_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"
#include "ParticleSystem.h"
#include "DummyParticle.h"
#include "ComboBox.h"
#include "ImageFileDialog.h"

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

	m_pEmitterVelocity_ComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Emission Velocity", { "RigidBody", "Transform" });
	m_pShapeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Shape", { "Sphere", "HemiSphere", "Cone", "Donut", "Box", "Mesh", "Sprite", "Circle", "Edge", "Rectangle"});
	m_pMeshModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong" });
	m_pSpriteTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pArcModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong, Burst_Spread" });
	m_pMeshCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Mesh", { "Cube", "Capsule", "Capsule", "Cylinder", "Plane", "Sphere", "Quad" });
	m_pConeEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Base", "Volume" });
	m_pBoxEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Volume", "Shell", "Edge" });
	m_pMeshTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "ClipChannel", { "Red", "Green", "Blue", "Alpha" }, "Alpha");

	m_pAlphaTexture = CImageFileDialog::Create(m_pDevice, "SelectTexture2D");
	m_pAlphaTexture->m_strStartPath = "../../Resources/Default/Textures/Particles/";
	m_pAlphaTexture->m_iImageButtonWidth = 32;

	m_pSpriteTexture = CImageFileDialog::Create(m_pDevice, "SpriteImageDialog");
	m_pSpriteTexture->m_strStartPath = "../../Resources/Default/Textures/Particles/";
	m_pSpriteTexture->m_iImageButtonWidth = 32;

	m_pMaterialTexture = CImageFileDialog::Create(m_pDevice, "MainTextureDialog");
	m_pMaterialTexture->m_strStartPath = "../../Resources/UI/UI/Game/VFX/Textures/";
	m_pMaterialTexture->m_iImageButtonWidth = 32;
	return S_OK;
}

void CEffect_Window::Tick(_float _fTimeDelta)
{
	__super::Tick(_fTimeDelta);

	ImGui::Begin("Effect", nullptr, m_WindowFlag);

	MainMoudle_TreeNode();
	ImGui::Separator();
	EmissionModule_TreeNode();
	ImGui::Separator();
	ShapeModule_TreeNode();
	ImGui::Separator();
	RendererModule_TreeNode();
	ImGui::Separator();
	Save_FileDialog();
	Load_FileDialog();
	Create_Button();

	ImGui::End();

	if (nullptr != m_pDummyParticle)
	{
		m_pDummyParticle->Tick(_fTimeDelta);
		m_pDummyParticle->Late_Tick(_fTimeDelta);
	}

	// 왼쪽 아래로 고정
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 0.f), _float(Top) };
	ImGui::SetNextWindowPos(vWinpos);
	_float4x4 pMatrix = m_pDummyParticle->Get_Transform()->Get_WorldMatrix();
	__super::MatrixNode(&pMatrix, "Effect_Transform", "Effect_Position", "Effect_Rotation", "Effect_Scale");
	m_pDummyParticle->Get_Transform()->Set_WorldMatrix(pMatrix);

	//ImGui::ShowDemoWindow();
}

void CEffect_Window::Save_FileDialog()
{
	if (ImGui::Button("Save Particle"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseSaveFilePtcKey", "Save File", ".ptc", "../../Resources/Effects/Particles/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseSaveFilePtcKey"))
	{
		// 자신의 정보는 자기가 세이브 시킨다.
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

			fs::path fsFilePath = ToRelativePath(filePath.data());

			m_pDummyParticle->SaveParticle(fsFilePath.wstring().data());
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

			Table_DragFloat("Duration", "ciovj380238uf", &pMainModuleDesc->fDuration);
			Table_CheckBox("Looping", "eu9v73429hc", &pMainModuleDesc->isLooping);
			if (m_isPrevLooping != pMainModuleDesc->isLooping)
			{
				m_pParticleSystem->Reset_AllParticles();
				m_pParticleSystem->Restart();
				m_isPrevLooping = pMainModuleDesc->isLooping;
			}	

			if (true == pMainModuleDesc->isLooping)
			{
				Table_CheckBox("Prewarm", "cv8yu893cnns", &pMainModuleDesc->isPrewarm, false);
			}
			
			if (false == pMainModuleDesc->isPrewarm)
			{
				Table_DragFloatWithOption("Start Delay", "xclvijowiejfc", &pMainModuleDesc->fStartDelay, &pMainModuleDesc->vStartDelayRange, &pMainModuleDesc->isStartDelayRange);
			}
			Table_DragFloatWithOption("Start Lifetime", "xcklvjiej", &pMainModuleDesc->fStartLifeTime, &pMainModuleDesc->vStartLifeTimeRange, &pMainModuleDesc->isStartLifeTimeRange);
			Table_DragFloatWithOption("Start Speed", "cvi93u7yfvrub", &pMainModuleDesc->fStartSpeed, &pMainModuleDesc->vStartSpeedRange, &pMainModuleDesc->isStartSpeedRange);
			//Table_DragFloat2Range("Start Speed", "cvi93u7yfvrub", );
			Table_CheckBox("3D Start Size", "sdfvu89ywr978h", &pMainModuleDesc->is3DStartSize);
			if (true == pMainModuleDesc->is3DStartSize)
			{
				Table_DragXYZ("3D Size", "1w978h9f8vhjejf", &pMainModuleDesc->f3DSizeXYZ);
			}
			else
			{
				Table_DragFloatWithOption("Start Size", "XCV89024890R0WRFOU", &pMainModuleDesc->fStartSize, &pMainModuleDesc->vStartSizeRange, &pMainModuleDesc->isStartSizeRange);
			}
			Table_CheckBox("3D Start Rotation", "vbe088030j45", &pMainModuleDesc->is3DStartRotation);
			if (true == pMainModuleDesc->is3DStartRotation)
			{
				Table_DragXYZ("3D Rotation", "vwsdovuihe90f834", &pMainModuleDesc->f3DRotationXYZ, 0.01f, 0.f, 360.f, false);
			}
			else
			{
				Table_DragFloatWithOption("Start Rotation", "dfvnko89h712enuo", &pMainModuleDesc->fStartRotation, &pMainModuleDesc->vStartRotationRange, &pMainModuleDesc->isStartRotationRange, 0.1f, 0.f, 360.f);
			}

			Table_DragFloat("Flip Rotation", "xcv ioiw", &pMainModuleDesc->fFlipRotation, 0.01f, 0.f, 1.f);
			Table_ColorEdit4("Start Color", "ergop805", &pMainModuleDesc->vStartColor);
			Table_DragFloat("GravityModifier", "g50j8dfbji0", &pMainModuleDesc->fGravityModifier);
			Table_DragFloat("SimulationSpeed", "a1ip40c854dfg", &pMainModuleDesc->fSimulationSpeed);
			Table_CheckBox("Play On Awake*", "zxci0pj380uj", &pMainModuleDesc->isPlayOnAwake);
			pMainModuleDesc->strEmmiterVelocity = m_pEmitterVelocity_ComboBox->Tick(CComboBox::FLAG::TABLE, false);
			if (Table_DragInt("Max Particles", "cv883diicvxcv", &pMainModuleDesc->iMaxParticles, 0.6f, 1, MAX_PARTICLE_NUM))
			{
				m_pParticleSystem->RemakeBuffer(pMainModuleDesc->iMaxParticles);
			}

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

			Table_DragFloat("RateOverTime", "v09u0u6oij4io", &pEmissionModuleDesc->fRateOverTime);
			Table_DragFloat("RateOverDistance", "cio7 7v7e4i", &pEmissionModuleDesc->fRateOverDistance);
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

			// 셰이프
			pShapeModule->strShape = m_pShapeCombo->Tick(CComboBox::FLAG::TABLE);
			string strShape = pShapeModule->strShape;
			//--------------------------------------------------

			if (strShape == "Box")
				pShapeModule->strBoxEmitFrom = m_pBoxEmitFromCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Mesh")
			{
				m_pMeshTypeCombo->Tick(CComboBox::FLAG::TABLE);
				m_pMeshModeCombo->Tick(CComboBox::FLAG::TABLE);
				m_pMeshCombo->Tick(CComboBox::FLAG::TABLE);
				Table_CheckBox("SingleMaterial", "XCVVO0UI89HJ", &pShapeModule->isSingleMaterial);
				Table_CheckBox("UseMeshColors", "DCSKOLPJF087983", &pShapeModule->isUseMeshColors);
			}			

			if (strShape == "Sprite")
				m_pSpriteTypeCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Sprite")
				Table_ImageButton("Sprite", "zxcvouiorvj39dxcv", m_pSpriteTexture);

			if (strShape == "Mesh" || strShape == "Sprite")
				Table_DragFloat("NormalOffset", "SDFPI48083X", &pShapeModule->fNormalOffset);

			if (strShape == "Cone")
				Table_DragFloat("Angle", "QWD0U8I38F", &pShapeModule->fAngle);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle" || strShape == "Edge")
				Table_DragFloat("Radius", "DVIFV809082CV", &pShapeModule->fRadius);

			if (strShape == "Donut")
				Table_DragFloat("DonutRadius", "WSDFVIO0H13890802", &pShapeModule->fDonutRadius);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				Table_DragFloat("RadiusThickness", "1AOUYU89AYIXC78", &pShapeModule->fRadiusThickness, 0.01f, 0.f, 1.f);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				Table_DragFloat("Arc", "XVBIO008DFB89", &pShapeModule->fArc);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				m_pArcModeCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				Table_DragFloat("   Spread", "SCFVPIFV8989R", &pShapeModule->fSpread, 0.01f, 0.f, 1.f);

			if (strShape == "Cone" && pShapeModule->strConeEmitFrom == "Base")
				Table_DragFloat("Length", "diovoirdmcmxjk", &pShapeModule->fLength);

			if (strShape == "Cone")
				m_pConeEmitFromCombo->Tick(CComboBox::FLAG::TABLE);

			// ---------------------------------------------------------------
			Table_Void();

			Table_ImageButton("Texture", "XCVKPVUIIUE", m_pAlphaTexture);
			pShapeModule->strClipChannel = m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
			Table_DragFloat("Clip Threshold", "XC08VB890890T4", &pShapeModule->fRadiusThickness, 0.01f, 0.f, 1.f);
			Table_CheckBox("Color affects Paricles", "D3UIOF79H79EFB", &pShapeModule->isColorAffectsParticles);
			Table_CheckBox("Alpha affects Paricles", "ZXCIUFV7980RV890", &pShapeModule->isAlphaAffectsParticles);
			Table_CheckBox("Bilinear Filtering", "X2EN90UZX3792E79FH", &pShapeModule->isBilinearFiltering);

			Table_Void();

			Table_DragXYZ("Position", "ZXCOUIV834DCV", &pShapeModule->fPosition);
			Table_DragXYZ("Rotation", "vioc4ijdfg", &pShapeModule->fRotation);
			Table_DragXYZ("Scale", "xciv8348kd", &pShapeModule->fScale);

			Table_Void();

			Table_DragFloat("Randomize Direction", "XC08xcsd890890T4", &pShapeModule->fRandomizeDirection, 0.01f, 0.f, 1.f);
			Table_DragFloat("Spherize Direction", "XC08wceV90cvT4", &pShapeModule->fSpherizeDirection, 0.01f, 0.f, 1.f);
			Table_DragFloat("Randomize Position", "xcf8u8vceV90", &pShapeModule->fRandomizePosition);

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CEffect_Window::RendererModule_TreeNode()
{
	RENDERER_MODULE* pRendererModule = &m_pParticleSystem->m_RendererModuleDesc;
	if (ImGui::Checkbox("##ShapeModule_CheckBox", &pRendererModule->isActivate))
	{
		// 렌더러 노드는 항상 참이어야 함.
		pRendererModule->isActivate = true;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("RendererModule"))
	{
		if (ImGui::BeginTable("RendererModuleTable", 2))
		{
			ImGui::TableNextRow();

			Table_ImageButton("Material", "xcvi3489009dv", m_pMaterialTexture);
			if (m_pMaterialTexture->IsOk())
			{
				string strFilePath = m_pMaterialTexture->Get_FilePathName();
				fs::path fsFilePath = ToRelativePath(strFilePath.data());
				m_pParticleSystem->ChangeMainTexture(fsFilePath.wstring().data());
			}

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
_bool CEffect_Window::Table_DragFloat(string strName, string _strTag, _float* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	//const static string strTag = Generate_Hashtag(true);
	isResult = ImGui::DragFloat(strTag.data(), _pValue, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_DragFloat2Range(string _strName, string _strTag, _float2* _pValue, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragFloatRange2(strTag.data(), &_pValue->x, &_pValue->y, _fDragSpeed, _fMin, _fMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();
	return isResult;
}
_bool CEffect_Window::Table_CheckBox(string _strName, string _strTag, _bool* _pValue, _bool isImplement)
{
	_bool isResult = false;
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::Checkbox(strTag.data(), _pValue);
	if (false == isImplement)
	{
		ImGui::SameLine(); ImGui::Text("Not Imple");
	}
	ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_ColorEdit4(string _strName, string _strTag, _float4* pValue)
{
	_bool isResult = false;
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::ColorEdit4(strTag.data(), reinterpret_cast<_float*>(pValue)); ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_ImageButton(string _strName, string _strTag, CImageFileDialog* _pImageFileDialog)
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	_pImageFileDialog->Tick(); ImGui::TableNextRow();
	return true;
}
_bool CEffect_Window::Table_Void()
{
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(" ");
	ImGui::TableSetColumnIndex(1);
	ImGui::Text(" "); ImGui::TableNextRow();
	return true;
}
_bool CEffect_Window::Table_DragFloatWithOption(string _strName, string _strTag, _float* _pValue, _float2* _pRangeValue, _bool* _pCheckBox, _float _fDragSpeed, _float _fMin, _float _fMax)
{
	_bool isResult = false;
	string strTag = "##" + _strTag;
	string strCheckBoxTag = strTag + "CheckBox";
	ImGui::PushItemWidth(m_fWidgetSize);
	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	if (true == *_pCheckBox)
	{
		string strRangeTag = strTag + "Range";
		isResult = ImGui::DragFloatRange2(strTag.data(), &_pRangeValue->x, &_pRangeValue->y, _fDragSpeed, _fMin, _fMax);
	}
	else
	{
		string strFloatTag = strTag + "Float";
		isResult = ImGui::DragFloat(strTag.data(), _pValue, _fDragSpeed, _fMin, _fMax);
	}
		
	ImGui::SameLine(); ImGui::Checkbox(strCheckBoxTag.data(), _pCheckBox);
	ImGui::PopItemWidth();
	ImGui::TableNextRow();
	return isResult;
}
_bool CEffect_Window::Table_DragXYZ(string _strName, string _strTag, _float3* pValue, _float _fDragSpeed, _float _fMin, _float _fMax, _bool isImplement)
{
	string strTagX = "##" + _strTag + "X"; // X, Y, Z를 구분하도록 수정
	string strTagY = "##" + _strTag + "Y";
	string strTagZ = "##" + _strTag + "Z";
	_bool isResult = false;
	ImVec2 textSize = ImGui::CalcTextSize("X");
	_float textWidth = textSize.x;

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data());
	ImGui::TableSetColumnIndex(1);

	ImGui::PushItemWidth(40.0f);  // DragFloat의 길이를 60으로 설정

	ImGui::Text("X"); ImGui::SameLine();
	isResult = ImGui::DragFloat(strTagX.data(), &pValue->x, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::Text("Y"); ImGui::SameLine();
	isResult = ImGui::DragFloat(strTagY.data(), &pValue->y, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::Text("Z"); ImGui::SameLine();
	isResult = ImGui::DragFloat(strTagZ.data(), &pValue->z, _fDragSpeed, _fMin, _fMax); ImGui::SameLine();

	ImGui::PopItemWidth();  // 설정을 원래대로 돌려놓습니다

	if (false == isImplement)
	{
		ImGui::SameLine(); ImGui::Text("Not Imple");
	}

	ImGui::TableNextRow();

	return isResult;
}
_bool CEffect_Window::Table_DragInt(string _strName, string _strTag, _int* _pValue, _float _fDragSpeed, _int _iMin, _int _iMax)
{
	_bool isResult = false;
	ImGui::PushItemWidth(m_fWidgetSize);

	ImGui::TableSetColumnIndex(0);
	ImGui::Text(_strName.data()); ImGui::TableSetColumnIndex(1);
	string strTag = "##" + _strTag;
	isResult = ImGui::DragInt(strTag.data(), _pValue, _fDragSpeed, _iMin, _iMax); ImGui::TableNextRow();

	ImGui::PopItemWidth();

	return isResult;
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
	Safe_Release(m_pMeshModeCombo);
	Safe_Release(m_pSpriteTypeCombo);
	Safe_Release(m_pArcModeCombo);
	Safe_Release(m_pMeshCombo);
	Safe_Release(m_pConeEmitFromCombo);
	Safe_Release(m_pBoxEmitFromCombo);
	Safe_Release(m_pMeshTypeCombo);
	Safe_Release(m_pClipChannelCombo);
	Safe_Release(m_pAlphaTexture);
	Safe_Release(m_pSpriteTexture);
	Safe_Release(m_pMaterialTexture);
}