#include "..\Public\Object_Window.h"
#include "ImGuiFileDialog.h"

#include "MapDummy.h"
#include "MapObject.h"
#include "MapObject_Ins.h"
#include "Terrain.h"

#include "HelpMaker.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
	ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);
	ZeroMemory(&m_vChangeMapObject, sizeof m_vChangeMapObject);

	// 스케일 값 1로 초기화
	for (size_t i = 0; i < 3; i++)
	{
		m_vDummyMatrix[DUMMY_SCALE][i] = 1.f;
		m_vChangeMapObject[DUMMY_SCALE][i] = 1.f;
	}
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	// 0이 Non_Anim이다.
	if (FAILED(Save_Model_Path(0, TEXT("../../Resources/Models/MapObject/NonAnims/"))))
	{
		MSG_BOX("Failed to Save_Model_Path function");
		return S_OK;
	}

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	ImGui::Text("Press U to Unlock Mouse Move");

	// Create Dummy 버튼
	if (ImGui::Button("Create Dummy"))
	{
		if (FAILED(Create_Dummy()))
		{
			MSG_BOX("Failed to Create Dummy");
		}
	}

	ImGui::SameLine();

	// Delete Dummy 버튼
	if (ImGui::Button("Delete Dummy"))
	{
		if (FAILED(Delete_Dummy()))
		{
			MSG_BOX("Failed to Delete Dummy");
		}
	}

	// Picking 창 On / Off
	ImGui::Checkbox("Picking", &m_isCheckPicking);
	if (true == m_isCheckPicking)
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
		// 새창으로 띄우면 프레임 떨어져서 주석처리함
		//ImGui::Begin("Current MapObject", nullptr);
		Current_MapObject();
		//ImGui::End();
	}

	ImGui::Separator();

	// Save Load 선택 창 On / Off
	ImGui::Checkbox("Save Load", &m_isSaveLoad);
	if (true == m_isSaveLoad)
	{
		Save_Load_Menu();
	}

	ImGui::Separator();

	// 인스턴스 Save Load 선택 창 On / Off
	ImGui::Checkbox("Instance Save Load", &m_isInsSaveLoad);
	if (true == m_isInsSaveLoad)
	{
		Ins_Save_Load_Menu();
	}

	ImGui::Separator();

	//// 맵 브러싱 메뉴 창 On / Off
	//ImGui::Checkbox("Map Brushing", &m_isBrushingMap);
	//if (true == m_isBrushingMap)
	//{
	//	Map_Brushing_Menu();
	//}

	// 맵 오브젝트 전체 이동 메뉴 On / Off
	ImGui::Checkbox("All MapObject Translation", &m_isAllTranslation);
	if (true == m_isAllTranslation)
	{
		All_Map_Object_Translation();
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
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return;
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

	ImGui::Text("Choice Install Method");
	ImGui::RadioButton("1Click One_Install", &m_iInstallMethod, 0);
	ImGui::RadioButton("1Pressing Continuius_Install", &m_iInstallMethod, 1);
	ImGui::RadioButton("2Click Random_Install in Range", &m_iInstallMethod, 2);
	ImGui::RadioButton("1Click One_Install for Instance", &m_iInstallMethod, 3);

	ImGui::Text("");

	// imGui에서 값을 조정해서 변환 행렬을 만들어줌.
	ImGui::DragFloat3("Scale", m_vDummyMatrix[DUMMY_SCALE], 0.1f, -10.f, 10.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-10.f ~ 10.f");

	ImGui::DragFloat3("Rotation", m_vDummyMatrix[DUMMY_ROT], 5.f, -360.f, 360.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-360.f ~ 360.f");

	ImGui::DragFloat3("Translation", m_vDummyMatrix[DUMMY_TRANS], 1.f, -500.f, 500.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-500.f ~ 500.f");

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
	if (nullptr != m_pDummy)
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

			m_pDummy->Set_Pos(vPos);
		}

		_float3 vTranslation =
		{ vPos.x + m_vDummyMatrix[DUMMY_TRANS][0], vPos.y + m_vDummyMatrix[DUMMY_TRANS][1], vPos.z + m_vDummyMatrix[DUMMY_TRANS][2] };

		m_pDummy->Get_Transform()->Set_Scale_No_Zero(vScale);
		m_pDummy->Get_Transform()->Set_Quaternion_With_Negative(m_pDummy->Get_Transform()->Get_QuaternionVector_RollPitchYaw(vRotation));
		m_pDummy->Get_Transform()->Set_Position(vTranslation);
	}

	// Object Install 선택 창 On / Off
	ImGui::Checkbox("Object Install", &m_isInstallObject);
	if (true == m_isInstallObject && nullptr != m_pDummy)
	{
		switch (m_iInstallMethod)
		{
		case ONE_METHOD: // 클릭 당 한 번 설치
			Install_Object(m_pDummy->Get_Transform()->Get_Position());
			break;

		case CONTINUOUS_METHOD: // 누르고 있으면 쭉 설치
			Install_Continuous_Object(m_pDummy->Get_Transform()->Get_Position());
			break;

		case RANDOM_METHOD: // 일정 범위를 지정해주면 그 안에서 랜덤 설치
			Install_Random_Object(m_pDummy->Get_Transform()->Get_Position());
			break;

		case MULTI_METHOD: // 한번에 여러개 설치(인스턴싱)
			Install_Multi_Object(m_pDummy->Get_Transform()->Get_Position());
			break;
		}

		// 오브젝트 삭제 관련 메뉴
		Delete_Object_Menu();
	}
}

void CObject_Window::Install_Object(_float3 vPos)
{
	ImGui::Text("Press H to Install");

	// 범위 안에 있을 경우 H키를 눌러 설치
	if (true == m_pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x)
	{
		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iModelCnt));
		Deep_Copy_Tag(wszobjName);
		++m_iModelCnt;

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.back().c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject");
			return;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		if (nullptr == m_pObject)
		{
			MSG_BOX("Failed to Find MapObject with Tag 5");
			return;
		}

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌

		// 저장용 벡터에 넣어준다.
		SAVEOBJECTDESC SaveDesc;

		SaveDesc.matTransform = vWorldMatrix;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
		SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

		m_vecSaveObject.push_back(SaveDesc);

		++m_iMapObjectIndex;
	}
}

