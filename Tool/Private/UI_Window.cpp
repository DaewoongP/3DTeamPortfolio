#include "UI_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"

#include "Dummy_UI.h"
#include "Layer.h"
#include "GameObject.h"
#include "Dummy_UI_Group.h"

#include "RenderTarget_Manager.h"
#include "RenderTarget.h"

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

	m_TreeDesc.m_wstrName = TEXT("../../Resources/UI/");
	m_TreeDesc.m_isFolder = true;
	Read_File_In_Directory_Tree(m_TreeDesc, TEXT("../../Resources/UI/"), TEXT(".png"));

	m_pGroupVector.clear();

	return S_OK;
}

void CUI_Window::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.top)) + m_vWindowPos);

	if (m_pUILayer == nullptr)
	{
		BEGININSTANCE

		m_pUILayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool_UI"));

		ENDINSTANCE
	}

	if (m_pUIGroupLayer == nullptr)
	{
		BEGININSTANCE

			m_pUIGroupLayer = pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Tool_UI_Group"));

		ENDINSTANCE
	}

	ImGui::Begin("UI", nullptr, m_WindowFlag);

	if (ImGui::CollapsingHeader("Image List"))
	{
		//Open_Dialog();
		Open_File_Path_Tree(&m_TreeDesc);
	}

	if (ImGui::CollapsingHeader("Object List"))
	{
		Object_List();
	}
	else
	{
		m_isObjectSelected = false;
		m_isOpenList = false;;
	}

	if (ImGui::CollapsingHeader("Gruop"))
	{
		Input_Text();
		UI_Gruop_Combo();
		UI_Group_Tree();
	}

	Correction_Pick();
	Interaction_UI();
	Move_UI();


	if (ImGui::Button("Capture"))
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		ID3D11Texture2D* pTexture = pGameInstance->Find_RenderTarget(TEXT("Target_Picking"))->Get_Texture2D();

		Capture_UI(pTexture, TEXT("../../"));

		Safe_Release(pGameInstance);
	}

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

void CUI_Window::Object_List_Button()
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.top + 600.f)) + m_vWindowPos);

	ImGui::Begin("List Interaction");

	if (ImGui::Button("Clear"))
	{
		for (auto& iter : m_pUIVector)
		{
			m_pDummy_UI_Group->Clear();
			iter->Set_ObjEvent(CGameObject::OBJ_EVENT::OBJ_DEAD);
		}
	}

	ImGui::Checkbox("MouseInteraction", &m_isListMouseInteraction);

	if (ImGui::Button("Save_UI"))
	{
		Save_UI();
	}

	if (ImGui::Button("Load_UI"))
	{

	}




	ImGui::End();

}

void CUI_Window::Open_Object_List()
{
	if (ImGui::BeginListBox("  "))
	{
		m_isOpenList = true;
		for (_int i = 0; i < m_pUIVector.size(); i++)
		{
			m_isObjectSelected = (m_iObjectListIndex == i);

			_char szGameObjectName[MAX_PATH];
			memset(szGameObjectName, 0, sizeof(_char) * MAX_PATH);

			WCharToChar(m_pUIVector[i]->Get_Tag(), szGameObjectName);
			if (ImGui::Selectable(szGameObjectName, m_isObjectSelected))
			{
				m_iObjectListIndex = i;
			}

			if (m_isObjectSelected)
			{
				ImGui::SetItemDefaultFocus();
				_tchar wszGameObjectName[MAX_PATH];
				lstrcpy(wszGameObjectName, m_pUIVector[i]->Get_Tag());
				m_pDummy_UI = dynamic_cast<CDummy_UI*>(m_pUIVector[i]);
				string DragFloatTag = "Transform##";
				_float4x4 pMatrix = m_pUIVector[i]->Get_Transform()->Get_WorldMatrix();


				RECT rc;
				GetWindowRect(g_hWnd, &rc);
				ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.top + 750.f)) + m_vWindowPos);

				__super::MatrixNode(&pMatrix, "UI_Transform##", "UI_Position##", "UI_Rotation##", "UI_Scale##");
				//	pGameObejctVector[i]->Get_Transform()->Set_WorldMatrix(pMatrix);
				_float2 fScale = _float2(pMatrix.Right().x, pMatrix.Up().y);

				// float2 fXY = dynamic_cast<CDummy_UI*>(m_pUIVector[i])->WorldPos_To_UIPos(pMatrix.Translation().x, pMatrix.Translation().y);
				_float fZ = pMatrix.Translation().z;

				//dynamic_cast<CDummy_UI*>(m_pUIVector[i])->Set_fXY(fXY.x, fXY.y);
				dynamic_cast<CDummy_UI*>(m_pUIVector[i])->Set_fZ(fZ);
				dynamic_cast<CDummy_UI*>(m_pUIVector[i])->Set_Size(fScale.x, fScale.y);

				GetWindowRect(g_hWnd, &rc);
				ImGui::SetNextWindowPos(ImVec2(_float(rc.left) + 200, _float(rc.top + 600.f)) + m_vWindowPos);
				ImGui::Begin("Selected Object");
				
				Select_Obejct(m_pUIVector[i]);

				ImGui::End();
			}
		}

		Object_List_Button();



		ImGui::EndListBox();
	}
}

