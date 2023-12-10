#include "..\Public\DummyMeshEffect.h"
#include "Window_Manager.h"
#include "Effect_Window.h"
#include "ImageFileDialog.h"
#include "ComboBox.h"
#include "ImGuiFileDialog.h"

CDummyMeshEffect::CDummyMeshEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CMeshEffect(_pDevice, _pContext)
{
}

CDummyMeshEffect::CDummyMeshEffect(const CDummyMeshEffect& _rhs)
	: CMeshEffect(_rhs)
{
}

CDummyMeshEffect::~CDummyMeshEffect()
{
}

HRESULT CDummyMeshEffect::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	m_pTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/";
	m_pTextureIFD->m_iImageButtonWidth = 32;

	m_pClipTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pClipTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/";
	m_pClipTextureIFD->m_iImageButtonWidth = 32;

	m_pEmissionTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pEmissionTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/";
	m_pEmissionTextureIFD->m_iImageButtonWidth = 32;

	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Clip Channel", { "Red", "Green", "Blue", "Alpha" }, "Red");
	m_pEmissionChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Emission Channel", { "Red", "Green", "Blue", "Alpha" }, "Red");
	m_pColorEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Color Ease", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pPosEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Position Ease", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pRotEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Rotation Ease", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);
	m_pScaleEaseCombo = CComboBox::Create(Generate_Hashtag(true).data(), "Scale Ease", CEase::pEases, CEase::EASE_END, CEase::pEases[0]);

	vector<string> Passes = m_pShader->Get_PassList();
	m_pPassComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Pass", Passes, Passes.front().data());
	
	return S_OK;
}