void CObject_Window::Install_Continuous_Object(_float3 vPos)
{
	m_fTimeAcc += m_pGameInstance->Get_World_Tick();

	// 마우스를 꾹 누르고 있으면 연속 설치
	if (true == m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x &&
		0.1f < m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;

		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iModelCnt));
		Deep_Copy_Tag(wszobjName);
		++m_iModelCnt;

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.back().c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject");
			return;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		if (nullptr == m_pObject)
		{
			MSG_BOX("Failed to Find MapObject with Tag 4");
			return;
		}

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌

		// 저장용 벡터에 넣어준다.
		SAVEOBJECTDESC SaveDesc;

		SaveDesc.matTransform = vWorldMatrix;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
		SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

		m_vecSaveObject.push_back(SaveDesc);

		++m_iMapObjectIndex;
	}
}

void CObject_Window::Install_Random_Object(_float3 vPos)
{
	ImGui::Text("Press H to Install");

	// 범위 안에 있을 경우 H키를 눌러 설치
	if (true == m_pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x)
	{
		m_vRandomInstall[m_iRandomInstall_Seq] = vPos;
		++m_iRandomInstall_Seq;

		// 두 포지션이 정했다면
		if (2 == m_iRandomInstall_Seq)
		{
			_float fL, fR, fT, fB;

			fL = min(m_vRandomInstall[0].x, m_vRandomInstall[1].x);
			fR = max(m_vRandomInstall[0].x, m_vRandomInstall[1].x);
			fT = max(m_vRandomInstall[0].z, m_vRandomInstall[1].z);
			fB = min(m_vRandomInstall[0].z, m_vRandomInstall[1].z);

			// 범위 내 랜덤한 곳에 설정한 수만큼 모델 설치
			for (size_t i = 0; i < 5; i++)
			{

				// 맵 오브젝트에 번호 붙여줌
				_tchar wszobjName[MAX_PATH] = { 0 };
				_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iModelCnt));
				Deep_Copy_Tag(wszobjName);
				++m_iModelCnt;

				_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

				_uint iX = _uint(fR - fL);
				_uint iY = _uint(fT - fB);
				if (0 == iX ||
					0 == iY)
				{
					return;
				}

				vWorldMatrix._41 = fL + (rand() % (_uint)(fR - fL));
				vWorldMatrix._43 = fB + (rand() % (_uint)(fT - fB));

				// 번호를 붙인 태그로 MapObject 등록
				if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
					TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
					m_vecMapObjectTag.back().c_str(), &vWorldMatrix)))
				{
					MSG_BOX("Failed to Install MapObject");
					return;
				}

				// 마지막에 설치한 맵 오브젝트 주소 가져옴
				m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
					TEXT("Layer_MapObject"), wszobjName));

				if (nullptr == m_pObject)
				{
					MSG_BOX("Failed to Find MapObject with Tag 3");
					return;
				}

				m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
				m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
				m_pObject->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌

				// 저장용 벡터에 넣어준다.
				SAVEOBJECTDESC SaveDesc;

				SaveDesc.matTransform = vWorldMatrix;
				lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
				SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

				m_vecSaveObject.push_back(SaveDesc);

				++m_iMapObjectIndex;
			}

			m_iRandomInstall_Seq = 0;
		}
	}
}

void CObject_Window::Install_Multi_Object(_float3 vPos)
{
	ImGui::Text("Press H to Install");

	ImGui::Text("There is %d Instance Model", m_iPushBackInsObject);

	// 범위 안에 있을 경우 H키를 눌러 설치
	if (true == m_pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		-1.f != vPos.x)
	{
		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (m_iModelCnt));
		Deep_Copy_Tag(wszobjName);
		++m_iModelCnt;

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.back().c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject");
			return;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		if (nullptr == m_pObject)
		{
			MSG_BOX("Failed to Find MapObject with Tag 1");
			return;
		}

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌

		++m_iMapObjectIndex;
		++m_iInsObjectCnt;
	}

	// 설치한 맵 오브젝트들을 인스턴스 모델로 묶음
	if (ImGui::Button("push_back"))
	{
		vector<_float4x4> vecWorldMatrix;

		for (_int i = m_iMapObjectIndex - m_iInsObjectCnt; i < m_iMapObjectIndex; ++i)
		{
			// 해당 게임 오브젝트를 찾음
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (i));

			CMapObject* pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
				TEXT("Layer_MapObject"), wszobjName));

			if (nullptr == pObject)
			{
				MSG_BOX("Failed to Find MapObject with Tag 2");
				return;
			}

			vecWorldMatrix.push_back(pObject->Get_Transform()->Get_WorldMatrix());
		}

		SAVEINSOBJECTDESC SaveDesc;

		SaveDesc.iInstanceCnt = m_iInsObjectCnt;

		SaveDesc.pMatTransform = New _float4x4[SaveDesc.iInstanceCnt];
		for (size_t i = 0; i < SaveDesc.iInstanceCnt; i++)
		{
			SaveDesc.pMatTransform[i] = vecWorldMatrix.at(i);
		}

		SaveDesc.matTransform = XMMatrixIdentity();
		SaveDesc.iTagLen = lstrlen(m_vecModelList_t.at(m_iModelIndex)) * 2;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));

		m_vecSaveInsObject.push_back(SaveDesc);
		m_vecFreeMatrix.push_back(SaveDesc.pMatTransform);

		m_iInsObjectCnt = 0;

		++m_iPushBackInsObject;
	}
}

