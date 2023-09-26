#include "..\Public\DummyParticle.h"
#include "GameInstance.h"
#include "Effect_Window.h"
#include "Window_Manager.h"
#include "ComboBox.h"
#include "ImageFileDialog.h"


CDummyParticle::CDummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CParticleSystem(_pDevice, _pContext)
{
}
CDummyParticle::CDummyParticle(const CDummyParticle& _rhs)
	: CParticleSystem(_rhs)
	, vShapePosition(_rhs.vShapePosition)
	, vShapeRotation(_rhs.vShapeRotation)
	, vShapeScale(_rhs.vShapeScale)
{
}
HRESULT CDummyParticle::Initialize_Prototype(const _tchar* _pDirectoryPath)
{
	if (FAILED(__super::Initialize_Prototype(L"", LEVEL_TOOL)))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}
HRESULT CDummyParticle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pEmitterVelocity_ComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Emission Velocity", { "RigidBody", "Transform" });
	m_pShapeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Shape", { "Sphere", "Cone", "Circle", "Box", "Mesh", "Sprite", "Edge", "Rectangle" }, "Sphere");
	m_pMeshModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong" });
	m_pSpriteTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pThetaModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong", "Burst_Spread" });
	m_pPhiModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong", "Burst_Spread" });
	m_pMeshCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Mesh", { "Cube", "Capsule", "Capsule", "Cylinder", "Plane", "Sphere", "Quad" });
	m_pConeEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Base", "Volume" });
	m_pBoxEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Volume", "Shell", "Edge" });
	m_pMeshTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pStopActionCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Stop Action", { "None", "Disable", "Destroy", "Callback" }, "None");
	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "ClipChannel", { "Red", "Green", "Blue", "Alpha" }, "Alpha");
	m_pClipChannelCombo->Set_StartTag(m_ShapeModuleDesc.strClipChannel.data());
	vector<string> Passes = m_pShader->Get_PassList();
	m_pPassComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Pass", Passes, Passes.front().data());
	m_pColorEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Easing", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pSizeXEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingX", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pSizeYEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingY", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pSizeZEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingZ", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pAngularVelocityCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Option", { "Constant", "Range" }, "Constant");
	m_pNoiseStrengthOptionComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Strength Option", { "Constant", "Range", "Curve"}, "Constant");
	m_pNoiseStrengthCurveEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Easing", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//CComboBox::Create(Generate_Hashtag(true).data(), "xvclk", CEase::asdfED, "vsd");

	m_pAlphaTextureIFD = CImageFileDialog::Create(m_pDevice, "SelectTexture2D");
	m_pAlphaTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/";
	m_pAlphaTextureIFD->m_iImageButtonWidth = 32;

	m_pSpriteTextureIFD = CImageFileDialog::Create(m_pDevice, "SpriteImageDialog");
	m_pSpriteTextureIFD->m_strStartPath = "../../Resources/Default/Textures/Particles/";
	m_pSpriteTextureIFD->m_iImageButtonWidth = 32;

	m_pMaterialTextureIFD = CImageFileDialog::Create(m_pDevice, "MainTextureDialog");
	m_pMaterialTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/";
	m_pMaterialTextureIFD->m_iImageButtonWidth = 32;

	m_pNormalTextureIFD = CImageFileDialog::Create(m_pDevice, "NormalTextureDialog");
	m_pNormalTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/Flipbooks/";
	m_pNormalTextureIFD->m_iImageButtonWidth = 32;

	m_pGradientTextureIFD = CImageFileDialog::Create(m_pDevice, "GradientTextureDialog");
	m_pGradientTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/Gradients/";
	m_pGradientTextureIFD->m_iImageButtonWidth = 32;
	

	Load_After();
	return S_OK;
}
HRESULT CDummyParticle::Render()
{
	return __super::Render();
}

