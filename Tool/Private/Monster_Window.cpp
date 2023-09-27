#include "..\Public\Monster_Window.h"
#include "ImGuiFileDialog.h"

#include "Dummy.h"
#include "Terrain.h"

#include "HelpMaker.h"

CMonster_Window::CMonster_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
	ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);

	// 스케일 값 1로 초기화
	for (size_t i = 0; i < 3; i++)
	{
		m_vDummyMatrix[DUMMY_SCALE][i] = 1.f;
	}
}

HRESULT CMonster_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	if (FAILED(Load_Model_Prototypes(TEXT("../../Resources/Models/Anims/"))))
	{
		MSG_BOX("Failed to Load_Model_Prototypes function");
		return S_OK;
	}

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CMonster_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Monster", nullptr, m_WindowFlag);

	// Create Dummy 버튼
	if (false == m_isCreateMonster && ImGui::Button("Create Monster##Monster"))
	{
		m_isCreateMonster = true;
	}

	if (true == m_isCreateMonster)
	{
		if (FAILED(Create_Dummy()))
		{
			MSG_BOX("Failed to Create Dummy");
		}
	}
	else
	{
		// Picking 창 On / Off
		ImGui::Checkbox("Picking", &m_isCheckPicking);
		if (true == m_isCheckPicking)
		{
			Picking_Menu();
		}

		ImGui::Separator();

		// 현재 설치되어 있는 맵 오브젝트 창 On / Off
		ImGui::Checkbox("Current MapObject", &m_isCurrentMapObject);
		if (true == m_isCurrentMapObject)
		{
			ImGui::Begin("Current MapObject", nullptr);
			Show_MonsterTable();
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
	}
	
	ImGui::End();
}

HRESULT CMonster_Window::Render()
{
	return S_OK;
}