void CObject_Window::Select_Model()
{
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return;
	}

	// 현재 선택된 모델이 무엇인지 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Current Model");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), m_strCurrentModel.c_str());

	// 모델 검색
	if (ImGui::InputTextWithHint("Find Model", "enter ModelName", m_szFindModel, IM_ARRAYSIZE(m_szFindModel)))
	{
		m_strFindModel = m_szFindModel;
	}

	ImGui::SameLine();

	if (ImGui::Button("Find"))
	{
		_uint i = 0;

		for (auto& iter : m_vecModelList)
		{
			if (string::npos != iter.find(m_strFindModel))
			{
				m_iModelIndex = i;
				break;
			}

			++i;
		}
	}

	// 모델 선택
	ImGui::ListBox("ModelList", &m_iModelIndex, VectorGetter,
		static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);

	// 선택된 모델을 적용
	if (ImGui::Button("SelectModel"))
	{
		if (FAILED(m_pDummy->Change_Model_Component(m_vecModelList_t.at(m_iModelIndex))))
		{
			MSG_BOX("Failed to Change Model");
			return;
		}

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

	// 메쉬 피킹 메뉴 On / Off
	ImGui::Checkbox("Object Picking", &m_isPickingObject);
	if (true == m_isPickingObject)
	{
		Mesh_Picking_Menu();
	}
}

void CObject_Window::Save_Load_Menu()
{
	// open Dialog Simple
	// Load 경로 지정
	if (ImGui::Button("Open Load File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseData", "Choose File", ".ddd",
			"../../Resources/GameData/MapData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseData"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// 주소 가공
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring ws;
			ws.assign(strFilePathName.begin(), strFilePathName.end());
			lstrcpy(m_wszMapLoadDataPath, ws.c_str());
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	_char szLoadPath[MAX_PATH] = "";
	WCharToChar(m_wszMapLoadDataPath, szLoadPath);

	// 현재 로드 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), szLoadPath);

	// 로드 버튼 처리
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_MapObject(m_wszMapLoadDataPath)))
			MSG_BOX("Failed to Load MapObject on Menu");
	}

	ImGui::Separator();

	if (ImGui::InputTextWithHint("SaveData Path", "enter SaveDataName.ddd", m_szPath, IM_ARRAYSIZE(m_szPath)))
	{
		m_strPath = m_szPath;
	}

	string s = ("../../Resources/GameData/MapData/");
	s += m_strPath;
	s += (".ddd");

	// 현재 세이브 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), s.c_str());

	// 세이브 버튼 처리
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_MapObject(m_strPath)))
			MSG_BOX("Failed to Save MapObject on Menu");
	}
}

void CObject_Window::Ins_Save_Load_Menu()
{
	// open Dialog Simple
	// Load 경로 지정
	if (ImGui::Button("Open Load File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseData", "Choose File", ".ddd",
			"../../Resources/GameData/MapData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseData"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// 주소 가공
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();

			wstring ws;
			ws.assign(strFilePathName.begin(), strFilePathName.end());
			lstrcpy(m_wszMapInsLoadDataPath, ws.c_str());
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	_char szLoadPath[MAX_PATH] = "";
	WCharToChar(m_wszMapInsLoadDataPath, szLoadPath);

	// 현재 로드 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), szLoadPath);

	// 로드 버튼 처리
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_MapObject_Ins(m_wszMapInsLoadDataPath)))
			MSG_BOX("Failed to Load MapObject_Ins on Menu");
	}

	ImGui::Separator();

	if (ImGui::InputTextWithHint("SaveData Path", "enter SaveDataName_Ins.ddd", m_szPath_Ins, IM_ARRAYSIZE(m_szPath_Ins)))
	{
		m_strPath_Ins = m_szPath_Ins;
	}

	string s = ("../../Resources/GameData/MapData/");
	s += m_strPath_Ins;
	s += (".ddd");

	// 현재 세이브 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), s.c_str());

	// 세이브 버튼 처리
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_MapObject_Ins(m_strPath_Ins)))
			MSG_BOX("Failed to Save MapObject_Ins on Menu");
	}
}

