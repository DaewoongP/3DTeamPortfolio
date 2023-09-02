#include "..\Public\Object_Window.h"
#include "ImGuiFileDialog.h"

#include "VIBuffer_Terrain.h"

#include "Layer.h"

#include "MapDummy.h"
#include "MapObject.h"
#include "Terrain.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	// 0이 Non_Anim이다.
	if (FAILED(Save_Model_Path(0, TEXT("../../Resources/Models/NonAnims/"))))
		return E_FAIL;

	if (FAILED(Create_Dummy()))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	// Picking 창 On / Off
	ImGui::Checkbox("Picking", &m_isCheckPicking);
	if(true == m_isCheckPicking)
	{
		Picking_Menu();
	}

	ImGui::Separator();

	// Model 선택 창 On / Off
	ImGui::Checkbox("Model Select", &m_isSelectModel);
	if (true == m_isSelectModel)
	{
		Select_Model();
	}

	ImGui::Separator();

	// 현재 설치되어 있는 맵 오브젝트 창 On / Off
	ImGui::Checkbox("Current MapObject", &m_isCurrentMapObject);
	if (true == m_isCurrentMapObject)
	{
		ImGui::Begin("Current MapObject", nullptr);
		Current_MapObject();
		ImGui::End();
	}

	ImGui::Separator();

	// Save Load 선택 창 On / Off
	ImGui::Checkbox("Save Load", &m_isSaveLoad);
	if (true == m_isSaveLoad)
	{
		Save_Load_Menu();
	}

	ImGui::Separator();

	ImGui::End();
}

HRESULT CObject_Window::Render()
{
	return S_OK;
}

void CObject_Window::Picking_Menu()
{
	_float3 vPos = Find_PickingPos();

	// 현재 피킹 위치 표시
	ImGui::Text("Picking Position");
	ImGui::Text("Pressing LShift : Rounding the value");
	ImGui::Text("%.1f /", vPos.x);
	ImGui::SameLine();
	ImGui::Text("%.1f /", vPos.y);
	ImGui::SameLine();
	ImGui::Text("%.1f", vPos.z);
	ImGui::Separator();

	// 마우스 좌클릭을 하면 해당 위치로 더미 이동
	BEGININSTANCE; if (true == pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		-1.f != vPos.x)
	{
		// shift키를 누르고 있으면 격자에 딱 맞게 위치가 반올림됨
		if (true == pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
		{
			vPos.x = round(vPos.x);
			vPos.y = round(vPos.y);
			vPos.z = round(vPos.z);

			m_pDummy->Set_Pos(vPos);
		}

		else
		{
			m_pDummy->Set_Pos(vPos);
		}		
	} ENDINSTANCE;

	// Object Install 선택 창 On / Off
	ImGui::Checkbox("Object Install", &m_isInstallObject);
	if (true == m_isInstallObject)
	{
		Install_Object(m_pDummy->Get_Transform()->Get_Position());
	}
}

void CObject_Window::Install_Object(_float3 vPos)
{
	ImGui::Text("Press H to install");

	// 범위 안에 있을 경우 H키를 눌러 설치
	BEGININSTANCE; if (true == pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		-1.f != vPos.x)
	{
		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"), 
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &vPos)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, 
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));

		// 저장용 벡터에 넣어준다.
		SAVEOBJECTDESC SaveDesc;

		SaveDesc.vPos = vPos;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
		SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

		m_vecSaveObject.push_back(SaveDesc);

		++m_iMapObjectIndex;

	} ENDINSTANCE;

	// 오브젝트 삭제 관련 메뉴
	Delete_Object_Menu();
}

void CObject_Window::Select_Model()
{
	// 현재 선택된 모델이 무엇인지 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Current Model");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), m_strCurrentModel.c_str());

	// 모델 선택
	ImGui::ListBox("ModelList", &m_iModelIndex, VectorGetter, 
		static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);

	// open Dialog Simple
	// 모델을 골라 프로토타입 만들어주는 부분
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose File", ".dat",
			"../../Resources/Models/NonAnims/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// 주소 가공
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			_char fileName[MAX_PATH] = "";
			_splitpath_s(strFilePathName.c_str(), nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

			_tchar wszfilePath[MAX_PATH] = {};
			_tchar wszfileName[MAX_PATH] = {};
			CharToWChar(strFilePathName.c_str(), wszfilePath);
			CharToWChar(fileName, wszfileName);
			_tchar wszModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_");
			lstrcat(wszModelTag, wszfileName);

			// _tchar형태의 문자열을 string으로 변환
			_char szTag[MAX_PATH] = "";
			WCharToChar(wszModelTag, szTag);
			m_strCurrentModel.clear();
			m_strCurrentModel.append(szTag);

			m_vecModelList.push_back(m_strCurrentModel);
			Deep_Copy_Name();
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// 선택된 모델을 적용
	if (ImGui::Button("SelectModel"))
	{
		if (FAILED(m_pDummy->Change_Model_Component(m_vecModelList_t.at(m_iModelIndex))))
		{
			MSG_BOX("Failed to Change Model");
			return;
		}

		CModel* pDummyModel = static_cast<CModel*>(m_pDummy->Find_Component(TEXT("Com_Buffer")));

		// 현재 선택된 모델 이름 갱신
		_char szName[MAX_PATH];
		WCharToChar(m_vecModelList_t.at(m_iModelIndex), szName);
		m_strCurrentModel.clear();
		m_strCurrentModel.append(szName);
	}
}

void CObject_Window::Current_MapObject()
{
	// 설치되어 있는 오브젝트 리스트
	ImGui::ListBox("Map Object List", &m_iTagIndex, VectorGetter,
		static_cast<void*>(&m_vecObjectTag_s), (_int)m_vecObjectTag_s.size(), 20);
}

void CObject_Window::Save_Load_Menu()
{
	// 세이브 버튼 처리
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_MapObject()))
			MSG_BOX("Failed to Save MapObject on Menu");
	}

	ImGui::SameLine();

	// 로드 버튼 처리
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_MapObject()))
			MSG_BOX("Failed to Load MapObject on Menu");
	}
}

