#include "..\Public\Monster_Window.h"
#include "ImGuiFileDialog.h"

#include "Dummy.h"
#include "Terrain.h"

#include "HelpMaker.h"

CMonster_Window::CMonster_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
	ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);

	// ������ �� 1�� �ʱ�ȭ
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

	// Create Dummy ��ư
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
		// Picking â On / Off
		ImGui::Checkbox("Picking", &m_isCheckPicking);
		if (true == m_isCheckPicking)
		{
			Picking_Menu();
		}

		ImGui::Separator();

		// ���� ��ġ�Ǿ� �ִ� �� ������Ʈ â On / Off
		ImGui::Checkbox("Current MapObject", &m_isCurrentMapObject);
		if (true == m_isCurrentMapObject)
		{
			ImGui::Begin("Current MapObject", nullptr);
			Show_MonsterTable();
			ImGui::End();
		}

		ImGui::Separator();

		// Save Load ���� â On / Off
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

	// ���� ��ŷ ��ġ ǥ��
	ImGui::Text("Picking Position");
	ImGui::Text("Pressing LShift : Rounding the value");
	ImGui::Text("%.1f /", vPos.x);
	ImGui::SameLine();
	ImGui::Text("%.1f /", vPos.y);
	ImGui::SameLine();
	ImGui::Text("%.1f", vPos.z);
	ImGui::Separator();

	ImGui::Text("");

	// imGui���� ���� �����ؼ� ��ȯ ����� �������.
	ImGui::DragFloat3("Scale", m_vDummyMatrix[DUMMY_SCALE], 0.1f, 0.1f, 10.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.1f ~ 10.f");

	ImGui::DragFloat3("Rotation", m_vDummyMatrix[DUMMY_ROT], 5.f, 0.f, 360.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("0.f ~ 360.f");

	ImGui::DragFloat3("Translation", m_vDummyMatrix[DUMMY_TRANS], 1.f, -50.f, 50.f);
	ImGui::SameLine(); CHelpMaker::HelpMarker("-50.f ~ 50.f");

	// ���� ��� �ʱ�ȭ
	if (ImGui::Button("reset"))
	{
		ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);

		// ������ �� 1�� �ʱ�ȭ
		for (size_t i = 0; i < 3; i++)
			m_vDummyMatrix[DUMMY_SCALE][i] = 1.f;
	}

	ImGui::Text("");

	// ������ ���°��� Dummy�� �����Ŵ
	if (nullptr != m_pCurrentDummy)
	{
		_float3 vScale;
		memcpy(&vScale, m_vDummyMatrix[DUMMY_SCALE], sizeof _float3);

		_float3 vRotation;
		memcpy(&vRotation, m_vDummyMatrix[DUMMY_ROT], sizeof _float3);

		vRotation.x = XMConvertToRadians(vRotation.x);
		vRotation.y = XMConvertToRadians(vRotation.y);
		vRotation.z = XMConvertToRadians(vRotation.z);

		// shiftŰ�� ������ ������ ���ڿ� �� �°� ��ġ�� �ݿø���
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
	// ��ġ�Ǿ� �ִ� ������Ʈ ����Ʈ
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

		/* Model Data Table ��� */
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
	// Load ��� ����
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
			// �ּ� ����
			strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			wstrFilePahtName = strToWStr(strFilePathName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// ���� �ε� ��� ǥ��
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Load Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), strFilePathName.c_str());

	// �ε� ��ư ó��
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

	// ���� ���̺� ��� ǥ��
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Save Path");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), strFilePath.c_str());

	// ���̺� ��ư ó��
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

	// shiftŰ�� ������ ������ ���ڿ� �� �°� ��ġ�� �ݿø���
	if (true == m_pGameInstance->Get_DIKeyState(DIK_U, CInput_Device::KEY_DOWN))
	{
		m_isLockMouseMove = !m_isLockMouseMove;
	}

	if (true == m_isLockMouseMove)
	{
		return m_vLockingMousePos;
	}

	// ���콺�� ȭ�� �ۿ� ������ �˻� ����
	if (false == m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		return _float3(-1.f, -1.f, -1.f);
	}

	m_pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer());

	float fDist = FLT_MAX; // ��ŷ ���� �� ���� �Ÿ���

	_bool bResult = pTerrain->IsPicked(vRayPos, vRayDir, fDist);

	// ����� ���� ��� RayDir�� �Ÿ����� ���� ���� ��ġ ����
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

	// ���� ��ŷ ��ġ ǥ��
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
	// �̹� ���̰� ���ٸ� ����
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
{// ���丮 ��θ� ��ȸ�� iterator
	fs::directory_iterator iter(fs::absolute(wstrFilePath));

	while (iter != fs::end(iter))
	{
		// ���� ���丮 ��θ� ����ִ� ���� (iterator�� ����)
		const fs::directory_entry& entry = *iter;

		// ���� entry ������ ���丮���� Ȯ�� �� ���丮�̸� ���
		if (fs::is_directory(entry.path()))
		{
			Load_Model_Prototypes(entry.path().c_str());
		}
		else
		{
			// dat���� üũ
			if (!lstrcmp(entry.path().extension().c_str(), TEXT(".dat")))
			{
				if (false == fs::exists(entry.path()))
				{
					MSG_BOX("Failed to find dat file");
					return E_FAIL;
				}

				wstring pathresult = entry.path().wstring();
				m_wstrModelPaths.push_back(pathresult);

				// ��ο��� �� �̸� �κи� �߶󳻴� �κ�
				size_t path_length = pathresult.length();
				size_t current = pathresult.find(TEXT("Models\\Anims")) + 13;

				// 1�� �и�, ���⼭ �� �̸� ���� ��ΰ� ���;� ��.
				wstring result = pathresult.substr(current, path_length);

				// ���ϸ� Lod�� �� �ִ��� ã�´�.
				if (std::wstring::npos != result.find((TEXT("_Lod"))))
				{
					// ���� �ִٸ� ������ Ÿ���� ������ �ʴ´�.
					iter++;
					continue;
				}

				size_t current1 = result.find(TEXT("\\"));

				// 2�� �и�, ���⼭ �� �̸��� ���;� ��
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

				// �� ����Ʈ�� ����� ���ڿ� ������ ����
				m_wstrModelTags.push_back(result1);

				// ���� ������Ʈ �� �̸����� ����
				wstring modelname = (TEXT("Prototype_Component_Model_"));
				modelname += result1;

				// ������Ÿ�� ����
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