void CMonster_Window::Picking_Menu()
{
	if (true == m_pGameInstance->Get_DIKeyState(DIK_U, CInput_Device::KEY_DOWN))
		m_isLockMouseMove = !m_isLockMouseMove;

	_float3 vPos = Find_PickingPos();

	// 현재 피킹 위치 표시
	ImGui::Text("Picking Position");
	ImGui::Text("Pressing LShift : Rounding the value");
	ImGui::Text("Follow Mouse Position Mode : "); ImGui::SameLine();
	if (true == m_isLockMouseMove)
		ImGui::Text("Disable");
	else
		ImGui::Text("Enable");

	ImGui::Text("%.1f /", vPos.x);
	ImGui::SameLine();
	ImGui::Text("%.1f /", vPos.y);
	ImGui::SameLine();
	ImGui::Text("%.1f", vPos.z);
	ImGui::Separator();

	ImGui::Text("");

	// imGui에서 값을 조정해서 변환 행렬을 만들어줌.
	ImGui::DragFloat3("Scale", m_vDummyMatrix[DUMMY_SCALE], 0.1f, 0.1f, 10.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.1f ~ 10.f");

	ImGui::DragFloat3("Rotation", m_vDummyMatrix[DUMMY_ROT], 5.f, 0.f, 360.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.f ~ 360.f");

	ImGui::DragFloat3("Translation", m_vDummyMatrix[DUMMY_TRANS], 0.1f, -50.f, 50.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-50.f ~ 50.f");

	// 상태 행렬 초기화
	if (ImGui::Button("reset"))
	{
		ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);

		// 스케일 값 1로 초기화
		for (size_t i = 0; i < 3; i++)
			m_vDummyMatrix[DUMMY_SCALE][i] = 1.f;
	}

	ImGui::Text("");

	// 조정한 상태값을 Dummy에 적용시킴
	if (nullptr != m_pCurrentDummy)
	{
		_float3 vScale;
		memcpy(&vScale, m_vDummyMatrix[DUMMY_SCALE], sizeof _float3);

		_float3 vRotation;
		memcpy(&vRotation, m_vDummyMatrix[DUMMY_ROT], sizeof _float3);

		vRotation.x = XMConvertToRadians(vRotation.x);
		vRotation.y = XMConvertToRadians(vRotation.y);
		vRotation.z = XMConvertToRadians(vRotation.z);

		// shift키를 누르고 있으면 격자에 딱 맞게 위치가 반올림됨
		if (true == m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
		{
			vPos.x = round(vPos.x);
			vPos.y = round(vPos.y);
			vPos.z = round(vPos.z);

			m_pCurrentDummy->Get_Transform()->Set_Position(vPos);
		}

		_float3 vTranslation =
		{ vPos.x + m_vDummyMatrix[DUMMY_TRANS][0], vPos.y + m_vDummyMatrix[DUMMY_TRANS][1], vPos.z + m_vDummyMatrix[DUMMY_TRANS][2] };

		m_pCurrentDummy->Get_Transform()->Set_Scale_No_Zero(vScale);
		m_pCurrentDummy->Get_Transform()->Set_Quaternion(m_pCurrentDummy->Get_Transform()->Get_QuaternionVector_RollPitchYaw(vRotation));
		m_pCurrentDummy->Get_Transform()->Set_Position(vTranslation);
	}
}

void CMonster_Window::Show_MonsterTable()
{
	// 설치되어 있는 오브젝트 리스트
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_SizingFixedFit;

	if (ImGui::BeginTable("Table_Model_Monster", 2, flags, ImVec2(0, 600), 0.f))
	{
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableHeadersRow();

		/* Model Data Table 출력 */
		_uint iIndex = 0;
		for (auto& MonsterData : m_MonsterDatas)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (ImGui::Selectable(std::to_string(iIndex).c_str(), m_iTableIndex == iIndex))
			{
				m_iTableIndex = iIndex;
				m_pCurrentDummy = MonsterData.pDummy;
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(wstrToStr(MonsterData.wstrTag).c_str());

			++iIndex;
		}
		ImGui::EndTable();
	}
}

void CMonster_Window::Save_Load_Menu()
{
	// open Dialog Simple
	// Load 경로 지정
	if (ImGui::Button("Open Load File Dialog##Monster"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseMonsterData", "Choose File", ".mon",
			"../../Resources/GameData/MonsterData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseMonsterData"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// 주소 가공
			m_strLoadFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}
	wstring wstrLoadFilePaht = strToWStr(m_strLoadFilePath);

	// 현재 로드 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), m_strLoadFilePath.c_str());

	// 로드 버튼 처리
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_Monsters(wstrLoadFilePaht)))
			MSG_BOX("Failed to Load Monster on Menu");
	}

	ImGui::Separator();

	string strDataName = { "" };
	ImGui::InputTextWithHint("SaveData Path", "enter SaveDataName.mon", m_szDataName, IM_ARRAYSIZE(m_szDataName));

	string strSaveFilePath = ("../../Resources/GameData/MonsterData/");
	strSaveFilePath = strSaveFilePath + m_szDataName + ".mon";

	// 현재 세이브 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), strSaveFilePath.c_str());

	// 세이브 버튼 처리
	if (ImGui::Button("Save"))
	{
		string strDataName = m_szDataName;
		if (FAILED(Save_Monsters(strToWStr(strDataName))))
			MSG_BOX("Failed to Save MapObject on Menu");
	}
}

HRESULT CMonster_Window::Save_Monsters(const wstring& wstrMapDataName)
{
	wstring wstrFilePath = TEXT("../../Resources/GameData/MonsterData/");

	wstrFilePath += wstrMapDataName + TEXT(".mon");

	HANDLE hFile = CreateFile(wstrFilePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Save MapObject");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	for (auto& MonsterData : m_MonsterDatas)
	{
		/* Write Monster WorldMatrix */
		if (!WriteFile(hFile, MonsterData.pDummy->Get_Transform()->Get_WorldMatrixPtr(), sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.vPos");
			return E_FAIL;
		}
		wstring wstrMonsterTag = MonsterData.wstrTag;
		wstring wstrModelFilePath = MonsterData.wstrModelFilePath;
		wstring wstrPrototypeModelTag = MonsterData.wstrPrototypeModelTag;

		/* Write Monster Tag */
		_uint iLength = wstrMonsterTag.size();
		if (!WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrMonsterTag.iTagLen");
			return E_FAIL;
		}
		_tchar szWriteData[MAX_PATH] = { TEXT("") };
		lstrcpy(szWriteData, wstrMonsterTag.c_str());
		if (!WriteFile(hFile, szWriteData, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrMonsterTag.wszTag");
			return E_FAIL;
		}

		/* Write Monster PrototypeModelTag */
		 iLength = wstrPrototypeModelTag.size();
		if (!WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrPrototypeModelTag.iTagLen");
			return E_FAIL;
		}
		ZEROMEM(szWriteData);
		lstrcpy(szWriteData, wstrPrototypeModelTag.c_str());
		if (!WriteFile(hFile, szWriteData, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrPrototypeModelTag.wszTag");
			return E_FAIL;
		}

		/* Write Monster ModelFilePath */
		iLength = wstrModelFilePath.size();
		if (!WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrModelFilePath.iTagLen");
			return E_FAIL;
		}
		ZEROMEM(szWriteData);
		lstrcpy(szWriteData, wstrModelFilePath.c_str());
		if (!WriteFile(hFile, szWriteData, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write wstrModelFilePath.wszTag");
			return E_FAIL;
		}
	}

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CMonster_Window::Load_Monsters(const wstring& wstrMapDataPath)
{
	HANDLE hFile = CreateFile(wstrMapDataPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD	dwByte = 0;
	vector<MONSTERDATA> vecDatas;

	while (true)
	{
		MONSTERDATA SaveDesc;

		/* Read Monster WorldMatrix */
		if (!ReadFile(hFile, &SaveDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.matTransform");
			CloseHandle(hFile);
			return E_FAIL;
		}

		/* Read Monster Tag */
		_uint iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		_tchar szTag[MAX_PATH] = { TEXT("") };
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		SaveDesc.wstrTag = szTag;

		/* Read Monster PrototypeModelTag */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(szTag);
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		SaveDesc.wstrPrototypeModelTag = szTag;

		/* Read Monster ModelFilePath */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(szTag);
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		SaveDesc.wstrModelFilePath = szTag;

		if (dwByte == 0)
		{
			break;
		}

		vecDatas.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	_uint iIndex = 0;
	// 로드한 데이터를 적용시켜 주는 부분
	for (auto& Data : vecDatas)
	{
		// 맵 오브젝트에 번호 붙여줌
		wstring wstrName = TEXT("GameObject_Monster_");
		wstrName += Data.wstrTag + to_wstring(iIndex++);

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_Dummy"), TEXT("Layer_Monster"),
			wstrName.c_str())))
		{
			MSG_BOX("Failed to Install MapObject");
			return E_FAIL;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		Data.pDummy = static_cast<CDummy*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_Monster"), wstrName.c_str()));
		if (nullptr == Data.pDummy)
		{
			MSG_BOX("pDummy is nullptr");
			return S_OK;
		}
		Safe_AddRef(Data.pDummy);
		Data.pDummy->Get_Transform()->Set_WorldMatrix(Data.matTransform);
		Data.pDummy->Add_Model_Component(Data.wstrPrototypeModelTag.c_str());
		Data.pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
	}
	m_MonsterDatas.insert(m_MonsterDatas.end(), vecDatas.begin(), vecDatas.end());

	return S_OK;
}

_float3 CMonster_Window::Find_PickingPos()
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	if (true == m_isLockMouseMove)
	{
		return m_vLockingMousePos;
	}

	// 마우스가 화면 밖에 있으면 검색 안함
	if (false == m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		return _float3(-1.f, -1.f, -1.f);
	}

	m_pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer());

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

			m_vLockingMousePos.x = vFinalPos.x;
			m_vLockingMousePos.y = vFinalPos.y;
			m_vLockingMousePos.z = vFinalPos.z;

			return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
		}
	}

	return _float3(-1.f, -1.f, -1.f);
}

HRESULT CMonster_Window::Create_Dummy()
{
	ImGui::Text("1. Select Monster");
	ImGui::Text("2. Press 'H' Button");
	ImGui::Text("3. Pick Terrain");

	ImGui::Separator();

	ImGui::Text("Monster List");
	if(ImGui::BeginListBox("##Monster Prototype List"))
	{
		_int iIndex = { 0 };
		for (auto& Tag : m_wstrMonsterTags)
		{
			if(ImGui::Selectable(wstrToStr(Tag).c_str(), false, 0, ImVec2(0, 20)) || 
				m_iPrototypeTableIndex == iIndex)
			{
				m_iPrototypeTableIndex = iIndex;
				ImGui::SameLine();
				ImGui::Text(" - Selected");
			}
			++iIndex;
		}
		ImGui::EndListBox();
	}

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

	if (m_pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_PRESSING))
	{
		m_isLockMouseMove = false;

		if(_float3(-1.f, -1.f, -1.f) != vPos && 
			m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			wstring wstrComponentTag = { TEXT("GameObject_Monster_") };
			wstrComponentTag += m_wstrMonsterTags[m_iPrototypeTableIndex] + to_wstring(m_iCountIndex++);

			if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy"),
				TEXT("Layer_Monster"), wstrComponentTag.c_str())))
			{
				MSG_BOX("[CMonster_Window] Failed Add_Component");
				return S_OK;
			}

			MONSTERDATA MonsterData;
			MonsterData.wstrModelFilePath = m_wstrModelPaths[m_iPrototypeTableIndex];
			MonsterData.wstrPrototypeModelTag = m_wstrPrototypeModelTags[m_iPrototypeTableIndex];
			MonsterData.wstrTag = m_wstrMonsterTags[m_iPrototypeTableIndex];
			MonsterData.pDummy = static_cast<CDummy*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
				TEXT("Layer_Monster"), wstrComponentTag.c_str()));
			if (nullptr == MonsterData.pDummy)
			{
				MSG_BOX("[CMonster_Window] pDummy is nullptr");
				return S_OK;
			}

			MonsterData.pDummy->Add_Model_Component(m_wstrPrototypeModelTags[m_iPrototypeTableIndex].c_str());
			MonsterData.pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
			MonsterData.pDummy->Get_Transform()->Set_Position(vPos);

			m_MonsterDatas.push_back(MonsterData);
			Safe_AddRef(MonsterData.pDummy);
		}
	}
	else
		m_isLockMouseMove = true;

	if (ImGui::Button("Exit Create Monster"))
		m_isCreateMonster = false;

	return S_OK;
}

