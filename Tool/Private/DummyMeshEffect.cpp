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
	m_pTextureIFD->m_strStartPath = "../../Resources/UI/UI/Game/VFX/Textures";
	m_pTextureIFD->m_iImageButtonWidth = 32;

	m_pAlphaClipTextureIFD = CImageFileDialog::Create(m_pDevice, Generate_Hashtag(true).data());
	m_pAlphaClipTextureIFD->m_strStartPath = "../../Resources/UI/UI/Game/VFX/Textures";
	m_pAlphaClipTextureIFD->m_iImageButtonWidth = 32;

	vector<string> Passes = m_pShader->Get_PassList();
	m_pPassComboBox = CComboBox::Create(Generate_Hashtag(true).data(), "Pass", Passes, Passes.front().data());
	
	return S_OK;
}

void CDummyMeshEffect::Tick_Imgui(_float _fTimeDelta)
{
	CImWindow* pWindow = CWindow_Manager::GetInstance()->Find_Window(TEXT("Effect_Window"));
	CEffect_Window* pEffectWindow = dynamic_cast<CEffect_Window*>(pWindow);

	if (ImGui::BeginTable("CDummyMeshEffectTable", 2))
	{
		ImGui::TableNextRow();
		
		pEffectWindow->Table_ImageButton("Change MainTexture", "xcvljii23458", m_pTextureIFD);
		// 메인 텍스처 교체
		if (m_pTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_Path[TEXTURE_PATH], ToRelativePath(fsFilePath.wstring().data()).c_str());
		}

		pEffectWindow->Table_ImageButton("Change AlphaClipTexture", "xcvljii23458", m_pAlphaClipTextureIFD);
		// 클립 텍스처 교체
		if (m_pAlphaClipTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pAlphaClipTextureIFD->Get_FilePathName();
			ChangeTexture(&m_pTexture, m_Path[ALPHA_CLIP_TEXTURE_PATH], ToRelativePath(fsFilePath.wstring().data()).c_str());
		}
		
		// 모델 교체
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("ChangeModel"); ImGui::TableSetColumnIndex(1);
		string strModelButtonName = "Select Model";
		if (ImGui::Button(strModelButtonName.data()))
			ImGuiFileDialog::Instance()->OpenDialog("ChagneModelFileDialog", "Load Model", ".dat", "../../Resources/Models/");
		ImGui::TableNextRow();
		// display
		if (ImGuiFileDialog::Instance()->Display("ChagneModelFileDialog"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				fs::path fsFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				fs::path fsFilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				ChangeModel(&m_pModel, m_Path[MODEL_PATH], ToRelativePath(fsFilePathName.wstring().data()).c_str());
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		// 셰이더 교체
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Change Shader"); ImGui::TableSetColumnIndex(1);
		string strShaderButtonName = "Select Shader";
		if (ImGui::Button(strShaderButtonName.data()))
			ImGuiFileDialog::Instance()->OpenDialog("ChagneShaderFileDialog", "Load Shader", ".hlsl", "../Bin/ShaderFiles/");
		ImGui::TableNextRow();
		// display
		if (ImGuiFileDialog::Instance()->Display("ChagneShaderFileDialog"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				fs::path fsFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				fs::path fsFilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				m_Path[MODEL_PATH] = ToRelativePath(fsFilePath.wstring().data());
				wstring pTag = ToPrototypeTag(TEXT("Prototype_Component_"), fsFilePathName.wstring().data());
				Safe_Release(m_pShader);
				CComposite::Delete_Component(TEXT("Com_Shader"));
				CComposite::Add_Component(0, pTag.data(), TEXT("Com_Shader"), (CComponent**)&m_pShader);
				m_pPassComboBox->Change_Items(m_pShader->Get_PassList());
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		// 현재 셰이더 파일의 패스 교체
		m_strPassName = m_pPassComboBox->Tick(CComboBox::TABLE, true);
		pEffectWindow->Table_DragFloat2Range("Offset", "dfvrpije48dic", &m_vOffset);
		pEffectWindow->Table_DragFloat2Range("Tililing", "dfvrpv8348e48dic", &m_vTililing);
		ImGui::EndTable();
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
	Safe_Release(m_pAlphaClipTextureIFD);
	Safe_Release(m_pPassComboBox);

	for (auto& pTag : m_pTags)
		Safe_Delete_Array(pTag);
}