void CUI_Window::Select_Obejct(CGameObject* pGameObject)
{

	if (nullptr == pGameObject)
		return;

	Add_Group(pGameObject);

	if (ImGui::Button("Delete"))
	{
		m_pDummy_UI_Group->Delete(dynamic_cast<CDummy_UI*>(pGameObject));
		pGameObject->Set_ObjEvent(CGameObject::OBJ_EVENT::OBJ_DEAD);
	}
}


void CUI_Window::Object_List()
{	
	if (nullptr == m_pUILayer)
	{
		return;
	}

	const unordered_map<const _tchar*, CComponent*> Components = m_pUILayer->Get_Components();

	m_pUIVector.clear();

	for (const auto& pair : Components)
	{
		m_pUIVector.push_back(static_cast<CGameObject*>(pair.second));
	}

	_tchar	wszName[MAX_STR] = {};
	if (m_pUIVector.size() > 0)
	{
		lstrcpy(wszName, m_pUIVector[0]->Get_Tag());
	}

	Open_Object_List();
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
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wszComponent,
			CTexture::Create(m_pDevice, m_pContext, pTree->m_wstrName.c_str()))))
		{
			MSG_BOX("Failed Create Texture Component");
		}
		
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wszGaemObject,
			CDummy_UI::Create(m_pDevice, m_pContext, wszComponent))))
		{
			MSG_BOX("Failed Create UI");
		}
		pTree->m_isAddPrototype = true;
	}


	// UI Layer 사이즈 확인 후 GameObjectTag 뒤 번호 달기.
	_int iSize = 0;
	if (nullptr != m_pUILayer)
	{
		iSize = _int(m_pUILayer->Get_Components().size());
	}
	
	string strFimeName = szFileName;
	string GameObjectTag = "UI_" + strFimeName + to_string(iSize);
	_tchar wszGameObjectTag[MAX_PATH] = TEXT("");

	CharToWChar(GameObjectTag.c_str(), wszGameObjectTag);
	
	_float2 fSize = _float2(_float(pTree->m_iWidth), _float(pTree->m_iHeight));

	// Dummy UI Object 생성.
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, wszGaemObject,
		TEXT("Layer_Tool_UI"), wszGameObjectTag, &fSize)))
	{
		MSG_BOX("Failed to Created CDummy_UI Clone");
	}

	Safe_Release(pGameInstance);
}


