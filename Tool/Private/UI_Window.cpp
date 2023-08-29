#include "UI_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"

CUI_Window::CUI_Window(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CUI_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);

	//pGameInstance->ReadFileInDirectory(m_FilePath, TEXT("../../Resources/UI/Game/UI/Actor/UI_T_HUD_Enemy_Skull.png"), TEXT(".png"));

	//Safe_Release(pGameInstance);

	m_TreeDesc.m_wstrName = TEXT("../../Resources/UI/");
	m_TreeDesc.m_isFolder = true;
	//Read_File_In_Directory_Tree(m_TreeDesc, TEXT("../../Resources/UI/"), TEXT(".png"));

	return S_OK;
}

void CUI_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("UI", nullptr, m_WindowFlag);

	Open_Dialog();
	Open_File_Path_Tree(&m_TreeDesc);

	ImGui::End();
}

HRESULT CUI_Window::Render()
{
	return S_OK;
}

void CUI_Window::Open_Dialog()
{
	// open Dialog Simple
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("UI_Texture", "Choose File", ".png, .dds", "");

	// display
	if (ImGuiFileDialog::Instance()->Display("UI_Texture"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

void CUI_Window::Open_File_Path_Tree(UI_Tree* pTree)
{
	if (pTree->m_isFolder)
	{
		_char szChar[MAX_STR];
		memset(szChar, 0, sizeof(_char));
		WCharToChar(pTree->m_wstrName.c_str(), szChar);

		_int iIndex = 0;
		if (pTree->m_wstrName.find(TEXT("Resources")) != string::npos)
		{
			iIndex = _int(pTree->m_wstrName.find(TEXT("Resources")));   //해당 문자의 시작 인덱스 반환
		}

		wstring wstrPath = pTree->m_wstrName.substr(iIndex, pTree->m_wstrName.length());
		WCharToChar(wstrPath.c_str(), szChar);

		if (ImGui::TreeNode(szChar))
		{
			for (auto& iter : pTree->children)
			{
				Open_File_Path_Tree(iter);
			}
			ImGui::TreePop();
		}
	}
	else
	{
		_char szChar[MAX_STR];
		memset(szChar, 0, sizeof(_char));
		WCharToChar(pTree->m_wstrName.c_str(), szChar);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szChar, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		_char szFullPath[MAX_PATH] = "";
		strcat_s(szFullPath, szFileName);
		strcat_s(szFullPath, szExt);

		ImGui::Text(szFullPath);
		ImGui::SameLine();
		if (ImGui::SmallButton("Button"))
		{

		}
	}
}

HRESULT CUI_Window::Read_File_In_Directory_Tree(_Inout_ UI_Tree& Root, const _tchar* pFilePath, const _tchar* pExt)
{
	// 디렉토리 경로를 순회할 iterator
	fs::directory_iterator iter(fs::absolute(pFilePath));

	while (iter != fs::end(iter))
	{
		// 실제 디렉토리 경로를 담고있는 변수 (iterator의 원본)
		const fs::directory_entry& entry = *iter;

		// 현재 entry 변수가 디렉토리인지 확인 후 디렉토리이면 재귀
		if (fs::is_directory(entry.path()))
		{
			UI_Tree* pChild = new UI_Tree;
			pChild->m_isFolder = true;
			pChild->m_wstrName = entry.path().c_str();
			if (FAILED(Read_File_In_Directory_Tree(*pChild, entry.path().c_str(), pExt)))
				return E_FAIL;
			Root.children.push_back(pChild);
			m_Trees.push_back(pChild);
		}
		else
		{
			// 파일 확장자 체크
			if (!_wcsicmp(entry.path().extension().c_str(), pExt))
			{
				UI_Tree* pChild = new UI_Tree;
				cout << entry.path() << endl;

				pChild->m_isFolder = false;
				pChild->m_wstrName = entry.path().c_str();
				Root.children.push_back(pChild);
				m_Trees.push_back(pChild);
			}
		}

		iter++;
	}

	return S_OK;
}

CUI_Window* CUI_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CUI_Window* pInstance = New CUI_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CUI_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CUI_Window::Free(void)
{
	__super::Free();

		for (auto& iter : m_Trees)
		{
			Safe_Delete(iter);
		}
}