HRESULT CMonster_Window::Delete_Dummy()
{
	// 이미 더미가 없다면 리턴
	if (nullptr == m_pCurrentDummy)
	{
		MSG_BOX("Dummy Already deleted");
		return S_OK;
	}

	if (FAILED(m_pGameInstance->Delete_Component(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Dummy"))))
	{
		MSG_BOX("Failed to delete Dummy");
		return E_FAIL;
	}

	m_pCurrentDummy = nullptr;

	return S_OK;
}

HRESULT CMonster_Window::Load_Model_Prototypes(const wstring& wstrFilePath)
{// 디렉토리 경로를 순회할 iterator
	fs::directory_iterator iter(fs::absolute(wstrFilePath));

	while (iter != fs::end(iter))
	{
		// 실제 디렉토리 경로를 담고있는 변수 (iterator의 원본)
		const fs::directory_entry& entry = *iter;

		// 현재 entry 변수가 디렉토리인지 확인 후 디렉토리이면 재귀
		if (fs::is_directory(entry.path()))
		{
			Load_Model_Prototypes(entry.path().c_str());
		}
		else
		{
			// dat파일 체크
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".gcm")))
			{
				if (false == fs::exists(entry.path()))
				{
					MSG_BOX("Failed to find dat file");
					return E_FAIL;
				}

				wstring pathresult = entry.path().wstring();
				m_wstrModelPaths.push_back(pathresult);

				// 경로에서 모델 이름 부분만 잘라내는 부분
				size_t path_length = pathresult.length();
				size_t current = pathresult.find(TEXT("Models\\Anims")) + 13;

				// 1차 분리, 여기서 모델 이름 파일 경로가 나와야 함.
				wstring result = pathresult.substr(current, path_length);

				// 파일명에 Lod가 들어가 있는지 찾는다.
				if (std::wstring::npos != result.find((TEXT("_Lod"))))
				{
					// 만약 있다면 프로토 타입을 만들지 않는다.
					iter++;
					continue;
				}

				size_t current1 = result.find(TEXT("\\"));

				// 2차 분리, 여기서 모델 이름이 나와야 함
				wstring result1 = result.substr(0, current1);

				if (wstring::npos != result1.find(TEXT("Golem")))
				{
				}
				else if (wstring::npos != result1.find(TEXT("Dugbog")))
				{
				}
				else if (wstring::npos != result1.find(TEXT("Troll")))
				{
				}
				else
				{
					m_wstrModelPaths.pop_back();
					++iter;
					continue;
				}

				// 모델 리스트에 출력할 문자열 모음에 넣음
				m_wstrMonsterTags.push_back(result1);

				// 이제 컴포넌트 모델 이름으로 결합
				wstring wstrPrototypeTag = (TEXT("Prototype_Component_Model_"));
				wstrPrototypeTag += result1;
				m_wstrPrototypeModelTags.push_back(wstrPrototypeTag);

				// 프로토타입 생성
				_float4x4 PivotMatrix;
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wstrPrototypeTag.c_str(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, m_wstrModelPaths.back().c_str(), PivotMatrix), true)))
				{
					MSG_BOX("Failed to Create New Model Prototype");
				}
			}
		}

		iter++;
	}

	return S_OK;
}

CMonster_Window* CMonster_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CMonster_Window* pInstance = New CMonster_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CMonster_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMonster_Window::Free(void)
{
	__super::Free();

	for (auto& MonsterData : m_MonsterDatas)
		Safe_Release(MonsterData.pDummy);
}
