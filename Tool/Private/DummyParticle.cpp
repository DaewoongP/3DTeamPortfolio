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
{
}

HRESULT CDummyParticle::Initialize_Prototype(const _tchar* _pDirectoryPath)
{
	if (FAILED(__super::Initialize_Prototype(_pDirectoryPath, LEVEL_TOOL)))
		return E_FAIL;

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

HRESULT CDummyParticle::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	m_pEmitterVelocity_ComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Emission Velocity", { "RigidBody", "Transform" });
	m_pShapeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Shape", { "Sphere", "HemiSphere", "Cone", "Donut", "Box", "Mesh", "Sprite", "Circle", "Edge", "Rectangle" });
	m_pMeshModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong" });
	m_pSpriteTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pArcModeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "   Mode", { "Random", "Loop", "Ping-Pong, Burst_Spread" });
	m_pMeshCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Mesh", { "Cube", "Capsule", "Capsule", "Cylinder", "Plane", "Sphere", "Quad" });
	m_pConeEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Base", "Volume" });
	m_pBoxEmitFromCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emit From", { "Volume", "Shell", "Edge" });
	m_pMeshTypeCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Type", { "Vertex", "Edge", "Triangle" });
	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "ClipChannel", { "Red", "Green", "Blue", "Alpha" }, "Alpha");
	m_pClipChannelCombo->Set_StartTag(m_ShapeModuleDesc.strClipChannel.data());

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