void CObject_Window::Delete_Object_Menu()
{
	// 마지막에 설치한 오브젝트를 제거
	// 단, 오브젝트가 하나라도 설치가 되어있어야 한다.
	if (ImGui::Button("Undo"))
	{
		if (0 != m_vecObjectTag_s.size())
		{
			if (FAILED(m_pGameInstance->Delete_Component(LEVEL_TOOL,
				TEXT("Layer_MapObject"), m_vecMapObjectTag.back().c_str())))
			{
				MSG_BOX("Failed to delete last MapObject");
			}

			if (0 < m_iTagIndex)
			{
				--m_iTagIndex;
			}

			if (0 < m_iMapObjectIndex)
			{
				--m_iMapObjectIndex;
			}

			m_vecMapObjectTag.pop_back();
			m_vecObjectTag_s.pop_back();
			m_vecSaveObject.pop_back();
		}
	}

	ImGui::SameLine();

	// 설치된 전체 오브젝트 제거 기능 활성화
	if (ImGui::Button("Delete All"))
	{
		m_isDeleteObject = !m_isDeleteObject;
	}

	// 위에 버튼을 눌러 경고창을 활성화한다.
	// 단, 오브젝트가 하나라도 설치가 되어있어야 한다.
	if (true == m_isDeleteObject && 0 != m_vecObjectTag_s.size())
	{
		ImGui::Text("Are you sure?");

		ImGui::SameLine();

		// yes를 누르면 MapObject 전부 삭제
		if (ImGui::Button("Yes"))
		{
			if (FAILED(m_pGameInstance->Clear_Layer(LEVEL_TOOL, TEXT("Layer_MapObject"))))
			{
				MSG_BOX("Failed to clear MapObject");
			}

			// 값 초기화
			m_iMapObjectIndex = 0;

			m_vecSaveObject.clear();
			m_vecObjectTag_s.clear();
			m_vecMapObjectTag.clear();

			// 인스턴싱 관련 초기화
			m_iPushBackInsObject = 0;
			m_iInsObjectCnt = 0;

			for (auto& iter : m_vecFreeMatrix)
			{
				Safe_Delete_Array(iter);
			}

			m_vecFreeMatrix.clear();
			m_vecSaveInsObject.clear();

			m_isDeleteObject = false;
		}

		ImGui::SameLine();

		// no를 누르면 아무일도 일어나지 않음
		if (ImGui::Button("No"))
		{
			m_isDeleteObject = false;
		}
	}
}

void CObject_Window::Mesh_Picking_Menu()
{
#ifdef _DEBUG
	if (true == m_pGameInstance->Get_DIKeyState(DIK_O, CInput_Device::KEY_DOWN))
	{
		//Target_Picking의 ID3D11Texture2D를 가져옴
		ID3D11Texture2D* pTexture = m_pGameInstance->Find_RenderTarget(TEXT("Target_Picking"))->Get_Texture2D();

		// 텍스처의 너비와 높이
		D3D11_TEXTURE2D_DESC  TextureDesc;
		pTexture->GetDesc(&TextureDesc);

		_uint iWidth = TextureDesc.Width;
		_uint iHeight = TextureDesc.Height;

		// 마우스 위치
		D3D11_VIEWPORT ViewPort;
		_uint iNumViewPorts = 1;

		ZEROMEM(&ViewPort);
		if (nullptr == m_pContext)
			return;
		m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);

		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		D3D11_BOX findDesc;
		ZEROMEM(&findDesc);

		// 이 구조체의 정보를 토대로 단 하나의 픽셀을 가져온다
		findDesc.left = pt.x;
		findDesc.right = pt.x + 1;
		findDesc.top = pt.y;
		findDesc.bottom = pt.y + 1;
		findDesc.front = 0;
		findDesc.back = 1;

		// Usage 버퍼 생성, 읽기 전용이다.
		ID3D11Texture2D* pCopyTexture2D = { nullptr };
		D3D11_TEXTURE2D_DESC	TextureDescCopy;
		ZEROMEMSIZE(&TextureDescCopy, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDescCopy.Width = 1;
		TextureDescCopy.Height = 1;
		TextureDescCopy.MipLevels = 1;
		TextureDescCopy.ArraySize = 1;
		TextureDescCopy.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

		TextureDescCopy.SampleDesc.Quality = 0;
		TextureDescCopy.SampleDesc.Count = 1;

		TextureDescCopy.Usage = D3D11_USAGE_STAGING;
		TextureDescCopy.BindFlags = 0;
		TextureDescCopy.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		TextureDescCopy.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDescCopy, nullptr, &pCopyTexture2D)))
			return;

		// 새로 만든 1칸짜리 텍스처에 가져온 렌더 타겟 텍스처에서 원하는 1칸을 넣어준다.
		m_pContext->CopySubresourceRegion(pCopyTexture2D, 0, 0, 0, 0, pTexture, 0, &findDesc);

		D3D11_MAPPED_SUBRESOURCE MappedDesc;
		ZEROMEM(&MappedDesc);

		if (FAILED(m_pContext->Map(pCopyTexture2D, 0, D3D11_MAP_READ, 0, &MappedDesc)))
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Failed to Map Picking Texture");
			return;
		}

		// 해당 Pixel Copy가 잘못 되었을 경우..
		if (MappedDesc.pData == nullptr)
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Copy Data is nullptr");
			return;
		}

		_uint pickID = { 0 };
		pickID = ((_uint*)MappedDesc.pData)[0];

		pickID -= 4278190081;

		string s = ("GameObject_MapObject_");
		s += std::to_string(pickID);

		// 대상 인덱스를 찾는 과정
		m_iTagIndex = 0;
		for (auto& iter : m_vecObjectTag_s)
		{
			if (iter == s)
				break;

			if (m_iTagIndex >= m_vecObjectTag_s.size() - 1)
				break;

			++m_iTagIndex;
		}

		wstring ws = TEXT("GameObject_MapObject_");
		ws += std::to_wstring(m_iTagIndex);

		CGameObject* pObject = static_cast<CGameObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), ws.c_str()));

		if (nullptr == pObject)
		{
			m_vObjectPickingPos = { -1.f, -1.f, -1.f };
		}
		else
		{
			m_vObjectPickingPos = pObject->Get_Transform()->Get_Position();
		}

		m_pContext->Unmap(pCopyTexture2D, 0);

		Safe_Release(pCopyTexture2D);
	}

	ImGui::Text("x : %f, y : %f, z : %f", m_vObjectPickingPos.x, m_vObjectPickingPos.y, m_vObjectPickingPos.z);

	// 메쉬 피킹한 오브젝트 상태 행렬 변경 메뉴
	ImGui::Checkbox("Change MapObject's Matrix", &m_isChangeObject);
	if (true == m_isChangeObject)
	{
		wstring ws = TEXT("GameObject_MapObject_");
		ws += std::to_wstring(m_iTagIndex);

		Change_Picking_Menu(ws.c_str(), m_iTagIndex);
	}

	// 메쉬 피킹한 오브젝트 삭제 메뉴
	if (ImGui::Button("Delete Choice MapObject"))
	{
		Delete_Picking_Object();
	}

	// 메쉬 피킹한 인스턴스 오브젝트 삭제 메뉴
	if (ImGui::Button("Delete Choice MapObject_Ins"))
	{
		Delete_Picking_Object_Ins();
	}