void CDummyParticle::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

	MainMoudle_TreeNode(pEffectWindow);
	ImGui::Separator();
	EmissionModule_TreeNode(pEffectWindow);
	ImGui::Separator();
	ShapeModule_TreeNode(pEffectWindow);
	ImGui::Separator();
	RotationOverLifetimeModule_TreeNode(pEffectWindow);
	ImGui::Separator();
	ColorOverLifeTime_TreeNode(pEffectWindow);
	ImGui::Separator();
	SizeOverLifeTime_TreeNode(pEffectWindow);
	ImGui::Separator();
	TextureSheetAnimationModule_TreeNode(pEffectWindow);
	ImGui::Separator();
	RendererModule_TreeNode(pEffectWindow);
	ImGui::Separator();
	Save_FileDialog();
	Load_FileDialog();
	ImGui::Separator();

	if (ImGui::Button("Play"))
		Play(m_pTransform->Get_Position());
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
		Stop();
	ImGui::SameLine();
	if (ImGui::Button("Reset"))
		Reset_AllParticles();

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
	_float4x4 WorldMatirx = Get_Transform()->Get_WorldMatrix();
	pEffectWindow->MatrixNode(&WorldMatirx, "Effect_Transform", "Effect_Position", "Effect_Rotation", "Effect_Scale");
	Get_Transform()->Set_WorldMatrix(WorldMatirx);

}
void CDummyParticle::MainMoudle_TreeNode(CEffect_Window* pEffectWindow)
{
	if (ImGui::Checkbox("##MainModule_CheckBox", &m_MainModuleDesc.isActivate))
	{
		m_MainModuleDesc.isActivate = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("MainModule"))
	{
		if (ImGui::BeginTable("MainModuleTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_DragFloat("Duration", "ciovj380238uf", &m_MainModuleDesc.fDuration);
			pEffectWindow->Table_CheckBox("Looping", "eu9v73429hc", &m_MainModuleDesc.isLooping);

			if (true == m_MainModuleDesc.isLooping)
			{
				pEffectWindow->Table_CheckBox("Prewarm", "cv8yu893cnns", &m_MainModuleDesc.isPrewarm, false);
			}

			if (false == m_MainModuleDesc.isPrewarm)
			{
				pEffectWindow->Table_DragFloatWithOption("Start Delay", "xclvijowiejfc", &m_MainModuleDesc.fStartDelay, &m_MainModuleDesc.vStartDelayRange, &m_MainModuleDesc.isStartDelayRange);
			}
			pEffectWindow->Table_DragFloatWithOption("Start Lifetime", "xcklvjiej", &m_MainModuleDesc.fStartLifeTime, &m_MainModuleDesc.vStartLifeTimeRange, &m_MainModuleDesc.isStartLifeTimeRange);
			pEffectWindow->Table_DragFloatWithOption("Start Speed", "cvi93u7yfvrub", &m_MainModuleDesc.fStartSpeed, &m_MainModuleDesc.vStartSpeedRange, &m_MainModuleDesc.isStartSpeedRange);
			//Table_DragFloat2Range("Start Speed", "cvi93u7yfvrub", );
			pEffectWindow->Table_CheckBox("3D Start Size", "sdfvu89ywr978h", &m_MainModuleDesc.is3DStartSize);
			if (true == m_MainModuleDesc.is3DStartSize)
			{
				pEffectWindow->Table_DragXYZ("3D Size", "1w978h9f8vhjejf", &m_MainModuleDesc.v3DSizeXYZ);
			}
			else
			{
				pEffectWindow->Table_DragFloatWithOption("Start Size", "XCV89024890R0WRFOU", &m_MainModuleDesc.fStartSize, &m_MainModuleDesc.vStartSizeRange, &m_MainModuleDesc.isStartSizeRange);
			}

			pEffectWindow->Table_CheckBox("DirectionRotaion", "ckjv8ijie9342", &m_MainModuleDesc.isDirectionRotation);

			//pEffectWindow->Table_CheckBox("3D Start Rotation", "vbe088030j45", &m_MainModuleDesc.is3DStartAngle);
			if (true == m_MainModuleDesc.is3DStartAngle)
			{
				pEffectWindow->Table_DragXYZ("3D Rotation", "vwsdovuihe90f834", &m_MainModuleDesc.vStartAngle3D, 0.01f, 0.f, 360.f, false);
			}
			else
			{
				pEffectWindow->Table_DragFloatWithOption("Start Rotation", "dfvnko89h712enuo", &m_MainModuleDesc.fStartAngle, &m_MainModuleDesc.vStartAngleRange, &m_MainModuleDesc.isStartAngleRange, 0.1f, 0.f, 360.f);
			}

			pEffectWindow->Table_DragFloat("Flip Rotation", "xcv ioiw", &m_MainModuleDesc.fFlipAngle, 0.01f, 0.f, 1.f);

			pEffectWindow->Table_ColorEdit4("Start Color", "ergop805", &m_MainModuleDesc.vStartColor);
			pEffectWindow->Table_DragFloat("GravityModifier", "g50j8dfbji0", &m_MainModuleDesc.fGravityModifier, 0.01f, -FLT_MAX, FLT_MAX);
			pEffectWindow->Table_DragFloat("SimulationSpeed", "a1ip40c854dfg", &m_MainModuleDesc.fSimulationSpeed);
			pEffectWindow->Table_CheckBox("Play On Awake*", "zxci0pj380uj", &m_MainModuleDesc.isPlayOnAwake);
			m_MainModuleDesc.strEmmiterVelocity = m_pEmitterVelocity_ComboBox->Tick(CComboBox::FLAG::TABLE, false);
			if (pEffectWindow->Table_DragInt("Max Particles", "cv883diicvxcv", &m_MainModuleDesc.iMaxParticles, 0.6f, 1, MAX_PARTICLE_NUM))
			{
				Resize_Container(m_MainModuleDesc.iMaxParticles);
				RemakeBuffer(m_MainModuleDesc.iMaxParticles);
			}
			m_MainModuleDesc.strStopAction = m_pStopActionCombo->Tick(CComboBox::TABLE);

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CDummyParticle::EmissionModule_TreeNode(CEffect_Window* pEffectWindow)
{
	if (ImGui::Checkbox("##EmissionModule_CheckBox", &m_EmissionModuleDesc.isActivate))
	{
		// 이미션 노드는 항상 참이어야 함.
		m_EmissionModuleDesc.isActivate = true;
	}

	ImGui::SameLine();
	if (ImGui::TreeNode("EmissionModule"))
	{
		if (ImGui::BeginTable("EmissionModuleTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_DragFloat("RateOverTime", "v09u0u6oij4io", &m_EmissionModuleDesc.fRateOverTime);
			pEffectWindow->Table_DragFloat("RateOverDistance", "cio7 7v7e4i", &m_EmissionModuleDesc.fRateOverDistance);
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
			for (_uint i = 0; i < m_EmissionModuleDesc.Bursts.size(); ++i)
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
				ImGui::DragFloat(strTag[0].data(), &m_EmissionModuleDesc.Bursts[i].fTime, 0.f, FLT_MAX);

				ImGui::TableSetColumnIndex(1);
				ImGui::DragIntRange2(strTag[1].data(), &m_EmissionModuleDesc.Bursts[i].iCount.x
					, &m_EmissionModuleDesc.Bursts[i].iCount.y, 0.1f, 0, INT_MAX);

				ImGui::TableSetColumnIndex(2);
				ImGui::DragInt(strTag[2].data(), &m_EmissionModuleDesc.Bursts[i].iCycles, 0.1f, 1, INT_MAX);

				ImGui::TableSetColumnIndex(3);
				ImGui::DragFloat(strTag[3].data(), &m_EmissionModuleDesc.Bursts[i].fInterval, 0.1f, 0.f, m_EmissionModuleDesc.Bursts[i].fTime - 0.001f);

				ImGui::TableSetColumnIndex(4);
				ImGui::DragFloat(strTag[4].data(), &m_EmissionModuleDesc.Bursts[i].fProbability, 0.1f, 0.f, 1.f);
				ImGui::TableNextRow();

				ImGui::PopItemWidth();
			}
			ImGui::EndTable();

			if (ImGui::Button("Create##BurstsPushBack"))
			{
				m_EmissionModuleDesc.Bursts.push_back(EMISSION_MODULE::BURST());
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete##BurstsPopBack"))
			{
				if (false == m_EmissionModuleDesc.Bursts.empty())
				{
					m_EmissionModuleDesc.Bursts.pop_back();
				}
			}

		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CDummyParticle::ShapeModule_TreeNode(CEffect_Window* pEffectWindow)
{
	if (ImGui::Checkbox("##ShapeModule_CheckBox", &m_ShapeModuleDesc.isActivate))
	{
		// 셰이프 노드는 항상 참이어야 함.
		m_ShapeModuleDesc.isActivate = true;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("ShapeModule"))
	{
		if (ImGui::BeginTable("ShapeModuleTable", 2))
		{
			ImGui::TableNextRow();

			// 셰이프
			m_ShapeModuleDesc.strShape = m_pShapeCombo->Tick(CComboBox::FLAG::TABLE);
			string strShape = m_ShapeModuleDesc.strShape;
			//-------------------------------------------------------

			if (strShape == "Box")
				m_ShapeModuleDesc.strBoxEmitFrom = m_pBoxEmitFromCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Mesh")
			{
				m_pMeshTypeCombo->Tick(CComboBox::FLAG::TABLE);
				m_pMeshModeCombo->Tick(CComboBox::FLAG::TABLE);
				m_pMeshCombo->Tick(CComboBox::FLAG::TABLE);
				pEffectWindow->Table_CheckBox("SingleMaterial", "XCVVO0UI89HJ", &m_ShapeModuleDesc.isSingleMaterial);
				pEffectWindow->Table_CheckBox("UseMeshColors", "DCSKOLPJF087983", &m_ShapeModuleDesc.isUseMeshColors);
			}

			if (strShape == "Sprite")
				m_pSpriteTypeCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Sprite")
				pEffectWindow->Table_ImageButton("Sprite", "zxcvouiorvj39dxcv", m_pSpriteTextureIFD);

			if (strShape == "Mesh" || strShape == "Sprite")
				pEffectWindow->Table_DragFloat("NormalOffset", "SDFPI48083X", &m_ShapeModuleDesc.fNormalOffset);

			pEffectWindow->Table_Void();

			if (strShape == "Sphere" || strShape == "Circle")
			{
				pEffectWindow->Table_DragFloatWithOption("Length", "vxckeiic93dk", &m_ShapeModuleDesc.vLength.y, &m_ShapeModuleDesc.vLength, &m_ShapeModuleDesc.isLengthRange);
			}

			if (strShape == "Sphere")
			{
				pEffectWindow->Table_DragFloatWithOption("Phi", "909082ccvijdf", &m_ShapeModuleDesc.vPhi.y, &m_ShapeModuleDesc.vPhi, &m_ShapeModuleDesc.isPhiRange, 0.9f, 0.f, 360.f);
				m_ShapeModuleDesc.strPhiMode = m_pPhiModeCombo->Tick(CComboBox::FLAG::TABLE);
				pEffectWindow->Table_DragFloat("   Spread", "cljkuo838", &m_ShapeModuleDesc.fPhiSpread, 0.01f, 0.f, 1.f);
				if ("Loop" == m_ShapeModuleDesc.strPhiMode || "Ping-Pong" == m_ShapeModuleDesc.strPhiMode)
				{
					pEffectWindow->Table_DragFloat("Phi Interval", "xcvkl2309ck", &m_ShapeModuleDesc.fPhiInterval);
				}

				pEffectWindow->Table_Void();
			}

			if (strShape == "Sphere" || strShape == "Circle")
			{
				pEffectWindow->Table_DragFloatWithOption("Theta", "XVBIO008DFB89", &m_ShapeModuleDesc.vTheta.y, &m_ShapeModuleDesc.vTheta, &m_ShapeModuleDesc.isThetaRange, 0.9f, 0.f, 360.f);
				m_ShapeModuleDesc.strThetaMode = m_pThetaModeCombo->Tick(CComboBox::FLAG::TABLE);
				pEffectWindow->Table_DragFloat("   Spread", "SCFVPIFV8989R", &m_ShapeModuleDesc.fThetaSpread, 0.01f, 0.f, 1.f);
				if ("Loop" == m_ShapeModuleDesc.strThetaMode || "Ping-Pong" == m_ShapeModuleDesc.strThetaMode)
				{
					pEffectWindow->Table_DragFloat("Theta Interval", "xcvkl2309ck", &m_ShapeModuleDesc.fThetaInterval);
				}
			}

			///////

			if (strShape == "Cone")
			{
				pEffectWindow->Table_DragFloat("Angle", "dkkv994mc", &m_ShapeModuleDesc.fAngle, 0.1f, 0.f, 90.f);
				pEffectWindow->Table_DragFloat("Radius", "xvcxcvkljjkei", &m_ShapeModuleDesc.fBaseRadius);
				pEffectWindow->Table_DragFloat("RadiusThickness", "ckv89389kck", &m_ShapeModuleDesc.fRadiusThickness, 0.01f, 0.f, 1.f);

				if (m_ShapeModuleDesc.strConeEmitFrom == "Volume")
					pEffectWindow->Table_DragFloat("Length", "cllo390ckxz", &m_ShapeModuleDesc.fConeLength);
			}

			if (strShape == "Cone")
			{
				m_ShapeModuleDesc.strConeEmitFrom = m_pConeEmitFromCombo->Tick(CComboBox::FLAG::TABLE);
			}

			if (strShape == "Sphere")
				pEffectWindow->Table_DragFloat("Phi Interval", "xclvk039icik", &m_ShapeModuleDesc.fPhiInterval);

			if ("Sphere" == strShape || "Cone" == strShape || "Circle" == strShape)
				pEffectWindow->Table_DragFloat("Theta Interval", "sxcdjmim85", &m_ShapeModuleDesc.fThetaInterval);

			if ("Circle" == strShape)
			{
				pEffectWindow->Table_CheckBox("CameraAxis", "ckjv939jdjcvcsxv", &m_ShapeModuleDesc.isCameraAxis);
			}

			// ---------------------------------------------------------------
			pEffectWindow->Table_Void();

			pEffectWindow->Table_ImageButton("Texture", "XCVKPVUIIUE", m_pAlphaTextureIFD);
			if (m_pAlphaTextureIFD->IsOk())
			{
				string strFilePath = m_pAlphaTextureIFD->Get_FilePathName();
				fs::path fsFilePath = ToRelativePath(strFilePath.data());
				ChangeTexture(&m_pClipTexture, m_ShapeModuleDesc.wstrClipTexturePath, fsFilePath.wstring().data());
			}

			m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
			if (m_pClipChannelCombo->IsUpdated())
				m_ShapeModuleDesc.strClipChannel = m_pClipChannelCombo->Get_Current_Item();

			pEffectWindow->Table_DragFloat("Clip Threshold", "XC08VB890890T4", &m_ShapeModuleDesc.fClipThreshold, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_CheckBox("Color affects Particles", "D3UIOF79H79EFB", &m_ShapeModuleDesc.isColorAffectsParticles);
			pEffectWindow->Table_CheckBox("Alpha affects Particles", "ZXCIUFV7980RV890", &m_ShapeModuleDesc.isAlphaAffectsParticles);
			pEffectWindow->Table_CheckBox("Bilinear Filtering", "X2EN90UZX3792E79FH", &m_ShapeModuleDesc.isBilinearFiltering);

			pEffectWindow->Table_Void();

			if (pEffectWindow->Table_DragXYZ("Position", "ZXCOUIV834DCV", &vShapePosition))
			{
				_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(vShapePosition);
				_float4x4 RotationMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(vShapeRotation.x),
					XMConvertToRadians(vShapeRotation.y),
					XMConvertToRadians(vShapeRotation.z)));
				_float4x4 ScaleMatrix = _float4x4::MatrixScale(vShapeScale);
				m_ShapeModuleDesc.ShapeMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;
			}

			if (pEffectWindow->Table_DragXYZ("Rotation", "vioc4ijdfg", &vShapeRotation))
			{
				_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(vShapePosition);
				_float4x4 RotationMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(vShapeRotation.x),
					XMConvertToRadians(vShapeRotation.y),
					XMConvertToRadians(vShapeRotation.z)));
				_float4x4 ScaleMatrix = _float4x4::MatrixScale(vShapeScale);
				m_ShapeModuleDesc.ShapeMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;


			}
			if (pEffectWindow->Table_DragXYZ("Scale", "xciv8348kd", &vShapeScale))
			{
				_float4x4 TranslationMatrix = _float4x4::MatrixTranslation(vShapePosition);
				_float4x4 RotationMatrix = _float4x4::MatrixFromQuaternion(XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(vShapeRotation.x),
					XMConvertToRadians(vShapeRotation.y),
					XMConvertToRadians(vShapeRotation.z)));
				_float4x4 ScaleMatrix = _float4x4::MatrixScale(vShapeScale);
				m_ShapeModuleDesc.ShapeMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;
			}

			pEffectWindow->Table_Void();

			pEffectWindow->Table_DragFloat("Randomize Direction", "XC08xcsd890890T4", &m_ShapeModuleDesc.fRandomizeDirection, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_DragFloat("Spherize Direction", "XC08wceV90cvT4", &m_ShapeModuleDesc.fSpherizeDirection, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_DragFloat("Randomize Position", "xcf8u8vceV90", &m_ShapeModuleDesc.fRandomizePosition);

			pEffectWindow->Table_CheckBox("Chase", "ck939cikcke", &m_ShapeModuleDesc.isChase);

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CDummyParticle::RendererModule_TreeNode(CEffect_Window* pEffectWindow)
{
	if (ImGui::Checkbox("##RendererModule_CheckBox", &m_RendererModuleDesc.isActivate))
	{
		// 렌더러 노드는 항상 참이어야 함.
		m_RendererModuleDesc.isActivate = true;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("RendererModule"))
	{
		if (ImGui::BeginTable("RendererModuleTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_ImageButton("Material", "xcvi3489009dv", m_pMaterialTextureIFD);
			if (m_pMaterialTextureIFD->IsOk())
			{
				string strFilePath = m_pMaterialTextureIFD->Get_FilePathName();
				fs::path fsFilePath = ToRelativePath(strFilePath.data());
				ChangeTexture(&m_pMainTexture, m_RendererModuleDesc.wstrMaterialPath, fsFilePath.wstring().data());
				// ../../Resources/Effect/Default_Particle.png;
				////////
			}

			pEffectWindow->Table_CheckBox("Billboard DeleteY", "CJCJV8389kdjd", &m_RendererModuleDesc.isDeleteY);

			pEffectWindow->Table_CheckBox("Use Gradient Texture", "lkcxvjjlef8", &m_RendererModuleDesc.isUseGradientTexture);
			if (true == m_RendererModuleDesc.isUseGradientTexture)
			{
				pEffectWindow->Table_ImageButton("Gradient", "jksdfkjlvioife", m_pGradientTextureIFD);
				if (m_pGradientTextureIFD->IsOk())
				{
					string strFilePath = m_pGradientTextureIFD->Get_FilePathName();
					fs::path fsFilePath = ToRelativePath(strFilePath.data());
					ChangeTexture(&m_pGradientTexture, m_RendererModuleDesc.wstrGraientTexture, fsFilePath.wstring().data());
					// ../../Resources/Effect/Default_Particle.png;
					////////
				}
			}

			m_RendererModuleDesc.strPass = m_pPassComboBox->Tick(CComboBox::TABLE);
			pEffectWindow->Table_CheckBox("GLOW", "KCXK939DJJZZZ", &m_RendererModuleDesc.isGlow);
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
void CDummyParticle::ColorOverLifeTime_TreeNode(CEffect_Window* pEffectWindow)
{
	ImGui::Checkbox("##ColorOverLifeTimeModule_CheckBox", &m_ColorOverLifeTimeModuleDesc.isActivate);

	if (false == m_ColorOverLifeTimeModuleDesc.isActivate)
	{
		ImGui::SameLine();
		ImGui::Text("     ColorOverLifeTimeModule");
		return;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("ColorOverLifeTimeModule"))
	{
		if (ImGui::BeginTable("ColorOverLifeTimeTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_ColorEdit4("Start Color", "xcvkljo8234", &m_ColorOverLifeTimeModuleDesc.vStartColor);
			pEffectWindow->Table_ColorEdit4("End Color", "dlkjiv993kdkc", &m_ColorOverLifeTimeModuleDesc.vEndColor);

			m_pColorEaseCombo->Tick(CComboBox::TABLE);
			if (m_pColorEaseCombo->IsUpdated())
				m_ColorOverLifeTimeModuleDesc.eEase = static_cast<CEase::EASE>(m_pColorEaseCombo->Get_Current_Item_Index());

			ImGui::EndTable();
		}

		ImGui::TreePop();
	}
}
void CDummyParticle::SizeOverLifeTime_TreeNode(CEffect_Window* pEffectWindow)
{
	ImGui::Checkbox("##SizeOverLifeTimeModule_CheckBox", &m_SizeOverLifeTimeModuleDesc.isActivate);

	if (false == m_SizeOverLifeTimeModuleDesc.isActivate)
	{
		ImGui::SameLine();
		ImGui::Text("     SizeOverLifeTimeModule");
		return;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("SizeOverLifeTimeModule"))
	{
		if (ImGui::BeginTable("SizeOverLifeTimeTable", 2))
		{
			ImGui::TableNextRow();
			pEffectWindow->Table_CheckBox("Separate Axes", "xcvklj3909di", &m_SizeOverLifeTimeModuleDesc.isSeparateAxes);

			pEffectWindow->Table_DragFloat2("SizeX", "XCVPIJDFKLJ983", &m_SizeOverLifeTimeModuleDesc.vSizeX, 0.01f);
			m_pSizeXEaseCombo->Tick(CComboBox::TABLE);
			if (m_pSizeXEaseCombo->IsUpdated())
				m_SizeOverLifeTimeModuleDesc.eEaseX = static_cast<CEase::EASE>(m_pSizeXEaseCombo->Get_Current_Item_Index());

			if (true == m_SizeOverLifeTimeModuleDesc.isSeparateAxes)
			{
				pEffectWindow->Table_DragFloat2("SizeY", "cvikj838jdfsdfsdf", &m_SizeOverLifeTimeModuleDesc.vSizeY, 0.01f);
				m_pSizeYEaseCombo->Tick(CComboBox::TABLE);
				if (m_pSizeYEaseCombo->IsUpdated())
					m_SizeOverLifeTimeModuleDesc.eEaseY = static_cast<CEase::EASE>(m_pSizeYEaseCombo->Get_Current_Item_Index());

				pEffectWindow->Table_DragFloat2("SizeZ", "zxcvhjyuwesfdwsd", &m_SizeOverLifeTimeModuleDesc.vSizeZ, 0.01f);
				m_pSizeZEaseCombo->Tick(CComboBox::TABLE);
				if (m_pSizeZEaseCombo->IsUpdated())
					m_SizeOverLifeTimeModuleDesc.eEaseZ = static_cast<CEase::EASE>(m_pSizeZEaseCombo->Get_Current_Item_Index());
			}

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
void CDummyParticle::RotationOverLifetimeModule_TreeNode(CEffect_Window* pEffectWindow)
{
	ImGui::Checkbox("##RotationOverLifetimeModule_CheckBox", &m_RotationOverLifetimeModuleDesc.isActivate);
	
	if (false == m_RotationOverLifetimeModuleDesc.isActivate)
	{
		ImGui::SameLine();
		ImGui::Text("     RotationOverLifetimeModule");
		return;
	}
	
	ImGui::SameLine();

	if (ImGui::TreeNode("RotationOverLifetimeModule"))
	{
		if (ImGui::BeginTable("RotationOverLifetimeTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_DragFloatWithOption("Angular Velocity", "vj93jf9jdfsd"
				, &m_RotationOverLifetimeModuleDesc.fAngularVelocity, &m_RotationOverLifetimeModuleDesc.vAngularVelocityRange
				, &m_RotationOverLifetimeModuleDesc.isAngularVelocityRange, 0.9f, -FLT_MAX);
			
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}

	ImGui::Spacing();
	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	//ImGui::Text("I recommend enabling the billboard option in the RendererModule.");
	//ImGui::PopStyleColor();

	
}
void CDummyParticle::TextureSheetAnimationModule_TreeNode(CEffect_Window* pEffectWindow)
{
	TEXTURE_SHEET_ANIMATION& TSAModule = m_TextureSheetAnimationModuleDesc;

	if (ImGui::Checkbox("##TextureSheetAnimationModule_CheckBox", &TSAModule.isActivate))
	{
		TSAModule.iWidthLength = 1;
		TSAModule.iHeightLength = 1;
	}
	
	if (false == TSAModule.isActivate)
	{
		ImGui::SameLine();
		ImGui::Text("     TextureSheetAnimationModule");
		return;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("TextureSheetAnimationModule"))
	{
		if (ImGui::BeginTable("TextureSheetAnimationTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_CheckBox("Animation", "ioivj992djfsdf", &TSAModule.isAnimation);

			// 열 길이
			if (pEffectWindow->Table_DragInt("Width Length", "cxvkj3999`9kskxk", (_int*)&TSAModule.iWidthLength))
			{
				TSAModule.CalculateMaxSize();
			}

			// 행 길이
			if (pEffectWindow->Table_DragInt("Height Length", "lckl9ecv9kkxc", (_int*)&TSAModule.iHeightLength))
			{
				TSAModule.CalculateMaxSize();
			}

			if (true == TSAModule.isAnimation)
			{
				pEffectWindow->Table_DragFloatWithOption("Start Frame", "vkjicjijsiji3c9", &TSAModule.fStartFrame
					, &TSAModule.vStartFrameRange, &TSAModule.isStartFrameRange, 0.001f, 0.f, 1.f);

				pEffectWindow->Table_DragFloat("Update Interval", "kjkvic898e", &TSAModule.fUpdateInterval, 0.001f, 0.015f);
				pEffectWindow->Table_CheckBox("LoopOption", "ckjvidjf93sdf", &TSAModule.isLoopOption);

			}
			else
			{
				pEffectWindow->Table_CheckBox("Random Range", "c99k3kdiicjxv56", &TSAModule.isSeletedIndexRange);

				if (true == TSAModule.isSeletedIndexRange)
				{
					pEffectWindow->Table_DragInt2Range("Index Range", "kjcv993jdjjcmxmcv", (_int2*)&TSAModule.vSeletedIndexRange, 0.1f, 0, TSAModule.iMaxIndex);
				}
				else
				{
					pEffectWindow->Table_DragInt("Current Index", "k993jkdjchjus", (_int*)&TSAModule.iSeletedIndex);
				}
			}

			// 노말 텍스처 쓸건지 여부
			pEffectWindow->Table_CheckBox("Use Normal Texture", "vsavsr55", &TSAModule.isUseNormalTexture);
			if (true == TSAModule.isUseNormalTexture)
			{
				pEffectWindow->Table_ImageButton("Normal Texture", "cvbnvbnjk1234eh", m_pNormalTextureIFD);
				// 노말 텍스처 교체
				if (m_pNormalTextureIFD->IsOk())
				{
					fs::path fsFilePath = m_pNormalTextureIFD->Get_FilePathName();
					ChangeTexture(&m_pNormalTexture, TSAModule.wstrNormalPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
					TSAModule.Restart();
					TSAModule.CalculateMaxSize();
				}
			}

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
void CDummyParticle::NoiseModule_TreeNode(CEffect_Window* pEffectWindow)
{
	ImGui::Checkbox("##NoiseModule_CheckBox", &m_NoiseModuleDesc.isActivate);

	if (false == m_NoiseModuleDesc.isActivate)
	{
		ImGui::SameLine();
		ImGui::Text("     NoiseLifetimeModule");
		return;
	}

	ImGui::SameLine();

	if (ImGui::TreeNode("NoiseLifetimeModule"))
	{
		if (ImGui::BeginTable("NoiseLifetimeTable", 2))
		{
			ImGui::TableNextRow();

			pEffectWindow->Table_CheckBox("Separate Axes", "IVJ89IVJ99K3F9", &m_NoiseModuleDesc.isSeparateAxes);
			m_NoiseModuleDesc.strStrengthOption = m_pNoiseStrengthOptionComboBox->Tick(CComboBox::TABLE);
			if (false == m_NoiseModuleDesc.isSeparateAxes)
			{
				if ("Constant" == m_NoiseModuleDesc.strStrengthOption)
				{
					pEffectWindow->Table_DragFloat("Strength", "lxckjvm8939dxcxv", &m_NoiseModuleDesc.fStrength);
				}
				else if ("Range" == m_NoiseModuleDesc.strStrengthOption)
				{
					pEffectWindow->Table_DragFloat2Range("Strength", "iopjlkcj9923", &m_NoiseModuleDesc.vStrengthRange, 0.01f, -FLT_MAX);
				}
				else if ("Curve" == m_NoiseModuleDesc.strStrengthOption)
				{
					//m_pNoiseStrengthCurveEaseCombo->Tick(CComboBox::TABLE);
					//if (m_pNoiseStrengthCurveEaseCombo->IsUpdated())
					//	m_NoiseModuleDesc.eStrengthEaseX = m_pNoiseStrengthCurveEaseCombo->Get_Current_Item_Index();
				}
			}
			else
			{
				if ("Constant" == m_NoiseModuleDesc.strStrengthOption)
				{
					pEffectWindow->Table_DragXYZ("Strength", "v9c9cvk2kci", &m_NoiseModuleDesc.vStrength3DMax);
				}
				else if ("Range" == m_NoiseModuleDesc.strStrengthOption)
				{
					pEffectWindow->Table_DragXYZ("Strength", "v9c9cvk2kci", &m_NoiseModuleDesc.vStrength3DMin);
					pEffectWindow->Table_DragXYZ("Strength", "v9c9cvk2kci", &m_NoiseModuleDesc.vStrength3DMax);
				}
				else if ("Curve" == m_NoiseModuleDesc.strStrengthOption)
				{
					//m_pNoiseStrengthCurveEaseCombo->Tick(CComboBox::TABLE);
					//if (m_pNoiseStrengthCurveEaseCombo->IsUpdated())
					//	m_NoiseModuleDesc.eStrengthEaseX = m_pNoiseStrengthCurveEaseCombo->Get_Current_Item_Index();
				}
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
void CDummyParticle::Save_FileDialog()
{
	if (ImGui::Button("Save Particle"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseSaveFilePtcKey", "Save File", ".ptc", "../../Resources/GameData/ParticleData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseSaveFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			fs::path fsFilePath = ToRelativePath(filePath.data());
			if (FAILED(__super::Save(fsFilePath.wstring().data())))
			{
				MSG_BOX("Failed to Saved CDummyParticle");
				return;
			}

			MSG_BOX("Success to Saved CDummyParticle");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}
void CDummyParticle::Load_FileDialog()
{
	if (ImGui::Button("Load Particle"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseLoadFilePtcKey", "Load File", ".ptc", "../../Resources/GameData/ParticleData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseLoadFilePtcKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			fs::path fsFilePathName = filePathName;
			fs::path fsFilePath = filePath;
			fsFilePath = ToRelativePath(fsFilePath.wstring().data());
			_ulong dwByte = 0;

			if (FAILED(Load(fsFilePath.wstring().c_str())))
			{
				MSG_BOX("Failed to loaded file");
			}
			else
			{
				Load_After();
				MSG_BOX("The file has been loaded successfully");
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}
void CDummyParticle::Load_After()
{
	Restart();
	m_ShapeModuleDesc.ShapeMatrix.Decompose(vShapeScale, vShapeQuaternion, vShapePosition);
	vShapeRotation = QuaternionToEuler(vShapeQuaternion);
	RemakeBuffer(m_MainModuleDesc.iMaxParticles);
	Resize_Container(m_MainModuleDesc.iMaxParticles);
	m_pShapeCombo->Update_Current_Item(m_ShapeModuleDesc.strShape);
	m_pMeshModeCombo->Update_Current_Item(m_ShapeModuleDesc.strMeshTypeMode);
	m_pMeshTypeCombo->Update_Current_Item(m_ShapeModuleDesc.strMeshType);
	m_pMeshCombo->Update_Current_Item(m_ShapeModuleDesc.strMesh);
	m_pStopActionCombo->Update_Current_Item(m_MainModuleDesc.strStopAction);
	m_pPhiModeCombo->Update_Current_Item(m_ShapeModuleDesc.strPhiMode);
	m_pThetaModeCombo->Update_Current_Item(m_ShapeModuleDesc.strThetaMode);
	m_pClipChannelCombo->Update_Current_Item(m_ShapeModuleDesc.strClipChannel);
	m_pPassComboBox->Update_Current_Item(m_RendererModuleDesc.strPass);
	//m_pColorEaseCombo->Update_Current_Item(M_)
	//m_pSizeXEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingX", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//m_pSizeYEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingY", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//m_pSizeZEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingZ", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//m_pAngularVelocityXEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingX", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//m_pAngularVelocityYEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingY", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	//m_pAngularVelocityZEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "EasingZ", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);


	ChangeTexture(&m_pMainTexture, m_RendererModuleDesc.wstrMaterialPath, ToRelativePath(m_RendererModuleDesc.wstrMaterialPath.c_str()).c_str());
	ChangeTexture(&m_pClipTexture, m_ShapeModuleDesc.wstrClipTexturePath, ToRelativePath(m_ShapeModuleDesc.wstrClipTexturePath.c_str()).c_str());
	ChangeTexture(&m_pNormalTexture, m_TextureSheetAnimationModuleDesc.wstrNormalPath, ToRelativePath(m_TextureSheetAnimationModuleDesc.wstrNormalPath.c_str()).c_str());
	m_TextureSheetAnimationModuleDesc.CalculateMaxSize();


	//m_pMaterialTextureIFD->ChangeTexture(wstrToStr(m_RendererModuleDesc.wstrMaterialPath).data());
//m_pAlphaTextureIFD->ChangeTexture(wstrToStr(m_ShapeModuleDesc.wstrClipTexturePath).data());
//m_pSpriteTypeCombo->Update_Current_Item(m_ShapeModuleDesc.str);
//m_pConeEmitFromCombo->Update_Current_Item(m_MainModuleDesc.str);
//m_pBoxEmitFromCombo->Update_Current_Item(m_ShapeModuleDesc.strThetaMode);
}
void CDummyParticle::Restart()
{
	Reset_AllParticles();
	__super::Restart();
}
void CDummyParticle::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
{
	BEGININSTANCE;
	// 소유하고 있는 컴포넌트를 지운다.

	_tchar* wszTag = pGameInstance->Make_WChar((*_pTexture)->Get_Tag());

	if (FAILED(CComposite::Delete_Component(wszTag)))
	{
		MSG_BOX("Failed to Delete");
		ENDINSTANCE;
		E_FAIL;
	}

	Safe_Release(*_pTexture);

	// 찾는 컴포넌트가 없으면 원본을 만들어준다.
	wstring tempTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), _pDestPath).data();
	_tchar* pTag = { nullptr };
	m_pTags.push_back(pTag = new _tchar[tempTag.length() + 1]);
	lstrcpy(pTag, tempTag.c_str());

	// 기존에 컴포넌트가 존재하는지 확인
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_TOOL, pTag))
	{

		// 없다면 새로운 프로토타입 컴포넌트 생성
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, pTag, CTexture::Create(m_pDevice
			, m_pContext, _pDestPath))))
		{
			MSG_BOX("Failed to Create Prototype : Change Texture");
		}
	}

	// 새로운 텍스처 컴포넌트를 클론한다.

	_wstrOriginPath = _pDestPath;
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, pTag
		, wszTag, reinterpret_cast<CComponent**>(_pTexture))))
	{
		MSG_BOX("Failed to Add Component");
		ENDINSTANCE;
		return;
	}
	ENDINSTANCE;
}
void CDummyParticle::RemakeBuffer(_uint iNumInstance)
{
	iNumInstance = min(iNumInstance, MAX_PARTICLE_NUM);
	m_pBuffer->Initialize(&iNumInstance);
	Restart();
}
CDummyParticle* CDummyParticle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath)
{
	CDummyParticle* pInstance = New CDummyParticle(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath)))
	{
		MSG_BOX("Faild to Created CDummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject* CDummyParticle::Clone(void* _pArg)
{
	CDummyParticle* pInstance = New CDummyParticle(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Faild to Created CDummyParticle");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CDummyParticle::Free(void)
{
	__super::Free();
	Safe_Release(m_pEmitterVelocity_ComboBox);
	Safe_Release(m_pShapeCombo);
	Safe_Release(m_pMeshModeCombo);
	Safe_Release(m_pSpriteTypeCombo);
	Safe_Release(m_pThetaModeCombo);
	Safe_Release(m_pPhiModeCombo);
	Safe_Release(m_pMeshCombo);
	Safe_Release(m_pConeEmitFromCombo);
	Safe_Release(m_pBoxEmitFromCombo);
	Safe_Release(m_pMeshTypeCombo);
	Safe_Release(m_pClipChannelCombo);
	Safe_Release(m_pStopActionCombo);
	Safe_Release(m_pAlphaTextureIFD);
	Safe_Release(m_pSpriteTextureIFD);
	Safe_Release(m_pMaterialTextureIFD);
	Safe_Release(m_pNormalTextureIFD);
	Safe_Release(m_pGradientTextureIFD);
	Safe_Release(m_pColorEaseCombo);
	Safe_Release(m_pSizeXEaseCombo);
	Safe_Release(m_pSizeYEaseCombo);
	Safe_Release(m_pSizeZEaseCombo);
	Safe_Release(m_pAngularVelocityCombo);
	Safe_Release(m_pNoiseStrengthOptionComboBox);
	Safe_Release(m_pNoiseStrengthCurveEaseCombo);
	Safe_Release(m_pPassComboBox);
	for (auto& pEaseCombo : m_pEaseCombo)
		Safe_Release(pEaseCombo);
	//for (auto& EaseComboBox : m_pColorEase)
	//{
	//	Safe_Release(EaseComboBox);
	//}

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}