void CDummyParticle::Tick_Imgui(_float _fTimeDelta)
{
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
}
void CDummyParticle::MainMoudle_TreeNode()
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

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
			if (m_isPrevLooping != m_MainModuleDesc.isLooping)
			{
				Restart();
				m_isPrevLooping = m_MainModuleDesc.isLooping;
			}

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
				pEffectWindow->Table_DragXYZ("3D Size", "1w978h9f8vhjejf", &m_MainModuleDesc.f3DSizeXYZ);
			}
			else
			{
				pEffectWindow->Table_DragFloatWithOption("Start Size", "XCV89024890R0WRFOU", &m_MainModuleDesc.fStartSize, &m_MainModuleDesc.vStartSizeRange, &m_MainModuleDesc.isStartSizeRange);
			}
			pEffectWindow->Table_CheckBox("3D Start Rotation", "vbe088030j45", &m_MainModuleDesc.is3DStartRotation);
			if (true == m_MainModuleDesc.is3DStartRotation)
			{
				pEffectWindow->Table_DragXYZ("3D Rotation", "vwsdovuihe90f834", &m_MainModuleDesc.f3DRotationXYZ, 0.01f, 0.f, 360.f, false);
			}
			else
			{
				pEffectWindow->Table_DragFloatWithOption("Start Rotation", "dfvnko89h712enuo", &m_MainModuleDesc.fStartRotation, &m_MainModuleDesc.vStartRotationRange, &m_MainModuleDesc.isStartRotationRange, 0.1f, 0.f, 360.f);
			}

			pEffectWindow->Table_DragFloat("Flip Rotation", "xcv ioiw", &m_MainModuleDesc.fFlipRotation, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_ColorEdit4("Start Color", "ergop805", &m_MainModuleDesc.vStartColor);
			pEffectWindow->Table_DragFloat("GravityModifier", "g50j8dfbji0", &m_MainModuleDesc.fGravityModifier);
			pEffectWindow->Table_DragFloat("SimulationSpeed", "a1ip40c854dfg", &m_MainModuleDesc.fSimulationSpeed);
			pEffectWindow->Table_CheckBox("Play On Awake*", "zxci0pj380uj", &m_MainModuleDesc.isPlayOnAwake);
			m_MainModuleDesc.strEmmiterVelocity = m_pEmitterVelocity_ComboBox->Tick(CComboBox::FLAG::TABLE, false);
			if (pEffectWindow->Table_DragInt("Max Particles", "cv883diicvxcv", &m_MainModuleDesc.iMaxParticles, 0.6f, 1, MAX_PARTICLE_NUM))
			{
				RemakeBuffer(m_MainModuleDesc.iMaxParticles);
			}

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CDummyParticle::EmissionModule_TreeNode()
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

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
				ImGui::DragFloat(strTag[0].data(), &m_EmissionModuleDesc.Bursts[i].fTime);

				ImGui::TableSetColumnIndex(1);
				ImGui::DragIntRange2(strTag[1].data(), &m_EmissionModuleDesc.Bursts[i].iCount.x
					, &m_EmissionModuleDesc.Bursts[i].iCount.y, 0.1f, 0, INT_MAX);

				ImGui::TableSetColumnIndex(2);
				ImGui::DragInt(strTag[2].data(), &m_EmissionModuleDesc.Bursts[i].iCycles, 0.1f, 0, INT_MAX);

				ImGui::TableSetColumnIndex(3);
				ImGui::DragFloat(strTag[3].data(), &m_EmissionModuleDesc.Bursts[i].fInterval, 0.1f, 0, m_EmissionModuleDesc.Bursts[i].fTime - 0.001f);

				ImGui::TableSetColumnIndex(4);
				ImGui::DragFloat(strTag[4].data(), &m_EmissionModuleDesc.Bursts[i].fProbability, 0.1f, 0, 1.f);
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
void CDummyParticle::ShapeModule_TreeNode()
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

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
			//--------------------------------------------------

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
				pEffectWindow->Table_ImageButton("Sprite", "zxcvouiorvj39dxcv", m_pSpriteTexture);

			if (strShape == "Mesh" || strShape == "Sprite")
				pEffectWindow->Table_DragFloat("NormalOffset", "SDFPI48083X", &m_ShapeModuleDesc.fNormalOffset);

			if (strShape == "Cone")
				pEffectWindow->Table_DragFloat("Angle", "QWD0U8I38F", &m_ShapeModuleDesc.fAngle);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle" || strShape == "Edge")
				pEffectWindow->Table_DragFloat("Radius", "DVIFV809082CV", &m_ShapeModuleDesc.fRadius);

			if (strShape == "Donut")
				pEffectWindow->Table_DragFloat("DonutRadius", "WSDFVIO0H13890802", &m_ShapeModuleDesc.fDonutRadius);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				pEffectWindow->Table_DragFloat("RadiusThickness", "1AOUYU89AYIXC78", &m_ShapeModuleDesc.fRadiusThickness, 0.01f, 0.f, 1.f);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				pEffectWindow->Table_DragFloat("Arc", "XVBIO008DFB89", &m_ShapeModuleDesc.fArc, 0.9f, 0.f, 360.f);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				m_pArcModeCombo->Tick(CComboBox::FLAG::TABLE);

			if (strShape == "Sphere" || strShape == "HemiSphere" || strShape == "Cone"
				|| strShape == "Donut" || strShape == "Circle")
				pEffectWindow->Table_DragFloat("   Spread", "SCFVPIFV8989R", &m_ShapeModuleDesc.fSpread, 0.01f, 0.f, 1.f);

			if (strShape == "Cone" && m_ShapeModuleDesc.strConeEmitFrom == "Base")
				pEffectWindow->Table_DragFloat("Length", "diovoirdmcmxjk", &m_ShapeModuleDesc.fLength);

			if (strShape == "Cone")
				m_pConeEmitFromCombo->Tick(CComboBox::FLAG::TABLE);

			// ---------------------------------------------------------------
			pEffectWindow->Table_Void();

			pEffectWindow->Table_ImageButton("Texture", "XCVKPVUIIUE", m_pAlphaTexture);
			if (m_pAlphaTexture->IsOk())
			{
				string strFilePath = m_pAlphaTexture->Get_FilePathName();
				fs::path fsFilePath = ToRelativePath(strFilePath.data());
				//ChangeTexture(fsFilePath.wstring().data());
			}
			m_ShapeModuleDesc.strClipChannel = m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
			pEffectWindow->Table_DragFloat("Clip Threshold", "XC08VB890890T4", &m_ShapeModuleDesc.fClipThreshold, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_CheckBox("Color affects Particles", "D3UIOF79H79EFB", &m_ShapeModuleDesc.isColorAffectsParticles);
			pEffectWindow->Table_CheckBox("Alpha affects Particles", "ZXCIUFV7980RV890", &m_ShapeModuleDesc.isAlphaAffectsParticles);
			pEffectWindow->Table_CheckBox("Bilinear Filtering", "X2EN90UZX3792E79FH", &m_ShapeModuleDesc.isBilinearFiltering);

			pEffectWindow->Table_Void();

			pEffectWindow->Table_DragXYZ("Position", "ZXCOUIV834DCV", &m_ShapeModuleDesc.vPosition);
			pEffectWindow->Table_DragXYZ("Rotation", "vioc4ijdfg", &m_ShapeModuleDesc.vRotation);
			pEffectWindow->Table_DragXYZ("Scale", "xciv8348kd", &m_ShapeModuleDesc.vScale);

			pEffectWindow->Table_Void();

			pEffectWindow->Table_DragFloat("Randomize Direction", "XC08xcsd890890T4", &m_ShapeModuleDesc.fRandomizeDirection, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_DragFloat("Spherize Direction", "XC08wceV90cvT4", &m_ShapeModuleDesc.fSpherizeDirection, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_DragFloat("Randomize Position", "xcf8u8vceV90", &m_ShapeModuleDesc.fRandomizePosition);

			ImGui::EndTable();
		}

		ImGui::TreePop(); // SubNode의 끝
	}
}
void CDummyParticle::RendererModule_TreeNode()
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

	if (ImGui::Checkbox("##ShapeModule_CheckBox", &m_RendererModuleDesc.isActivate))
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

			pEffectWindow->Table_ImageButton("Material", "xcvi3489009dv", m_pMaterialTexture);
			if (m_pMaterialTexture->IsOk())
			{
				string strFilePath = m_pMaterialTexture->Get_FilePathName();
				fs::path fsFilePath = ToRelativePath(strFilePath.data());
				ChangeTexture(&m_pTexture, m_RendererModuleDesc.wstrMaterialPath, fsFilePath.wstring().data());
				wcout << m_RendererModuleDesc.wstrMaterialPath << endl;
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
			CharToWChar(filePath.c_str(), wszFilePath);

			_ulong dwByte = 0;

			MSG_BOX("The file has been loaded successfully");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CDummyParticle::Restart()
{
	Reset_AllParticles();
	__super::Restart();
}
void CDummyParticle::ChangeMainTexture(const _tchar* pTexturePath)
{
	m_RendererModuleDesc.wstrMaterialPath = pTexturePath;
	Safe_Release(m_pTexture);
	m_pTexture = CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterialPath.data());
}
void CDummyParticle::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
{
	BEGININSTANCE;

	// 소유하고 있는 컴포넌트를 지운다.
	wstring wstrComponentTag = (*_pTexture)->Get_Tag();
	CComposite::Delete_Component(wstrComponentTag.data());
	Safe_Release(*_pTexture);

	// 찾는 컴포넌트가 없으면 원본을 만들어준다.
	wstring wstrTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), _pDestPath);
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_TOOL, wstrTag.data()))
	{
		pGameInstance->Add_Prototype(LEVEL_TOOL, wstrTag.data(), CTexture::Create(m_pDevice, m_pContext, m_RendererModuleDesc.wstrMaterialPath.data()));
	}

	// 새로운 텍스처 컴포넌트를 추가한다.
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, wstrTag.data()
		, wstrComponentTag.data(), reinterpret_cast<CComponent**>(_pTexture))))
	{
		MSG_BOX("Failed to Add Component");
		ENDINSTANCE;
		return;
	}
	_wstrOriginPath = _pDestPath;

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