void CUI_Window::Interaction_UI()
{
	if (m_isOpenList || m_isObjectSelected)
		return;

	if (nullptr == m_pUILayer)
		return;
	

	const unordered_map<const _tchar*, CComponent*> Components = m_pUILayer->Get_Components();

	vector <CGameObject*> pGameObejctVector;

	for (const auto& pair : Components)
	{
		pGameObejctVector.push_back(static_cast<CGameObject*>(pair.second));
	}

	//std::sort(pGameObejctVector.begin(), pGameObejctVector.end(), [](const CGameObject* pSour, const CGameObject* pDest)
	//	{
	//		_float fSourZ = XMVectorGetZ(pSour->Get_Transform()->Get_Position());
	//		_float fDestZ = XMVectorGetZ(pDest->Get_Transform()->Get_Position());
	//		// 내림차순 (멀리있는거부터 그림.)
	//		if (fSourZ < fDestZ)
	//			return true;
	//		return false;
	//	}); 

	for (auto& pGameObject : pGameObejctVector)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (dynamic_cast<CDummy_UI*>(pGameObject)->Is_In_Rect() && 
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			m_pDummy_UI = dynamic_cast<CDummy_UI*>(pGameObject);
			Safe_Release(pGameInstance);
			break;
		}
		else if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_UP))
		{
			m_pDummy_UI = nullptr;
		}

		Safe_Release(pGameInstance);
	}

#ifdef _DEBUG
//	cout << m_pDummy_UI << endl;
#endif // _DEBUG
}

void CUI_Window::Correction_Pick()
{
	POINT CurrentMousePos;
	GetCursorPos(&CurrentMousePos);
	ScreenToClient(g_hWnd, &CurrentMousePos);

	BEGININSTANCE

		if (nullptr != m_pDummy_UI &&
			pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
			pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
		{
			_int iRoundX = CurrentMousePos.x % 10;
			if (iRoundX < 5)
				CurrentMousePos.x -= iRoundX;
			else
				CurrentMousePos.x += (10 - iRoundX);


			_int iRoundY = CurrentMousePos.y % 10;
			if (iRoundY < 5)
				CurrentMousePos.y -= iRoundY;
			else
				CurrentMousePos.y += (10 - iRoundY);

			CDummy_UI* pParent = dynamic_cast<CDummy_UI*>(m_pDummy_UI)->Get_Parent();

			if (nullptr != pParent)
			{
				_float2 vParentPos = pParent->Get_fXY();
				_float2 vPickPos;
				vPickPos.x = CurrentMousePos.x - vParentPos.x;
				vPickPos.y = CurrentMousePos.y - vParentPos.y;
				m_pDummy_UI->Set_fXY(vPickPos.x, vPickPos.y);
			}
			else
			{
				m_pDummy_UI->Set_fXY(CurrentMousePos.x, CurrentMousePos.y);
			}
		}

	ENDINSTANCE
}

void CUI_Window::Save_UI()
{
	ImGui::Begin("List Interaction");

	Open_Dialog();

	ImGui::End();
}

void CUI_Window::Add_Group(CGameObject* pGameObject)
{

	// 콤보박스 시작
	if (ImGui::BeginCombo("Type", m_isParent ? "Parent" : "Child")) {
		if (ImGui::Selectable("Parent", m_isParent == true)) {
			m_isParent = true;
		}
		if (ImGui::Selectable("Child", m_isParent == false)) {
			m_isParent = false;
		}
		ImGui::EndCombo();
	}


	_char szGroupName[MAX_PATH];
	memset(szGroupName, 0, sizeof(_char) * MAX_PATH);
	if (m_pGroupVector.size() > 0)
	{
		WCharToChar(m_pGroupVector[m_AddGroupIndex]->Get_Tag(), szGroupName);
	}
	else
	{
		const _char* nullString = " ";
		strcpy_s(szGroupName, nullString);
	}

	if (ImGui::BeginCombo("##combobox", szGroupName))
	{
		for (int i = 0; i < m_pGroupVector.size(); i++)
		{
			bool isSelected = (i == m_AddGroupIndex); // 현재 항목이 선택된 항목인지 확인합니다.

			_char szGameObjectName[MAX_PATH];
			memset(szGameObjectName, 0, sizeof(_char) * MAX_PATH);

			WCharToChar(m_pGroupVector[i]->Get_Tag(), szGameObjectName);

			// ComboBox 항목을 그립니다.
			if (ImGui::Selectable(szGameObjectName, isSelected))
			{
				m_AddGroupIndex = i; // 항목을 선택하면 선택된 항목을 업데이트합니다.
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus(); // 선택된 항목에 초점을 맞춥니다.
			}
		}

		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Button("Enter"))
	{
		m_isEnterGroupName = true;
	}
	else
	{
		m_isEnterGroupName = false;
	}

	if (m_isEnterGroupName)
	{
		_int iSize = m_pGroupVector.size();
		if (iSize <= 0 || nullptr == m_pGroupVector[m_AddGroupIndex])
		{
			MSG_BOX("Group is null");
			return;
		}
		
		if (m_isParent)
		{
			dynamic_cast<CDummy_UI*>(pGameObject)->Set_bParent();
			dynamic_cast<CDummy_UI_Group*>(m_pGroupVector[m_AddGroupIndex])->Set_Parent(dynamic_cast<CDummy_UI*>(pGameObject));
		}
		else
		{
			dynamic_cast<CDummy_UI_Group*>(m_pGroupVector[m_AddGroupIndex])->Set_Child(dynamic_cast<CDummy_UI*>(pGameObject));
		}
	}

}

void CUI_Window::Create_UI_Gruop(string _strGroupName)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	string strGroupName = _strGroupName;
	_tchar wszGroupName[MAX_PATH] = TEXT("");
	CharToWChar(strGroupName.c_str(), wszGroupName);

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, wszGroupName,
		CDummy_UI_Group::Create(m_pDevice, m_pContext, wszGroupName))))
	{
		MSG_BOX("Failed Create UI");
	}

	_int iSize = 0;
	if (nullptr != m_pUIGroupLayer)
	{
		iSize = _int(m_pUIGroupLayer->Get_Components().size());
	}

	string strGameObjectTag = strGroupName;
	_tchar wszGameObjectTag[MAX_PATH] = TEXT("");

	CharToWChar(strGameObjectTag.c_str(), wszGameObjectTag);

	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, wszGroupName,
		TEXT("Layer_Tool_UI_Group"), wszGameObjectTag)))
	{
		MSG_BOX("Failed to Created CDummy_UI Clone");
	}

	Safe_Release(pGameInstance);
}

