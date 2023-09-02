#include "UI_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"

#include "Dummy_UI.h"
#include "Layer.h"
#include "GameObject.h"

CUI_Window::CUI_Window(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CUI_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_None;

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//Safe_AddRef(pGameInstance);

	//pGameInstance->ReadFileInDirectory(m_FilePath, TEXT("../../Resources/UI/Game/UI/Actor/UI_T_HUD_Enemy_Skull.png"), TEXT(".png"));

	//Safe_Release(pGameInstance);

	/*m_TreeDesc.m_wstrName = TEXT("../../Resources/UI/");
	m_TreeDesc.m_isFolder = true;
	Read_File_In_Directory_Tree(m_TreeDesc, TEXT("../../Resources/UI/"), TEXT(".png"));*/

	return S_OK;
}

void CUI_Window::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.top)) + m_vWindowPos);

	ImGui::Begin("UI", nullptr, m_WindowFlag);

	if (ImGui::CollapsingHeader("Image List"))
	{
		//Open_Dialog();
		Open_File_Path_Tree(&m_TreeDesc);
	}

	if (ImGui::CollapsingHeader("Object List"))
	{
		Show_Object_List();
	}


	Interaction_UI();
	Move_UI();

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


void CUI_Window::Show_Object_List()
{	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool_UI"));

	if (nullptr == pLayer)
	{
		Safe_Release(pGameInstance);
		return;
	}


	const unordered_map<const _tchar*, CGameObject*> GameObjects = pLayer->Get_GameObjects();

	vector <CGameObject*> pGameObejctVector;

	for (const auto& pair : GameObjects)
	{
		pGameObejctVector.push_back(pair.second);
	}

	_tchar	wszName[MAX_STR] = {};
	lstrcpy(wszName, pGameObejctVector[0]->Get_Tag());

	Safe_Release(pGameInstance);

	static int iItem = 0;
	if (ImGui::BeginListBox("Layer_Tool_UI"))
	{
		for (int n = 0; n < pGameObejctVector.size(); n++)
		{
			const bool is_selected = (iItem == n);

			_char szGameObjectName[MAX_PATH];
			memset(szGameObjectName, 0, sizeof(_char) * MAX_PATH);

			WCharToChar(pGameObejctVector[n]->Get_Tag(), szGameObjectName);
			if (ImGui::Selectable(szGameObjectName, is_selected))
				iItem = n;		

			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
				_tchar wszGameObjectName[MAX_PATH];
				lstrcpy(wszGameObjectName, pGameObejctVector[n]->Get_Tag());

				string DragFloatTag = "Speed##";
				_float3 vPos = pGameObejctVector[n]->Get_Transform()->Get_Position();
				ImGui::DragFloat3(DragFloatTag.c_str(), reinterpret_cast<_float*>(&vPos), 0.01f);
				pGameObejctVector[n]->Get_Transform()->Set_Position(vPos);
			}
		}
		ImGui::EndListBox();
	}
}

