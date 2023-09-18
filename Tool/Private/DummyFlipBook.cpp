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

HRESULT CDummyFlipBook::Initialize_Prototype(_uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum)
{
	if (FAILED(__super::Initialize_Prototype(iLevel, pFilePath, iWidthNum, iHeightNum)))
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

	m_pClipChannelCombo = CComboBox::Create(Generate_Hashtag(true).data(), "ClipChannel", { "Red", "Green", "Blue", "Alpha" }, "Alpha");
	m_pClipChannelCombo->Set_StartTag(m_strClipChannel.data());
	m_pLoopOption = CComboBox::Create(Generate_Hashtag(true).data(), "Loop Option", { "Once", "Loop", "Ping-Pong", }, "Once");

	m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	return S_OK;
}

void CDummyFlipBook::ChangeTexture(CTexture** _pTexture, wstring& _wstrOriginPath, const _tchar* _pDestPath)
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

void CDummyFlipBook::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);
	if (ImGui::BeginTable("CDummyFlipbookTable", 2))
	{
		ImGui::TableNextRow();

		pEffectWindow->Table_ImageButton("Texture", "dkkcii499kd", m_pTextureIFD);
		// 메인 텍스처 교체
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_wstrPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
			m_iCurIndex = 0;
		}

		if (pEffectWindow->Table_DragInt("Width Length", "kcvjie939d", (_int*)&m_iWidthLength))
		{
			m_iMaxIndex = m_iWidthLength * m_iHeightLength - 1;
		}

		if(pEffectWindow->Table_DragInt("Height Length", "cckei39kckc", (_int*)&m_iHeightLength))
		{
			m_iMaxIndex = m_iWidthLength * m_iHeightLength - 1;
		}

		m_pClipChannelCombo->Tick(CComboBox::FLAG::TABLE);
		if (m_pClipChannelCombo->IsUpdated())
			m_strClipChannel = m_pClipChannelCombo->Get_Current_Item();

		m_pLoopOption->Tick(CComboBox::FLAG::TABLE);
		if (m_pLoopOption->IsUpdated())
			m_strLoopOption = m_pLoopOption->Get_Current_Item();

		pEffectWindow->Table_CheckBox("Use Normal Texture", "xcljv8o38dx", &m_isUseNormalTexture);
		pEffectWindow->Table_ImageButton("Texture", "zxzxkkdie0", m_pNormalTextureIFD);
		
		// 노말 텍스처 교체
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_wstrPath, ToRelativePath(fsFilePath.wstring().data()).c_str());
			m_iCurIndex = 0;
		}
		ImGui::EndTable();
	}
}

CDummyFlipBook* CDummyFlipBook::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel, const _tchar* pFilePath, _uint iWidthNum, _uint iHeightNum)
{
	CDummyFlipBook* pInstance = New CDummyFlipBook(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel, pFilePath, iWidthNum, iHeightNum)))
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
	
	Safe_Release(m_pClipChannelCombo);
	Safe_Release(m_pLoopOption);

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}