#endif
}

void CObject_Window::Change_Picking_Menu(const _tchar* wszTag, _uint iTagNum)
{
	m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
		TEXT("Layer_MapObject"), wszTag));

	if (nullptr == m_pObject)
	{
		MSG_BOX("Failed to Find Picking Object");
		m_isChangeObject = false;
		return;
	}

	if (m_pChangeMapObject != m_pObject)
	{
		ZeroMemory(&m_vChangeMapObject, sizeof m_vChangeMapObject);

		// 스케일 값 1로 초기화
		for (size_t i = 0; i < 3; i++)
			m_vChangeMapObject[DUMMY_SCALE][i] = 1.f;

		m_vChangeMapScaleOffset = m_pObject->Get_Transform()->Get_Scale();
		m_vChangeMapRotOffset = m_pObject->Get_Transform()->Get_Quaternion();
		m_vChangeMapTransOffset = m_pObject->Get_Transform()->Get_Position();

		m_pChangeMapObject = m_pObject;
	}

	// imGui에서 값을 조정해서 변환 행렬을 만들어줌.
	ImGui::DragFloat3("Scale", m_vChangeMapObject[DUMMY_SCALE], 0.1f, 0.1f, 10.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.1f ~ 10.f");

	ImGui::DragFloat3("Rotation", m_vChangeMapObject[DUMMY_ROT], 5.f, 0.f, 360.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.f ~ 360.f");

	ImGui::DragFloat3("Translation", m_vChangeMapObject[DUMMY_TRANS], 1.f, -50.f, 50.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-50.f ~ 50.f");

	// 상태 행렬 초기화
	if (ImGui::Button("reset"))
	{
		ZeroMemory(&m_vChangeMapObject, sizeof m_vChangeMapObject);

		// 스케일 값 1로 초기화
		for (size_t i = 0; i < 3; i++)
			m_vChangeMapObject[DUMMY_SCALE][i] = 1.f;
	}

	ImGui::Text("");

	// 조정한 상태값을 적용시킴
	if (nullptr != m_pObject)
	{
		_float3 vScale;
		vScale.x = m_vChangeMapScaleOffset.x * m_vChangeMapObject[DUMMY_SCALE][0];
		vScale.y = m_vChangeMapScaleOffset.y * m_vChangeMapObject[DUMMY_SCALE][1];
		vScale.z = m_vChangeMapScaleOffset.z * m_vChangeMapObject[DUMMY_SCALE][2];

		_float3 vRotation;
		vRotation.x = m_vChangeMapRotOffset.x + XMConvertToRadians(m_vChangeMapObject[DUMMY_ROT][0]);
		vRotation.y = m_vChangeMapRotOffset.y + XMConvertToRadians(m_vChangeMapObject[DUMMY_ROT][1]);
		vRotation.z = m_vChangeMapRotOffset.z + XMConvertToRadians(m_vChangeMapObject[DUMMY_ROT][2]);

		_float3 vTranslation =
		{ m_vChangeMapTransOffset.x + m_vChangeMapObject[DUMMY_TRANS][0],
			m_vChangeMapTransOffset.y + m_vChangeMapObject[DUMMY_TRANS][1],
			m_vChangeMapTransOffset.z + m_vChangeMapObject[DUMMY_TRANS][2] };

		m_pObject->Get_Transform()->Set_Scale_No_Zero(vScale);
		m_pObject->Get_Transform()->Set_Quaternion(m_pObject->Get_Transform()->Get_QuaternionVector_RollPitchYaw(vRotation));
		m_pObject->Get_Transform()->Set_Position(vTranslation);

		m_vecSaveObject.at(m_iTagIndex).matTransform = m_pObject->Get_Transform()->Get_WorldMatrix();
	}
}

void CObject_Window::Delete_Picking_Object()
{
	if (0 < m_vecObjectTag_s.size())
	{
		if (FAILED(m_pGameInstance->Delete_Component(LEVEL_TOOL,
			TEXT("Layer_MapObject"), m_vecMapObjectTag.at(m_iTagIndex).c_str())))
		{
			MSG_BOX("Failed to delete Picking MapObject");
			return;
		}

		if (0 < m_iMapObjectIndex)
		{
			--m_iMapObjectIndex;
		}

		m_vecMapObjectTag.erase(m_vecMapObjectTag.begin() + m_iTagIndex);
		m_vecObjectTag_s.erase(m_vecObjectTag_s.begin() + m_iTagIndex);
		m_vecSaveObject.erase(m_vecSaveObject.begin() + m_iTagIndex);
	}
}

void CObject_Window::Delete_Picking_Object_Ins()
{
	/*if (0 < m_vecSaveInsObject.size())
	{
		if (FAILED(m_pGameInstance->Delete_Component(LEVEL_TOOL,
			TEXT("Layer_MapObject"), m_vecMapObjectTag.at(iNum).c_str())))
		{
			MSG_BOX("Failed to delete Picking MapObject_Ins");
			return;
		}

		if (0 < m_iMapObjectIndex)
		{
			--m_iMapObjectIndex;
		}

		m_vecMapObjectTag.erase(m_vecMapObjectTag.begin() + iNum);
		m_vecInsObjectTag.erase(m_vecInsObjectTag.begin() + iNum);
		m_vecSaveInsObject.erase(m_vecSaveInsObject.begin() + iNum);
	}*/
}

void CObject_Window::Map_Brushing_Menu()
{
	_float3 vPos = Find_PickingPos();

	CTerrain* pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
		TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));

	ImGui::DragFloat("Brush Size", &m_fBrushSize, 0.1f, 0.1f, 100.f);

	// 지형 커서의 움직임과 크기 설정
	pTerrain->Set_CurrentBrushingPoint(vPos);
	pTerrain->Set_CurrentBrushingSize(m_fBrushSize);

	// 지형 텍스처와 브러쉬 텍스처 선택
	const char* GroundTexture[] = { "FarGrass", "Grass_Spring", "DeadGrass", "DeadGrassLeaves" };
	const char* BrushTexture[] = { "FarGrass", "Grass_Spring", "DeadGrass", "DeadGrassLeaves" };

	ImGui::Combo("GroundTexture", &m_iDiffuseIndex, GroundTexture, IM_ARRAYSIZE(GroundTexture));
	pTerrain->Set_DiffuseTextureIndex((_uint)m_iDiffuseIndex);

	ImGui::Combo("BrushTexture", &m_iBrushIndex, BrushTexture, IM_ARRAYSIZE(BrushTexture));
	pTerrain->Set_BrushTextureIndex((_uint)m_iBrushIndex);

	m_fTimeAccBrush += m_pGameInstance->Get_World_Tick();

	// 마우스를 꾹 누르고 있으면 연속 설치
	if (true == m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x &&
		0.2f < m_fTimeAccBrush)
	{
		pTerrain->Set_BrushDesc(vPos, m_fBrushSize, m_iBrushIndex);
		m_fTimeAccBrush = 0.f;
	}

	// 브러쉬 Undo
	if (ImGui::Button("Undo"))
	{
		pTerrain->Set_BrushUndo();
	}

	ImGui::SameLine();

	// 브러쉬 전체 삭제
	if (ImGui::Button("Delete All"))
	{
		m_isDeleteBrush = !m_isDeleteBrush;
	}

	if (true == m_isDeleteBrush)
	{
		pTerrain->Set_BrushReset();
		m_isDeleteBrush = false;
	}

	// 현재 지형을 텍스처로 뽑아냄
	if (ImGui::Button("Save"))
	{
		CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
			static_cast<CTerrain*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
				TEXT("GameObject_Terrain")))->Get_Buffer());

		//Target_Brushing의 ID3D11Texture2D를 가져옴
		ID3D11Texture2D* pTexture = m_pGameInstance->Find_RenderTarget(TEXT("Target_MapBrushing"))->Get_Texture2D();

		if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture, TEXT("../../Resources/Texture/Terrain/Create/Filter.dds"))))
		{
			MSG_BOX("Failed to Save Terrain Texture");
			return;
		}

		MSG_BOX("Save Terrain Texture");
	}
}

