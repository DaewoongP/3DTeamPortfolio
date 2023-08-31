#include "..\Public\Object_Window.h"
#include "ImGuiFileDialog.h"

#include "VIBuffer_Terrain.h"

#include "MapDummy.h"
#include "Terrain.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	if (FAILED(Create_Dummy()))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	// 가장 처음 요소 하나(더미) 넣어줌
	Deep_Copy_Name();
	m_vecModelList.push_back(m_strCurrentModel);
	m_vecModelPath_t.push_back(TEXT("C:/Users/micro/3DTeamPortfolio/Resources/Models/NonAnims/Tree/Tree.dat"));

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	// 메뉴 On Off static _bool 변수 모음
	static _bool bCheckPicking = { false };
	static _bool bSelectModel = { false };

	// Picking 창 On / Off
	ImGui::Checkbox("Picking", &bCheckPicking);
	if(true == bCheckPicking)
	{
		PickingMenu();
	}

	// Model 선택 창 On / Off
	ImGui::Checkbox("Model Select", &bSelectModel);
	if (true == bSelectModel)
	{
		SelectModel();
	}	

	ImGui::End();
}

HRESULT CObject_Window::Render()
{
	return S_OK;
}

void CObject_Window::PickingMenu()
{
	_float3 vPos = Find_PickingPos();

	// 현재 피킹 위치 표시
	ImGui::Text("Picking Position");
	ImGui::Text("%.1f /", vPos.x);
	ImGui::SameLine();
	ImGui::Text("%.1f /", vPos.y);
	ImGui::SameLine();
	ImGui::Text("%.1f", vPos.z);
	ImGui::Text("----------------------------------------");

	// 마우스 좌클릭을 하면 해당 위치로 더미 이동
	BEGININSTANCE; if (true == pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		-1.f != vPos.x)
	{
		m_pDummy->Set_Pos(vPos);
	} ENDINSTANCE;
}

void CObject_Window::SelectModel()
{
	// 현재 선택된 모델이 무엇인지 표시
	ImGui::Text("Current Model");
	ImGui::Text(":");
	ImGui::SameLine();
	ImGui::Text(m_strCurrentModel.c_str());

	// 모델 선택
	ImGui::ListBox("ModelList", &m_iModelIndex, VectorGetter, static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);

	// open Dialog Simple
	// 모델을 골라 프로토타입 만들어주는 부분
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose File", ".dat", "../../Resources/Models/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			_char fileName[MAX_PATH] = "";
			_splitpath_s(strFilePathName.c_str(), nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

			_tchar wszfilePath[MAX_PATH] = {};
			_tchar wszfileName[MAX_PATH] = {};
			CharToWChar(strFilePathName.c_str(), wszfilePath);
			CharToWChar(fileName, wszfileName);
			_tchar wszModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_");
			lstrcat(wszModelTag, wszfileName);

			wstring ws(wszModelTag);
			m_strCurrentModel.assign(ws.begin(), ws.end());
			m_vecModelList.push_back(m_strCurrentModel);
			Deep_Copy_Name();
			Deep_Copy_Path(wszfilePath);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// 선택된 모델을 적용
	static _bool bCheckModel = { false };
	if (ImGui::Button("SelectModel"))
	{
		// 기존에 깊은 복사로 저장해뒀던 문자열들을 가지고 프로토 생성
		_float4x4 PivotMatrix = XMMatrixIdentity();
		BEGININSTANCE; if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, m_vecModelList_t.at(m_iModelIndex),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, m_vecModelPath_t.at(m_iModelIndex), PivotMatrix))))
		{
			MSG_BOX("Failed to Create New Model Prototype");
		} ENDINSTANCE;

		if (FAILED(m_pDummy->Change_Model_Component(m_vecModelList_t.at(m_iModelIndex))))
		{
			MSG_BOX("Failed to Change Model");
			return;
		}
	}

	ImGui::Text("----------------------------------------");
}

void CObject_Window::Deep_Copy_Name()
{
	// 모델 이름을 const _tchar* 형태로 깊은 복사
	size_t length = m_strCurrentModel.size();

	wstring ws (m_strCurrentModel.begin(), m_strCurrentModel.end());

	_tchar* wszNew = new _tchar[length + 1];

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

	_tchar* wszNew = new _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Path)");
		return;
	}

	m_vecModelPath_t.push_back(wszNew);
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

	_float4x4 PivotMatrix = XMMatrixIdentity();

	if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Tree"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, 
			TEXT("../../Resources/Models/NonAnims/Tree/Tree.dat"), PivotMatrix))))
		return E_FAIL;

	m_pDummy->Add_Model_Component(TEXT("Prototype_Component_Model_Tree"));
	m_pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh")); ENDINSTANCE;

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
}
