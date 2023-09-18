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

	ImGui::Text("");

	// imGui에서 값을 조정해서 변환 행렬을 만들어줌.
	ImGui::DragFloat3("Scale", m_vDummyMatrix[DUMMY_SCALE], 0.1f, 0.1f, 10.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.1f ~ 10.f");

	ImGui::DragFloat3("Rotation", m_vDummyMatrix[DUMMY_ROT], 5.f, 0.f, 360.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.f ~ 360.f");

	ImGui::DragFloat3("Translation", m_vDummyMatrix[DUMMY_TRANS], 1.f, -50.f, 50.f);
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

	if (ImGui::BeginTable("Table_Model_Monster", 3, flags, ImVec2(0, 600), 0.f))
	{
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, 1);
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
		ImGui::TableSetupColumn("ModelTag", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableHeadersRow();

		/* Model Data Table 출력 */
		_uint iIndex = 0;
		for (auto& pDummy : m_Dummies)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			if (ImGui::Selectable(std::to_string(iIndex).c_str(), m_iTableIndex == iIndex))
			{
				m_iTableIndex = iIndex;
				m_pCurrentDummy = pDummy;
			}

			ImGui::TableSetColumnIndex(1);
			ImGui::TextUnformatted(wstrToStr(pDummy->Get_Tag()).c_str());

			ImGui::TableSetColumnIndex(2);
			if (nullptr != pDummy->Get_Model())
				ImGui::TextUnformatted(wstrToStr(pDummy->Get_Model()->Get_Tag()).c_str());

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

	string strFilePathName = { "" };
	wstring wstrFilePahtName = { L"" };
	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseMonsterData"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// 주소 가공
			strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			wstrFilePahtName = strToWStr(strFilePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// 현재 로드 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), strFilePathName.c_str());

	// 로드 버튼 처리
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_Monsters(wstrFilePahtName)))
			MSG_BOX("Failed to Load MapObject on Menu");
	}

	ImGui::Separator();

	string strDataName = { "" };
	ImGui::InputTextWithHint("SaveData Path", "enter SaveDataName.mon", m_szDataName, IM_ARRAYSIZE(m_szDataName));

	string strFilePath = ("../../Resources/GameData/MonsterData/");
	strFilePath = strFilePath + m_szDataName + ".mon";

	// 현재 세이브 경로 표시
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), strFilePath.c_str());

	// 세이브 버튼 처리
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_Monsters(m_szDataName)))
			MSG_BOX("Failed to Save MapObject on Menu");
	}
}

HRESULT CMonster_Window::Save_Monsters(const string& szMapDataPath)
{
	return S_OK;
}

HRESULT CMonster_Window::Load_Monsters(const wstring& wstrMapDataPath)
{
	return S_OK;
}

_float3 CMonster_Window::Find_PickingPos()
{
	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	// shift키를 누르고 있으면 격자에 딱 맞게 위치가 반올림됨
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

HRESULT CMonster_Window::Create_Dummy()
{
	ImGui::Text("1. Select Prototype");
	ImGui::Text("2. Press 'H' Button");
	ImGui::Text("3. Pick Terrain");

	ImGui::Separator();

	ImGui::Text("Monster Prototype List");
	if(ImGui::BeginListBox("##Monster Prototype List"))
	{
		_int iIndex = { 0 };
		for (auto& Tag : m_wstrModelTags)
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
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".dat")))
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

				_bool isAdd = { true };
				if (true == isAdd && wstring::npos == result1.find(TEXT("Golem")))
				{
					m_wstrModelPaths.pop_back();
					iter++;
					continue;
				}
				else
					isAdd = false;

				if (true == isAdd && wstring::npos == result1.find(TEXT("Troll")))
				{
					m_wstrModelPaths.pop_back();
					iter++;
					continue;
				}
				else
					isAdd = false;

				// 모델 리스트에 출력할 문자열 모음에 넣음
				m_wstrModelTags.push_back(result1);

				// 이제 컴포넌트 모델 이름으로 결합
				wstring modelname = (TEXT("Prototype_Component_Model_"));
				modelname += result1;

				// 프로토타입 생성
				_float4x4 PivotMatrix;
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, m_wstrModelTags.back().c_str(),
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
}