void CDummyMeshEffect::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = static_cast<CEffect_Window*>(pWindow);

	if (ImGui::BeginTable("CDummyMeshEffectTable", 2))
	{
		ImGui::TableNextRow();
		
		// 메인 텍스처 교체
		pEffectWindow->Table_ImageButton("Diffuse Texture", "bgfqsqxxc", m_pTextureIFD);
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_Path[TEXTURE_PATH], ToRelativePath(fsFilePath.wstring().data()).c_str());
		}

		pEffectWindow->Table_CheckBox("Alpha Blend", "939kjvodjdkjkcvxc", &m_isAlphaBlend);
		if (true == m_isAlphaBlend)
		{
			m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
			if (m_pClipChannelCombo->IsUpdated())
				m_strClipChannel = m_pClipChannelCombo->Get_Current_Item();

			pEffectWindow->Table_DragFloat("Clip Threshold", "dsfef4f44f4f44", &m_fClipThreshold, 0.01f, 0.f, 1.f);
		}

		pEffectWindow->Table_Void();

		// 모델 교체
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("ChangeModel"); ImGui::TableSetColumnIndex(1);
		string strModelButtonName = "Select Model";
		if (ImGui::Button(strModelButtonName.data()))
			ImGuiFileDialog::Instance()->OpenDialog("ChagneModelFileDialog", "Load Model", ".dat", "../../Resources/Models/");
		ImGui::TableNextRow();

		if (ImGuiFileDialog::Instance()->Display("ChagneModelFileDialog"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				fs::path fsFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				fs::path fsFilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				ChangeModel(&m_pModel, m_Path[MODEL_PATH], ToRelativePath(fsFilePathName.wstring().data()).c_str());
			}

			ImGuiFileDialog::Instance()->Close();
		}

		// 패스 교체
		m_strPassName = m_pPassComboBox->Tick(CComboBox::TABLE, true);
		pEffectWindow->Table_DragFloat("LifeTime", "dfimiwejf34g", &m_fLifeTime, 0.1f);
		pEffectWindow->Table_DragFloat2("StartOffset", "xcvlkejic756", &m_vStartOffset, 0.1f, -FLT_MAX);
		pEffectWindow->Table_DragFloat2("Offset", "dfvrpije48dic", &m_vOffset, 0.1f, -FLT_MAX);
		pEffectWindow->Table_DragFloat2("Delta Offset", "dikfvj349v9jvsdv", &m_vDeltaOffset, 0.1f, -FLT_MAX);
		pEffectWindow->Table_Void();
		pEffectWindow->Table_DragFloat2("Start Tiling", "389jcijwec", &m_vStartTiling, 0.1f, -FLT_MAX);
		pEffectWindow->Table_DragFloat2("Tiling", "dfvrpver234v8348e48dic", &m_vTililing, 0.1f, -FLT_MAX);
		pEffectWindow->Table_DragFloat2("Delta Tiling", "ovo9049vfji", &m_vDeltaTiling, 0.1f, -FLT_MAX);

		pEffectWindow->Table_Void();
		 m_pPosEaseCombo->Tick(CComboBox::TABLE);
		 if (m_pPosEaseCombo->IsUpdated())
			 m_ePosEase = static_cast<CEase::EASE>(m_pPosEaseCombo->Get_Current_Item_Index());
		pEffectWindow->Table_DragXYZ("Start Position", "ujmynerfwd", &m_vStartPos, 0.1f);
		pEffectWindow->Table_DragXYZ("End Position", "ghn35235df ", &m_vEndPos, 0.1f);
		pEffectWindow->Table_Void();
		m_pRotEaseCombo->Tick(CComboBox::TABLE);
		if (m_pRotEaseCombo->IsUpdated())
			m_eRotEase = static_cast<CEase::EASE>(m_pRotEaseCombo->Get_Current_Item_Index());
		pEffectWindow->Table_DragXYZ("Start Rotation", "cbvcbnghert", &m_vStartRot, 0.1f);
		pEffectWindow->Table_DragXYZ("End Rotation", "dcsdcvrf34", &m_vEndRot, 0.1f);
		pEffectWindow->Table_Void();
		m_pScaleEaseCombo->Tick(CComboBox::TABLE);
		if (m_pScaleEaseCombo->IsUpdated())
			m_eSizeEase = static_cast<CEase::EASE>(m_pScaleEaseCombo->Get_Current_Item_Index());
		pEffectWindow->Table_DragXYZ("Start Scale", " df3rr5445", &m_vStartSize, 0.1f);
		pEffectWindow->Table_DragXYZ("End Scale", "bjikuouisd", &m_vEndSize, 0.1f);
		pEffectWindow->Table_Void();
		m_pColorEaseCombo->Tick(CComboBox::TABLE);
		if (m_pColorEaseCombo->IsUpdated())
			m_eColorEase = static_cast<CEase::EASE>(m_pColorEaseCombo->Get_Current_Item_Index());
		pEffectWindow->Table_ColorEdit4("Start Color", "cvkjv94efcvxcv", &m_vStartColor);
		pEffectWindow->Table_ColorEdit4("End Color", "sfgnyn45634fg", &m_vEndColor);
		pEffectWindow->Table_CheckBox("Glow", "dkvijies", &m_isGlow);
		pEffectWindow->Table_CheckBox("Distortion", "ciceicjoeijdsfs", &m_isDistortion);
		pEffectWindow->Table_CheckBox("Diffuse", "dkssudgktpdyqksrkqtmtqslek", &m_isDiffuse);
		pEffectWindow->Table_CheckBox("Flutter", "dcijm3i39ckkc", &m_isFlutter);
		if (m_isFlutter)
		{
			pEffectWindow->Table_DragXYZ("Strength", "dkjivjkemsdf", &m_vStrength, 0.1f);
		}
		ImGui::Separator();
		pEffectWindow->Table_CheckBox("Emission", "ckvmie4jmimcvk", &m_isEmission);
		if (m_isEmission)
		{
			// 이미션 텍스처 교체
			pEffectWindow->Table_ImageButton("Emission Texture", "kcjviejf", m_pEmissionTextureIFD);
			if (m_pEmissionTextureIFD->IsOk())
			{
				fs::path fsFilePath = m_pEmissionTextureIFD->Get_FilePathName();
				ChangeTexture(&m_pEmissionTexture, m_Path[EMISSION_PATH], ToRelativePath(fsFilePath.wstring().data()).c_str());
			}
			m_pEmissionChannelCombo->Tick(CComboBox::FLAG::TABLE);
			if (m_pEmissionChannelCombo->IsUpdated())
				m_strEmissionChannel = m_pEmissionChannelCombo->Get_Current_Item();

			pEffectWindow->Table_DragFloat2Range("Remap Range", "kjvisdjfiwejf", &m_vRemap, 0.01f, 0.f, 1.f);
			pEffectWindow->Table_DragFloat("Frequency", "ckkcmdmdmcmx", &m_fFrequency, 0.01f, 0.001f);
			pEffectWindow->Table_ColorEdit3("Emmision Color", "kdiijv93oidksdf", &m_vEmissionColor);
		}

		//pEffectWindow->Table_DragFloat("RimPower", "iv893jdjxxcv", &m_fRimPower);

		ImGui::EndTable();
	}

	if (ImGui::Button("Play"))
		Play(m_pTransform->Get_Position());
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
		Stop();
	ImGui::SameLine();
	// Button for Hard Reset
	if (ImGui::Button("Hard Reset")) {
		// 버튼의 현재 위치와 크기를 가져옵니다.
		ImVec2 buttonPos = ImGui::GetItemRectMin();
		ImVec2 buttonSize = ImGui::GetItemRectSize();

		// 팝업의 크기를 설정합니다.
		ImGui::SetNextWindowSize(ImVec2(350, 100));  // 여기서 300과 150은 예제로 사용된 크기입니다. 원하는 크기로 조절해주세요.

		// 팝업이 버튼의 오른쪽에 위치하도록 좌표를 설정합니다.
		ImGui::SetNextWindowPos(ImVec2(buttonPos.x + buttonSize.x, buttonPos.y));

		// 팝업을 엽니다.
		ImGui::OpenPopup("Confirm Reset");
	}

	// Modal popup for Hard Reset Confirmation
	if (ImGui::BeginPopupModal("Confirm Reset", NULL, ImGuiWindowFlags_NoMove)) {
		ImGui::Text("Are you sure you want to 'destroy' this effect?");

		// OK Button
		if (ImGui::Button("OK")) {
			m_isHardReset = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();

		// Spacing
		ImGui::SameLine();

		// Cancel Button
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::Separator();

	// 세이브 로드
	Save_FileDialog();
	Load_FileDialog();

	// 트랜스폼 노드
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
	pEffectWindow->MatrixNode(&WorldMatirx, "DummyMesh_Transform", "DummyMesh_Position", "DummyMesh_Rotation", "DummyMesh_Scale");
	Get_Transform()->Set_WorldMatrix(WorldMatirx);
}

void CDummyMeshEffect::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
{
	// 소유하고 있는 컴포넌트를 지운다.
	_tchar wszTag[MAX_STR];
	lstrcpy(wszTag, (*_pTexture)->Get_Tag());

	if (FAILED(CComposite::Delete_Component(wszTag)))
	{
		MSG_BOX("Failed to Delete");
		E_FAIL;
	}

	Safe_Release(*_pTexture);

	BEGININSTANCE;
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
void CDummyMeshEffect::ChangeModel(CModel** _pModel, wstring& _wstrOriginPath, const _tchar* _pDestPath, CModel::TYPE _eAnimType)
{
	// 소유하고 있는 컴포넌트를 지운다.
	_tchar wszTag[MAX_STR];
	lstrcpy(wszTag, (*_pModel)->Get_Tag());

	if (FAILED(CComposite::Delete_Component(wszTag)))
	{
		MSG_BOX("Failed to Delete");
		E_FAIL;
	}

	Safe_Release(*_pModel);

	BEGININSTANCE;
	// 찾는 컴포넌트가 없으면 원본을 만들어준다.
	wstring tempTag = ToPrototypeTag(TEXT("Prototype_Component_Model"), _pDestPath).data();
	_tchar* pTag = { nullptr };
	m_pTags.push_back(pTag = new _tchar[tempTag.length() + 1]);
	lstrcpy(pTag, tempTag.c_str());

	// 기존에 컴포넌트가 존재하는지 확인
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_TOOL, pTag))
	{
		// 없다면 새로운 프로토타입 컴포넌트 생성
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, pTag, CModel::Create(m_pDevice
			, m_pContext, _eAnimType,_pDestPath))))
		{
			MSG_BOX("Failed to Create Prototype : Change Model");
		}
	}

	// 새로운 모델 컴포넌트를 클론한다.
	_wstrOriginPath = _pDestPath;
	if (FAILED(CComposite::Add_Component(LEVEL_TOOL, pTag
		, wszTag, reinterpret_cast<CComponent**>(_pModel))))
	{
		MSG_BOX("Failed to Add Component");
		ENDINSTANCE;
		return;
	}
	ENDINSTANCE;
}