void CUI_Window::Open_File_Path_Tree(UI_Tree* pTree)
{
	if (pTree->m_isFolder)
	{
		m_iTreeIndex = m_iInitIndex;

		_char szChar[MAX_PATH];
		memset(szChar, 0, sizeof(_char) * MAX_PATH);
		WCharToChar(pTree->m_wstrName.c_str(), szChar);

		_int iIndex = 0;
		if (pTree->m_wstrName.find(TEXT("UI")) != string::npos)
		{
			iIndex = _int(pTree->m_wstrName.find(TEXT("UI")));   //해당 문자의 시작 인덱스 반환
		}

		wstring wstrPath = pTree->m_wstrName.substr(iIndex, pTree->m_wstrName.length());
		WCharToChar(wstrPath.c_str(), szChar);

		if (ImGui::TreeNode(szChar))
		{
			for (auto& iter : pTree->children)
			{
				Open_File_Path_Tree(iter);
				m_iTreeIndex++;
			}
			ImGui::TreePop();
		}
	}
	else
	{
		_char szChar[MAX_PATH];
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

		string ShowIndex = "Show_Image##" + to_string(m_iTreeIndex);
		if (ImGui::SmallButton(ShowIndex.c_str()))
		{
			pTree->m_isShow = !(pTree->m_isShow);
		}

		if (pTree->m_isShow)
		{
			if (nullptr == pTree->m_SRV)
			{
				bool ret = Load_ImTexture(szChar, &pTree->m_SRV, &pTree->m_iWidth, &pTree->m_iHeight);
				IM_ASSERT(ret);
				m_SRVs.push_back(pTree->m_SRV);
			}

			ImGui::Begin(szFullPath);
			ImGui::Text("size = %d x %d", pTree->m_iWidth, pTree->m_iHeight);
			ImGui::Image((void*)pTree->m_SRV, ImVec2(_float(pTree->m_iWidth), _float(pTree->m_iHeight)));
			ImGui::End();
		}

		ImGui::SameLine();
		string strFileName = szFileName;
		string CreateIndex =  "Create_UI##" + strFileName + to_string(m_iTreeIndex);
		if (ImGui::SmallButton(CreateIndex.c_str()))
		{
			Create_UI(pTree);
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


void CUI_Window::Create_UI(UI_Tree* pTree)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_char szChar[MAX_PATH];
	memset(szChar, 0, sizeof(_char) * MAX_PATH);
	WCharToChar(pTree->m_wstrName.c_str(), szChar);

	_char szFileName[MAX_PATH] = "";
	_splitpath_s(szChar, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	// "Prototype_Component_Texture_" 고정하고 그 뒤 파일 명 붙혀주는 작업
	const _char comDest[MAX_PATH] = "Prototype_Component_Texture_";
	_char szComponent[MAX_PATH] = "";
	strcat_s(szComponent, comDest);
	strcat_s(szComponent, szFileName);

	_tchar wszComponent[MAX_PATH] = {};
	CharToWChar(szComponent, wszComponent);

	// "Prototype_GameObject_UI_" 고정하고 그 뒤 파일 명 붙혀주는 작업
	const _char* objDest = "Prototype_GameObject_UI_";
	_char szGameObject[MAX_PATH] = "";
	strcat_s(szGameObject, objDest);
	strcat_s(szGameObject, szFileName);

	_tchar wszGaemObject[MAX_PATH] = {};
	CharToWChar(szGameObject, wszGaemObject);


	// m_isAddPrototype를 확인 후 중복해서 프로토타입 추가되는 작업 방지.
	if (pTree->m_isAddPrototype == false)
	{
		// 텍스쳐 추가.
		if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, wszComponent,
			CTexture::Create(m_pDevice, m_pContext, pTree->m_wstrName.c_str()))))
		{
			MSG_BOX("Failed Create Texture Component");
		}
		
		if (FAILED(pGameInstance->Add_Prototype_GameObject(wszGaemObject,
			CDummy_UI::Create(m_pDevice, m_pContext, wszComponent))))
		{
			MSG_BOX("Failed Create UI");
		}
		pTree->m_isAddPrototype = true;
	}


	// UI Layer 사이즈 확인 후 GameObjectTag 뒤 번호 달기.
	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool_UI"));
	_int iSize = 0;
	if (nullptr != pLayer)
		iSize = _int(pLayer->Get_GameObjects().size());
	
	string strFimeName = szFileName;
	string GameObjectTag = "UI_" + strFimeName + to_string(iSize);
	_tchar wszGameObjectTag[MAX_PATH] = TEXT("");

	CharToWChar(GameObjectTag.c_str(), wszGameObjectTag);
	
	// Dummy UI Object 생성.
	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, wszGaemObject,
		TEXT("Layer_Tool_UI"), wszGameObjectTag, nullptr)))
	{
		MSG_BOX("Failed to Created CDummy_UI Clone");
	}

	Safe_Release(pGameInstance);
}

void CUI_Window::Interaction_UI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool_UI"));

	if (nullptr == pLayer)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);

	const unordered_map<const _tchar*, CGameObject*> GameObjects = pLayer->Get_GameObjects();

	vector <CGameObject*> pGameObejctVector;

	for (const auto& pair : GameObjects)
	{
		pGameObejctVector.push_back(pair.second);
	}


	for (auto& pGameObject : pGameObejctVector)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (dynamic_cast<CDummy_UI*>(pGameObject)->Is_In_Rect() && 
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			m_pDummy_UI = dynamic_cast<CDummy_UI*>(pGameObject);
			break;
		}
		else if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_UP))
		{
			m_pDummy_UI = nullptr;
		}

		Safe_Release(pGameInstance);
	}

#ifdef _DEBUG
	cout << m_pDummy_UI << endl;
#endif // _DEBUG
}

void CUI_Window::Move_UI()
{
	// 현재 마우스 위치 얻기
	POINT CurrentMousePos;
	GetCursorPos(&CurrentMousePos);

	if (nullptr == m_pDummy_UI)
	{
		m_MousePos = CurrentMousePos; // 현재 위치를 이전 위치로 저장
		return;
	}

	// 마우스 이동 거리 계산
	_int iMoveX = CurrentMousePos.x - m_MousePos.x;
	_int iMoveY = CurrentMousePos.y - m_MousePos.y;

	_float3 vPos = m_pDummy_UI->Get_Transform()->Get_Position();
	_float2 fXY = m_pDummy_UI->Get_fXY();

	m_pDummy_UI->Set_fXY(fXY.x + iMoveX, fXY.y + iMoveY);

	m_pDummy_UI->Get_Transform()->Set_Position(
		XMVectorSet((fXY.x + iMoveX)- g_iWinSizeX * 0.5f,
			(-fXY.y + iMoveY) + g_iWinSizeY * 0.5f, 0.f, 1.f));

	
	m_MousePos = CurrentMousePos; // 현재 위치를 이전 위치로 저장
}

_bool CUI_Window::Load_ImTexture(const _char* pFilePath, ID3D11ShaderResourceView** out_srv, _int* out_width, _int* out_height)
{
	_int image_width = 0;
	_int image_height = 0;
	_uchar* image_data = stbi_load(pFilePath, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	Safe_Release(pTexture);

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
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

	for (auto& Tree : m_Trees)
	{
		Safe_Delete(Tree);
	}

	for (auto& SRV : m_SRVs)
	{
		Safe_Release(SRV);
	}
}