void CUI_Window::Input_Text()
{
	if (ImGui::InputText("UI Group Name", m_szInputText, IM_ARRAYSIZE(m_szInputText), ImGuiInputTextFlags_EnterReturnsTrue))
	{
	}
	ImGui::SameLine();
	if (ImGui::Button("Enter"))
	{
		m_isEnterGroupName = true;
	}
	else
	{
		m_isEnterGroupName = false;
	}

	if (m_isEnterGroupName)
	{
		string strText = m_szInputText;
		Create_UI_Gruop(strText);

		// Enter 키를 누르면 입력된 텍스트를 객체에 설정합니다.
		memset(m_szInputText, 0, sizeof(m_szInputText));
		m_isEnterGroupName = false;
	}

}

void CUI_Window::UI_Gruop_Combo()
{
	if (nullptr == m_pUIGroupLayer)
		return;

	const unordered_map<const _tchar*, CComponent*> GameObjects = m_pUIGroupLayer->Get_Components();

	m_pGroupVector.clear();

	for (const auto& pair : GameObjects)
	{
		m_pGroupVector.push_back(static_cast<CGameObject*>(pair.second));
	}

	if (m_pGroupVector.size() <= 0)
	{
		return;
	}

	_char wszGaemObject[MAX_PATH] = {};
	WCharToChar(dynamic_cast<CDummy_UI_Group*>(m_pGroupVector[m_GroupComboIndex])->Get_GroupName(), wszGaemObject);

	if (ImGui::BeginCombo("Group List", wszGaemObject))
	{
		for (int i = 0; i < m_pGroupVector.size(); i++)
		{
			bool isSelected = (m_GroupComboIndex == i);
			const _tchar* gameObjectName = dynamic_cast<CDummy_UI_Group*>(m_pGroupVector[i])->Get_GroupName();
			WCharToChar(gameObjectName, wszGaemObject);

			if (ImGui::Selectable(wszGaemObject, isSelected))
			{
				ImGui::SetItemDefaultFocus(); // 선택한 항목을 기본으로 설정
				m_GroupComboIndex = i;
				m_pDummy_UI_Group = dynamic_cast<CDummy_UI_Group*>(m_pGroupVector[i]);
			}

			if (isSelected) 
			{
				//m_isPreGroupComboIndex = m_GroupComboIndex;
			}
		}
		ImGui::EndCombo();
	}
}