HRESULT CDummyMeshEffect::Save_FileDialog()
{
	if (ImGui::Button("Save MeshEffect"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseSaveFileMeshEffectKey", "Save File", ".ME", "../../Resources/GameData/MeshEffectData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseSaveFileMeshEffectKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			fs::path fsFilePathName = ToRelativePath(filePathName.data());
			if (FAILED(__super::Save(fsFilePathName.wstring().data())))
			{
				MSG_BOX("Failed to Saved CDummyTrail");
				return E_FAIL;
			}

			MSG_BOX("Success to Saved CDummyTrail");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	return S_OK;
}

HRESULT CDummyMeshEffect::Load_FileDialog()
{
	if (ImGui::Button("Load MeshEffect"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseLoadFileMeshEffectKey", "Load File", ".ME", "../../Resources/GameData/MeshEffectData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseLoadFileMeshEffectKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			fs::path fsFilePathName = filePathName;
			fs::path fsFilePath = filePath;

			_ulong dwByte = 0;

			if (FAILED(__super::Load(fsFilePathName.wstring().c_str())))
			{
				MSG_BOX("Failed to loaded file");
			}
			else
			{
				// LoadAfter
 				ChangeTexture(&m_pTexture, m_Path[TEXTURE_PATH], m_Path[TEXTURE_PATH].c_str());
				ChangeTexture(&m_pEmissionTexture, m_Path[EMISSION_PATH], m_Path[EMISSION_PATH].c_str());
				ChangeModel(&m_pModel, m_Path[MODEL_PATH], m_Path[MODEL_PATH].c_str());
				m_pTextureIFD->ChangeTexture(wstrToStr(m_Path[TEXTURE_PATH]).c_str());
				m_pEmissionTextureIFD->ChangeTexture(wstrToStr(m_Path[EMISSION_PATH]).c_str());
				m_pColorEaseCombo->Update_Current_Item(m_eColorEase);
				m_pScaleEaseCombo->Update_Current_Item(m_eSizeEase);
				m_pRotEaseCombo->Update_Current_Item(m_eRotEase);
				m_pPosEaseCombo->Update_Current_Item(m_ePosEase);
				m_pPassComboBox->Update_Current_Item(m_strPassName);
				MSG_BOX("The file has been loaded successfully");
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	return S_OK;
}

CDummyMeshEffect* CDummyMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* pFilePath, _uint _iLevel)
{
	CDummyMeshEffect* pInstance = new CDummyMeshEffect(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath, _iLevel)))
	{
		MSG_BOX("Failed to Created CDummyMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummyMeshEffect::Clone(void* _pArg)
{
	CDummyMeshEffect* pInstance = new CDummyMeshEffect(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Created CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummyMeshEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureIFD);
	Safe_Release(m_pClipTextureIFD);
	Safe_Release(m_pEmissionTextureIFD);
	Safe_Release(m_pClipChannelCombo);
	Safe_Release(m_pEmissionChannelCombo);
	Safe_Release(m_pPassComboBox);
	Safe_Release(m_pColorEaseCombo);
	Safe_Release(m_pPosEaseCombo);
	Safe_Release(m_pRotEaseCombo);
	Safe_Release(m_pScaleEaseCombo);

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}
