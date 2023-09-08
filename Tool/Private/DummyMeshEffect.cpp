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


void CDummyMeshEffect::ImGui(_float _fTimeDelta)
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
			m_Path[TEXTURE_PATH] = ToRelativePath(fsFilePath.wstring().data());
			wstring wstrTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), fsFilePath.wstring().data());
			CComposite::Delete_Component(TEXT("Com_MainTexture"));
			Safe_Release(m_pTexture);
			CComposite::Add_Component(0, wstrTag.data(), L"Com_MainTexture", (CComponent**)&m_pTexture);
		}

		pEffectWindow->Table_ImageButton("Change AlphaClipTexture", "xcvljii23458", m_pAlphaClipTextureIFD);
		// 클립 텍스처 교체
		if (m_pAlphaClipTextureIFD->IsOk())
		{
			fs::path fsFilePath = m_pAlphaClipTextureIFD->Get_FilePathName();
			m_Path[ALPHA_CLIP_TEXTURE_PATH] = ToRelativePath(fsFilePath.wstring().data());
			wstring wstrTag = ToPrototypeTag(TEXT("Prototype_Component_Texture"), fsFilePath.wstring().data());
			CComposite::Delete_Component(TEXT("Com_AlphaClipTexture"));
			Safe_Release(m_pAlphaClipTexture);
			CComposite::Add_Component(0, wstrTag.data(), L"Com_AlphaClipTexture", (CComponent**)&m_pAlphaClipTexture);
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
				m_Path[MODEL_PATH] = ToRelativePath(fsFilePath.wstring().data());
				wstring pTag = ToPrototypeTag(TEXT("Prototype_Component_Model"), fsFilePath.wstring().data());
				Safe_Release(m_pModel);
				CComposite::Delete_Component(TEXT("Com_Model"));
				CComposite::Add_Component(0, pTag.data(), TEXT("Com_Model"), (CComponent**)&m_pModel);
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
}

void CDummyMeshEffect::ChangeTexture(const _tchar* _pTag)
{
}

void CDummyMeshEffect::ChangeModel(const _tchar* _pTag)
{
}

CDummyMeshEffect* CDummyMeshEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath)
{
	CDummyMeshEffect* pInstance = new CDummyMeshEffect(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype(_pDirectoryPath)))
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
}