void CUI_Window::Load_UI()
{
	Open_Dialog();
}

void CUI_Window::Capture_UI(ID3D11Texture2D* pTexture, const _tchar* pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
	{
		MSG_BOX("CutScene Save Fail");
		CloseHandle(hFile);
		return;
	}

	DWORD dwByte = { 0 };

	WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	MSG_BOX("CutScene Save Success");
	CloseHandle(hFile);
}

void CUI_Window::Move_UI()
{
	// 현재 마우스 위치 얻기
	POINT CurrentMousePos;
	GetCursorPos(&CurrentMousePos);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (m_isOpenList)
	{
		if (!m_isListMouseInteraction)
		{
			m_MousePos = CurrentMousePos; // 현재 위치를 이전 위치로 저장
			Safe_Release(pGameInstance);
			return;
		}
	}

	if (nullptr == m_pDummy_UI || 
		!(pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING)))
	{
		m_MousePos = CurrentMousePos; // 현재 위치를 이전 위치로 저장
		Safe_Release(pGameInstance);
		return;
	}

	// 마우스 이동 거리 계산
	_float iMoveX = _float(CurrentMousePos.x - m_MousePos.x);
	_float iMoveY = _float(CurrentMousePos.y - m_MousePos.y);

	if (iMoveX == 0 && iMoveY == 0)
	{
		Safe_Release(pGameInstance);
		return;
	}

	_float2 fXY = m_pDummy_UI->Get_fXY();

	m_pDummy_UI->Set_fXY(fXY.x + iMoveX, fXY.y + iMoveY);
	m_MousePos = CurrentMousePos; // 현재 위치를 이전 위치로 저장

	Safe_Release(pGameInstance);
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

void CUI_Window::UI_Group_Tree()
{
	if (nullptr == m_pDummy_UI_Group)
	{
		//MSG_BOX("m_pDummy_UI_Group is NULL");
		return;
	}

	CDummy_UI* pParent = m_pDummy_UI_Group->Get_Parent();

	if (ImGui::TreeNode("Parent"))
	{
		if (pParent != nullptr)
		{
			_bool selection = false;

			_char szName[MAX_PATH];
			memset(szName, 0, sizeof(_char) * MAX_PATH);
			WCharToChar(pParent->Get_Tag(), szName);
			if (ImGui::Selectable(szName, selection))
			{
				selection = !selection;
			}
		}
		ImGui::TreePop(); // 트리 노드 닫기
	}

	vector<class CDummy_UI*>* pChilds = m_pDummy_UI_Group->Get_Childs();
	_uint iSize = (*pChilds).size();

	if (ImGui::TreeNode("Child"))
	{
		_bool* selection = new _bool[iSize];
		for (int i = 0; i < iSize; i++)
		{
			selection[i] = false;
		}


		_uint iIndex = 0;
		for (auto& iter : *pChilds)
		{
			_char szName[MAX_PATH];
			memset(szName, 0, sizeof(_char) * MAX_PATH);
			WCharToChar(iter->Get_Tag(), szName);
			if (ImGui::Selectable(szName, selection[iIndex]))
			{
				selection[iIndex] = !selection[iIndex];
			}

			iIndex++;
		}

		Safe_Delete_Array(selection);

		ImGui::TreePop(); // 트리 노드 닫기
	}
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