void CObject_Window::Delete_Object_Menu()
{
	// 마지막에 설치한 오브젝트를 제거
	if (ImGui::Button("Undo"))
	{

	}

	ImGui::SameLine();

	// 설치된 전체 오브젝트 제거
	if (ImGui::Button("Delete All"))
	{
		BEGININSTANCE;
		if (FAILED(pGameInstance->Clear_Layer((_uint)LEVEL_TOOL, TEXT("Layer_MapObject"))))
		{
			MSG_BOX("Failed to clear MapObject");
		} ENDINSTANCE;

		// 값 초기화
		m_iMapObjectIndex = 0;

		m_vecSaveObject.clear();
		m_vecObjectTag_s.clear();
		m_vecMapObjectTag.clear();
	}
}

HRESULT CObject_Window::Save_MapObject()
{
	_tchar dataFile[MAX_PATH] = TEXT("../../Resources/MapData/MapObject.ddd");

	HANDLE hFile = CreateFile(dataFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Save MapObject");
	}

	DWORD	dwByte = 0;

	for (auto& iter : m_vecSaveObject)
	{
		if (!WriteFile(hFile, &iter.vPos, sizeof(_float3), &dwByte, nullptr))
			MSG_BOX("Failed to Write m_vecSaveObject.vPos");

		if (!WriteFile(hFile, &iter.iTagLen, sizeof(_uint), &dwByte, nullptr))
			MSG_BOX("Failed to Write m_vecSaveObject.iTagLen");

		if (!WriteFile(hFile, &iter.wszTag, iter.iTagLen, &dwByte, nullptr))
			MSG_BOX("Failed to Write m_vecSaveObject.wszTag");
	}

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CObject_Window::Load_MapObject()
{
	m_vecSaveObject.clear();

	_tchar dataFile[MAX_PATH] = TEXT("../../Resources/MapData/MapObject.ddd");

	HANDLE hFile = CreateFile(dataFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
	}

	DWORD	dwByte = 0;
	SAVEOBJECTDESC SaveDesc;
	ZEROMEM(&SaveDesc);

	while (true)
	{
		if (!ReadFile(hFile, &SaveDesc.vPos, sizeof(_float3), &dwByte, nullptr))
			MSG_BOX("Failed to Read m_vecSaveObject.vPos");

		if (!ReadFile(hFile, &SaveDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen");

		if (!ReadFile(hFile, &SaveDesc.wszTag, SaveDesc.iTagLen, &dwByte, nullptr))
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");

		if (dwByte == 0)
		{
			break;
		}

		m_vecSaveObject.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	// 로드한 데이터를 적용시켜 주는 부분
	for (size_t i = 0; i < m_vecSaveObject.size(); i++)
	{
		_uint iCount = 0;

		// m_vecModelList_t를 순회하며 중복되는 문자열이 있는지 체크
		for (auto& iter : m_vecModelList_t)
		{
			if (0 == lstrcmp(iter, m_vecSaveObject[i].wszTag))
			{
				++iCount;
			}
		}

		// 중복되는 문자열이 없다면 m_vecModelList_t에 모델 이름 삽입
		if (0 == iCount) 
		{
			m_vecModelList_t.push_back(Deep_Copy(m_vecSaveObject[i].wszTag));
		}

		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		// 번호를 붙인 태그로 MapObject 등록
		BEGININSTANCE if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &m_vecSaveObject[i].vPos)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return E_FAIL;
		} ENDINSTANCE;

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecSaveObject[i].wszTag);
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));

		++m_iMapObjectIndex;
	}	

	return S_OK;
}

void CObject_Window::Deep_Copy_Name(const _tchar* wszName)
{
	size_t length = 0;
	wstring ws;

	// 모델 이름을 const _tchar* 형태로 깊은 복사
	if (nullptr != wszName)
	{
		length = wcslen(wszName);
		ws.append(wszName);
	}

	else
	{
		length = m_strCurrentModel.size();
		ws.append(m_strCurrentModel.begin(), m_strCurrentModel.end());
	}

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Name)");
		return;
	}

	m_vecModelList_t.push_back(wszNew);
}

