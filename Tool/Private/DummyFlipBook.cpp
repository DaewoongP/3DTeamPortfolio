#include "DummyFlipBook.h"
#include "GameInstance.h"
#include "Effect_Window.h"
#include "Window_Manager.h"
#include "ComboBox.h"
#include "ImageFileDialog.h"

CDummyFlipBook::CDummyFlipBook(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTexture_Flipbook(pDevice, pContext)
{
}

CDummyFlipBook::CDummyFlipBook(const CDummyFlipBook& rhs)
	: CTexture_Flipbook(rhs)
{
}

HRESULT CDummyFlipBook::Initialize_Prototype(_uint iLevel, const _tchar* pFilePath)
{
	if (FAILED(__super::Initialize_Prototype(iLevel, pFilePath)))
		return E_FAIL;

	m_iLevel = iLevel;

	return S_OK;
}

HRESULT CDummyFlipBook::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/Flipbooks/";
	m_pTextureIFD->m_iImageButtonWidth = 32;

	m_pNormalTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pNormalTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/Flipbooks/";
	m_pNormalTextureIFD->m_iImageButtonWidth = 32;

	m_pFlipbookMaterialTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pFlipbookMaterialTextureIFD->m_strStartPath = "../../Resources/Effects/Textures/Flipbooks/";
	m_pFlipbookMaterialTextureIFD->m_iImageButtonWidth = 32;

	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "ClipChannel", { "Red", "Green", "Blue", "Alpha" }, "Alpha");
	m_pClipChannelCombo->Set_StartTag(m_strClipChannel.data());
	m_pLoopOption = CComboBox::Create(Generate_Hashtag(true).data(), "Loop Option", { "Once", "Loop", "Ping-Pong", }, "Loop");

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CDummyFlipBook::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
{
	// 소유하고 있는 컴포넌트를 지운다.
	_tchar wszTag[MAX_STR];
	if (nullptr != *_pTexture)
	{
		lstrcpy(wszTag, (*_pTexture)->Get_Tag());
		if (FAILED(CComposite::Delete_Component(wszTag)))
		{
			MSG_BOX("Failed to Delete");
			E_FAIL;
		}
		Safe_Release(*_pTexture);
	}

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

void CDummyFlipBook::Save_FileDialog()
{
	if (ImGui::Button("Save Flipbook"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseSaveFileFlipbookKey", "Save File", ".flipbook", "../../Resources/GameData/FlipbookData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseSaveFileFlipbookKey"))
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
				return;
			}

			MSG_BOX("Success to Saved CDummyTrail");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CDummyFlipBook::Load_FileDialog()
{
	if (ImGui::Button("Load Flipbook"))
		ImGuiFileDialog::Instance()->OpenDialog("CooseLoadFileFlipbookKey", "Load File", ".flipbook", "../../Resources/GameData/FlipbookData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("CooseLoadFileFlipbookKey"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			fs::path fsFilePathName = filePathName;
			fs::path fsFilePath = filePath;

			_ulong dwByte = 0;

			if (FAILED(Load(fsFilePathName.wstring().c_str())))
			{
				MSG_BOX("Failed to loaded file");
			}
			else
			{
				ChangeTexture(&m_pDiffuseTexture, m_wstrPath, ToRelativePath(m_wstrPath.c_str()).c_str());
				m_pTextureIFD->ChangeTexture(wstrToStr(m_wstrPath).c_str());
				if (true == m_isUseNormalTexture)
				{
					ChangeTexture(&m_pNormalTexture, m_wstrNormalPath, ToRelativePath(m_wstrNormalPath.c_str()).c_str());
					m_pNormalTextureIFD->ChangeTexture(wstrToStr(m_wstrNormalPath).c_str());
				}
					
				m_pClipChannelCombo->Update_Current_Item(m_strClipChannel);
				m_pLoopOption->Update_Current_Item(m_strLoopOption);

				MSG_BOX("The file has been loaded successfully");
			}
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CDummyFlipBook::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = static_cast<CEffect_Window*>(pWindow);
	if (ImGui::BeginTable("CDummyFlipbookTable", 2))
	{
		ImGui::TableNextRow();

		pEffectWindow->Table_ImageButton("Texture", "cvcvklxcvkl99ds", m_pTextureIFD);
		// 메인 텍스처 교체
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pDiffuseTexture, m_wstrPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
			m_iCurIndex = 0;
		}

		// 열 길이
		if (pEffectWindow->Table_DragInt("Width Length", "kcvjie939d", (_int*)&m_iWidthLength))
		{
			m_iMaxIndex = m_iWidthLength * m_iHeightLength - 1;
		}

		// 행 길이
		if(pEffectWindow->Table_DragInt("Height Length", "cckei39kckc", (_int*)&m_iHeightLength))
		{
			m_iMaxIndex = m_iWidthLength * m_iHeightLength - 1;
		}

		// 클립 채널
		m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
		if (m_pClipChannelCombo->IsUpdated())
			m_strClipChannel = m_pClipChannelCombo->Get_Current_Item();

		// 루프 옵션
		m_pLoopOption->Tick(CComboBox::FLAG::TABLE);
		if (m_pLoopOption->IsUpdated())
			m_strLoopOption = m_pLoopOption->Get_Current_Item();

		// 색상 섞는 옵션
		pEffectWindow->Table_CheckBox("Mix Color", "xcvxcvxcvwe3", &m_isMixColor);
		if (true == m_isMixColor)
		{
			pEffectWindow->Table_ColorEdit4("Color", "ckjv88d3ikdfs", &m_vColor);
		}

		// 노말 텍스처 쓸건지 여부
		pEffectWindow->Table_CheckBox("Use Normal Texture", "qweipidjckxc", &m_isUseNormalTexture);
		if (true == m_isUseNormalTexture)
		{
			pEffectWindow->Table_ImageButton("Normal Texture", "vbnmfgjkiuerwety", m_pNormalTextureIFD);
			// 노말 텍스처 교체
			if (m_pNormalTextureIFD->IsOk())
			{
				fs::path fsFilePath = m_pNormalTextureIFD->Get_FilePathName();
				ChangeTexture(&m_pNormalTexture, m_wstrNormalPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
				m_iCurIndex = 0;
			}
		}

		pEffectWindow->Table_CheckBox("BillBoard DeleteY", "dkjiucjvicvxcve", &m_isDeleteY);
		pEffectWindow->Table_DragFloat("Update Interval", "ckk99djjjejded", &m_fUpdateInterval, 0.001f, 0.015f);

		// 노말 텍스처 쓸건지 여부
		pEffectWindow->Table_CheckBox("Use Flipbook Material Texture", "xcvbnmuirty", &m_isUseFlipbookMaterialTexture);
		if (true == m_isUseFlipbookMaterialTexture)
		{
			pEffectWindow->Table_ImageButton("Flipbook Material Texture", "flkdsjfi3ij3", m_pFlipbookMaterialTextureIFD);
			// 노말 텍스처 교체
			if (m_pFlipbookMaterialTextureIFD->IsOk())
			{
				fs::path fsFilePath = m_pFlipbookMaterialTextureIFD->Get_FilePathName();
				ChangeTexture(&m_pFlipbookMaterialTexture, m_wstrFlipbookMaterialPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
				m_iCurIndex = 0;
			}
		}

		ImGui::EndTable();
	}

	ImGui::Separator();
	// 재생 버튼
	if (ImGui::Button("Play##paslkdjfvidrvjo"))
		this->Play(m_pTransform->Get_Position());

	// 중지 버튼
	if (ImGui::Button("Stop##dkkc9949dkcv"))
		this->Stop();

	ImGui::Separator();
	Save_FileDialog();
	Load_FileDialog();

	_float4x4 pMatrix = *m_pTransform->Get_WorldMatrixPtr();
	pEffectWindow->MatrixNode(&pMatrix, "Flipbook Transform", "Flipbook_Position", "Flipbook_Rotation", "Flipbook_Scale");
	m_pTransform->Set_WorldMatrix(pMatrix);
}

CDummyFlipBook* CDummyFlipBook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath)
{
	CDummyFlipBook* pInstance = New CDummyFlipBook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pFilePath)))
	{
		MSG_BOX("Failed to Created CDummyFlipBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummyFlipBook::Clone(void* pArg)
{
	CDummyFlipBook* pInstance = New CDummyFlipBook(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummyFlipBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummyFlipBook::Free()
{
	__super::Free();

	Safe_Release(m_pTextureIFD);
	Safe_Release(m_pNormalTextureIFD);
	Safe_Release(m_pFlipbookMaterialTextureIFD);
	
	Safe_Release(m_pClipChannelCombo);
	Safe_Release(m_pLoopOption);

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}