void CObject_Window::All_Map_Object_Translation()
{
	ImGui::DragFloat3("All MapObject Translation", m_vAllMapTrans, 1.f, -500.f, 500.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-500.f ~ 500.f");

	if (m_vAllMapTrans[0] != m_vAllMapPre[0] ||
		m_vAllMapTrans[1] != m_vAllMapPre[1] ||
		m_vAllMapTrans[2] != m_vAllMapPre[2])
	{
		for (auto& iter : m_vecMapObjectTag)
		{
			CGameObject* pMapObject = static_cast<CGameObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
				TEXT("Layer_MapObject"), iter.c_str()));

			_float3 vPos = pMapObject->Get_Transform()->Get_Position();

			_float3 vTranslation =
			{ vPos.x + (m_vAllMapTrans[0] - m_vAllMapPre[0]),
				vPos.y + (m_vAllMapTrans[1] - m_vAllMapPre[1]),
				vPos.z + (m_vAllMapTrans[2] - m_vAllMapPre[2]) };

			pMapObject->Get_Transform()->Set_Position(vTranslation);
		}

		for (auto& iter : m_vecSaveObject)
		{
			iter.matTransform._41 += (m_vAllMapTrans[0] - m_vAllMapPre[0]);
			iter.matTransform._42 += (m_vAllMapTrans[1] - m_vAllMapPre[1]);
			iter.matTransform._43 += (m_vAllMapTrans[2] - m_vAllMapPre[2]);
		}

		for (auto& iter : m_vecSaveInsObject)
		{
			iter.matTransform._41 += (m_vAllMapTrans[0] - m_vAllMapPre[0]);
			iter.matTransform._42 += (m_vAllMapTrans[1] - m_vAllMapPre[1]);
			iter.matTransform._43 += (m_vAllMapTrans[2] - m_vAllMapPre[2]);
		}

		for (size_t i = 0; i < 3; i++)
		{
			m_vAllMapPre[i] = m_vAllMapTrans[i];
		}
	}
}