void CObject_Window::Deep_Copy_Path(const _tchar* wszPath)
{
	// 모델 경로를 const _tchar* 형태로 깊은 복사
	size_t length = wcslen(wszPath);

	wstring ws(wszPath);

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Path)");
		return;
	}

	_uint iCount = 0;

	for (auto& iter : m_vecModelPath_t)
	{
		// 중복되는 문자열이 있을 경우 증가
		if (0 == lstrcmp(wszNew, iter))
		{
			++iCount;
		}
	}
	
	// 중복이라면 저장하지 않는다.
	if (0 == iCount)
	{
		m_vecModelPath_t.push_back(wszNew);
	}

	else
	{
		delete[] wszNew;
	}		
}

void CObject_Window::Deep_Copy_Tag(const _tchar* wszTag)
{
	m_vecMapObjectTag.push_back(wszTag);

	// 깊은 복사는 안하고 변환만 해줌
	size_t length = wcslen(wszTag);

	char c[MAX_PATH] = "";;
	WCharToChar(wszTag, c);

	string s(c);

	m_vecObjectTag_s.push_back(s);
}

const _tchar* CObject_Window::Deep_Copy(const _tchar* wszString)
{
	// 맵 오브젝트 경로를 const _tchar* 형태로 깊은 복사
	size_t length = wcslen(wszString);

	wstring ws(wszString);

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(String)");
		return nullptr;
	}

	return wszNew;
}

_float3 CObject_Window::Find_PickingPos()
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

	float fDist = FLT_MAX; // 피킹 연산 후 최종 거리값

	_bool bResult = pTerrain->IsPicked(vRayPos, vRayDir, fDist);

	// 결과가 나올 경우 RayDir에 거리값을 곱해 최종 위치 산출
	if (true == bResult)
	{
		if (FLT_MAX > fDist)
		{
			_float4 vFinalPos;

			vRayDir *= fDist;
			vFinalPos = vRayPos + vRayDir;

			return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
		}
	}	

	return _float3(-1.f, -1.f, -1.f);
}

HRESULT CObject_Window::Create_Dummy()
{
	_float3 vPos = { 5.f, 0.f, 5.f };

	BEGININSTANCE; if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_MapDummy"), 
		TEXT("Layer_Tool"), TEXT("Map_Dummy"), &vPos)))
	{
		MSG_BOX("Failed to GameObject Map_Dummy");
		return E_FAIL;
	}

	m_pDummy = static_cast<CMapDummy*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Map_Dummy")));
	m_pDummy->Add_Model_Component(TEXT("Prototype_Component_Model_Tree"));
	m_pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh")); ENDINSTANCE;

	return S_OK;
}

HRESULT CObject_Window::Save_Model_Path(_uint iType, const _tchar* pFilePath)
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
			Save_Model_Path(0, entry.path().c_str());
		}
		else
		{
			// fbx파일 체크
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".fbx")) ||
				!lstrcmp(entry.path().extension().c_str(), TEXT(".FBX")))
			{
				// .fbx를 .dat로 변경
				size_t _dat = entry.path().string().find(".");
				wstring pathresult = entry.path().wstring().substr(0, _dat);
				wstring datext = TEXT(".dat");
				pathresult += datext;

				Deep_Copy_Path(pathresult.c_str());

				// 경로에서 모델 이름 부분만 잘라내는 부분
				string s = entry.path().string();

				size_t path_length = pathresult.length();
				size_t current = s.find("NonAnim") + 9;

				// 1차 분리, 여기서 모델 이름 파일 경로가 나와야 함.
				string result = s.substr(current, path_length);

				size_t current1 = result.find("\\");

				// 2차 분리, 여기서 모델 이름이 나와야 함
				string result1 = result.substr(0, current1);

				// 이제 컴포넌트 모델 이름으로 결합
				string modelname = ("Prototype_Component_Model_");
				modelname += result1;
				m_vecModelList.push_back(modelname);

				wstring wmodelname(modelname.begin(), modelname.end());
				Deep_Copy_Name(wmodelname.c_str());

				// 프로토타입 생성
				_float4x4 PivotMatrix = XMMatrixIdentity();
				BEGININSTANCE; if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, m_vecModelList_t.back(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, m_vecModelPath_t.back(), PivotMatrix))))
				{
					MSG_BOX("Failed to Create New Model Prototype");
				} ENDINSTANCE;
			}
		}

		iter++;
	}

	return S_OK;
}

CObject_Window* CObject_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CObject_Window* pInstance = New CObject_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CObject_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CObject_Window::Free(void)
{
	__super::Free();

	// 동적 할당 해줬던 문자열 해제
	for (auto& iter : m_vecModelList_t)
	{
		delete[] iter;
	}
	m_vecModelList_t.clear();

	for (auto& iter : m_vecModelPath_t)
	{
		delete[] iter;
	}
	m_vecModelPath_t.clear();

	m_vecObjectTag_s.clear();
	m_vecMapObjectTag.clear();
}