HRESULT CObject_Window::Save_MapObject(string szMapDataPath)
{
	_tchar wszPath[MAX_PATH] = TEXT("../../Resources/GameData/MapData/");

	lstrcpy(m_wszMapSaveDataPath, TEXT(""));
	lstrcat(m_wszMapSaveDataPath, wszPath);
	wstring ws(szMapDataPath.begin(), szMapDataPath.end());
	lstrcat(m_wszMapSaveDataPath, ws.c_str());
	lstrcat(m_wszMapSaveDataPath, TEXT(".ddd"));

	HANDLE hFile = CreateFile(m_wszMapSaveDataPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Save MapObject");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	for (auto& iter : m_vecSaveObject)
	{
		if (!WriteFile(hFile, &iter.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.vPos");
			return E_FAIL;
		}

		if (!WriteFile(hFile, &iter.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.iTagLen");
			return E_FAIL;
		}

		if (!WriteFile(hFile, &iter.wszTag, iter.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.wszTag");
			return E_FAIL;
		}
	}

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CObject_Window::Load_MapObject(const _tchar* wszMapDataPath)
{
	HANDLE hFile = CreateFile(wszMapDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	while (true)
	{
		SAVEOBJECTDESC SaveDesc;
		ZEROMEM(&SaveDesc);

		if (!ReadFile(hFile, &SaveDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos");
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &SaveDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen");
			CloseHandle(hFile);
		}

		if (!ReadFile(hFile, &SaveDesc.wszTag, SaveDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		m_vecSaveObject.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	_uint iPreCnt = m_iMapObjectIndex;

	// 로드한 데이터를 적용시켜 주는 부분
	for (size_t i = iPreCnt; i < m_vecSaveObject.size(); i++)
	{
		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iModelCnt));
		Deep_Copy_Tag(wszobjName);
		++m_iModelCnt;

		// 번호를 붙인 태그로 MapObject 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.back().c_str(), &m_vecSaveObject[i].matTransform)))
		{
			MSG_BOX("Failed to Install MapObject");
			return E_FAIL;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObject = static_cast<CMapObject*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecSaveObject[i].wszTag);
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌

		++m_iMapObjectIndex;
	}

	return S_OK;
}

HRESULT CObject_Window::Save_MapObject_Ins(string szMapDataPath)
{
	_tchar wszPath[MAX_PATH] = TEXT("../../Resources/GameData/MapData/");

	lstrcpy(m_wszMapInsSaveDataPath, TEXT(""));
	lstrcat(m_wszMapInsSaveDataPath, wszPath);
	wstring ws(szMapDataPath.begin(), szMapDataPath.end());
	lstrcat(m_wszMapInsSaveDataPath, ws.c_str());
	lstrcat(m_wszMapInsSaveDataPath, TEXT(".ddd"));

	HANDLE hFile = CreateFile(m_wszMapInsSaveDataPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Save MapData_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	for (_uint i = 0; i < m_vecSaveInsObject.size(); ++i)
	{
		if (!WriteFile(hFile, &m_vecSaveInsObject.at(i).iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write iInstanceCnt");
			return E_FAIL;
		}

		// 여기서 따로 벡터에 저장해둔 값들을 저장한다.
		for (size_t j = 0; j < m_vecSaveInsObject.at(i).iInstanceCnt; ++j)
		{
			if (!WriteFile(hFile, &m_vecSaveInsObject.at(i).pMatTransform[j], sizeof(_float4x4), &dwByte, nullptr))
			{
				MSG_BOX("Failed to Write pMatTransform");
				return E_FAIL;
			}
		}

		if (!WriteFile(hFile, &m_vecSaveInsObject.at(i).matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write matTransform");
			return E_FAIL;
		}

		if (!WriteFile(hFile, &m_vecSaveInsObject.at(i).iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write iTagLen");
			return E_FAIL;
		}

		// 현재 선택된 모델 문자열
		if (!WriteFile(hFile, &m_vecSaveInsObject.at(i).wszTag, m_vecSaveInsObject.at(i).iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecModelList_t.at(m_iModelIndex)");
			return E_FAIL;
		}
	}

	m_iInsObjectCnt = 0;

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CObject_Window::Load_MapObject_Ins(const _tchar* wszMapDataPath)
{
	HANDLE hFile = CreateFile(wszMapDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	while (true)
	{
		SAVEINSOBJECTDESC SaveDesc;
		ZEROMEM(&SaveDesc);

		if (!ReadFile(hFile, &SaveDesc.iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.iInstanceCnt");
			return E_FAIL;
		}

		// 저장되어있던 인스턴스 개수만큼 동적할당
		if (0 != SaveDesc.iInstanceCnt)
		{
			SaveDesc.pMatTransform = New _float4x4[SaveDesc.iInstanceCnt];

			for (size_t i = 0; i < SaveDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &SaveDesc.pMatTransform[i], sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read m_vecSaveInsObject.pMatTransform");
					return E_FAIL;
				}
			}

			m_vecFreeMatrix.push_back(SaveDesc.pMatTransform);
		}

		if (!ReadFile(hFile, &SaveDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.matTransform");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &SaveDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.iTagLen");
		}

		if (!ReadFile(hFile, &SaveDesc.wszTag, SaveDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		m_vecSaveInsObject.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	// 모델 중복 체크 비교 변수
	_uint j = 0;

	// 로드한 데이터를 적용시켜 주는 부분
	for (size_t i = 0; i < m_vecSaveInsObject.size(); i++)
	{
		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		// 여기서 프로토타입 태그 문자열 가공해줘야함
		wstring ws = TEXT("Prototype_Component_Model_Instance_");
		wstring wsTag = TEXT("Prototype_Component_Model_");
		wstring wsSave(m_vecSaveInsObject.at(i).wszTag);
		_uint iLength = wsTag.size();

		// 모델 이름
		wstring wsModelName = wsSave.substr(iLength);
		ws += wsModelName;

		ws += TEXT("_");

		_tchar wszNumber[MAX_PATH];
		_itow_s(j, wszNumber, 10);

		ws += wszNumber; // 여기까지 오면 Prototype_Component_Model_Instance_모델명_번호 이런식으로 이름이 붙음	

		m_vecInsObjectTag.push_back(ws);
		++j;

		wstring wsPath = TEXT("../../Resources/Models/MapObject/NonAnims/");
		wsPath += wsModelName;
		wsPath += TEXT("/");
		wsPath += wsModelName;
		wsPath += TEXT(".dat");

		// 인스턴스 모델 프로토타입 생성
		_float4x4 PivotMatrix = XMMatrixIdentity();
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, ws.c_str(),
			CModel_Instance::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, wsPath.c_str(),
				m_vecSaveInsObject.at(i).pMatTransform, m_vecSaveInsObject.at(i).iInstanceCnt, PivotMatrix), true)))
		{
			MSG_BOX("Failed to Create New CModel_Instance Prototype");
		}

		// 번호를 붙인 태그로 MapObject_Ins 등록
		if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject_Ins"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &m_vecSaveInsObject[i].matTransform)))
		{
			MSG_BOX("Failed to Install MapObject");
			return E_FAIL;
		}

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		m_pObjIns = static_cast<CMapObject_Ins*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		if (nullptr != m_pObjIns)
		{
			m_pObjIns->Add_Model_Component(ws.c_str());
			m_pObjIns->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMeshInstance"));
			m_pObjIns->Set_Color(m_iModelCnt); // 고유한 색깔 값을 넣어줌
			++m_iModelCnt;
		}

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

	char c[MAX_PATH] = "";
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
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return _float3(-1.f, -1.f, -1.f);
	}

	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	// U키를 누르면 마우스 움직임 봉쇄
	if (true == m_pGameInstance->Get_DIKeyState(DIK_U, CInput_Device::KEY_DOWN))
	{
		m_isLockMouseMove = !m_isLockMouseMove;
	}

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

HRESULT CObject_Window::Create_Dummy()
{
	// 이미 더미가 있다면 리턴
	if (nullptr != m_pDummy)
	{
		MSG_BOX("Dummy Already exists");
		return S_OK;
	}

	_float3 vPos = { 5.f, 0.f, 5.f };

	if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, LEVEL_TOOL, TEXT("Prototype_GameObject_MapDummy"),
		TEXT("Layer_Tool"), TEXT("Map_Dummy"), &vPos)))
	{
		MSG_BOX("Failed to GameObject Map_Dummy");
		return E_FAIL;
	}

	Safe_AddRef(m_pDummy);

	m_pDummy = static_cast<CMapDummy*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Map_Dummy")));
	m_pDummy->Add_Model_Component(TEXT("Prototype_Component_Model_Tree"));
	m_pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));

	return S_OK;
}

HRESULT CObject_Window::Delete_Dummy()
{
	// 이미 더미가 없다면 리턴
	if (nullptr == m_pDummy)
	{
		MSG_BOX("Dummy Already deleted");
		return S_OK;
	}

	if (FAILED(m_pGameInstance->Delete_Component(LEVEL_TOOL,
		TEXT("Layer_Tool"), TEXT("Map_Dummy"))))
	{
		MSG_BOX("Failed to delete MapDummy");
		return E_FAIL;
	}

	m_pDummy = nullptr;

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
			// dat파일 체크
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".dat")))
			{
				if (false == fs::exists(entry.path()))
				{
					MSG_BOX("Failed to find dat file");
					return E_FAIL;
				}

				wstring pathresult = entry.path().wstring();

				Deep_Copy_Path(pathresult.c_str());

				// 경로에서 모델 이름 부분만 잘라내는 부분
				string s = entry.path().string();

				size_t path_length = pathresult.length();
				size_t current = s.find("MapObject\\NonAnims") + 19;

				// 1차 분리, 여기서 모델 이름 파일 경로가 나와야 함.
				string result = s.substr(current, path_length);

				// 파일명에 Lod가 들어가 있는지 찾는다.
				if (std::string::npos != result.find(("_Lod")))
				{
					// 만약 있다면 프로토 타입을 만들지 않는다.
					iter++;
					continue;
				}

				size_t current1 = result.find("\\");

				// 2차 분리, 여기서 모델 이름이 나와야 함
				string result1 = result.substr(0, current1);

				// 모델 리스트에 출력할 문자열 모음에 넣음
				m_vecModelList.push_back(result1);

				// 이제 컴포넌트 모델 이름으로 결합
				string modelname = ("Prototype_Component_Model_");
				modelname += result1;

				wstring wmodelname(modelname.begin(), modelname.end());
				Deep_Copy_Name(wmodelname.c_str());

				// 프로토타입 생성
				_float4x4 PivotMatrix = XMMatrixIdentity();

				// 파일명에 Anim_가 들어가 있는지 찾는다.
				if (std::string::npos != result.find(("Anim_")))
				{
					// 만약 있다면 다른 피벗 메트릭스를 부여한다.
					PivotMatrix = XMMatrixRotationX(XMConvertToRadians(90.f));
				}

				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, m_vecModelList_t.back(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, m_vecModelPath_t.back(), PivotMatrix), true)))
				{
					MSG_BOX("Failed to Create New Model Prototype");
				}
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

	for (auto& iter : m_vecFreeMatrix)
	{
		Safe_Delete_Array(iter);
	}

	m_vecFreeMatrix.clear();
	m_vecSaveInsObject.clear();
}
