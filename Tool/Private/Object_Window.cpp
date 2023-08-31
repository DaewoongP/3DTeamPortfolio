#include "..\Public\Object_Window.h"
#include "ImGuiFileDialog.h"

#include "VIBuffer_Terrain.h"

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

	if (FAILED(Create_Dummy()))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	// 가장 처음 요소 하나(더미) 넣어줌
	// 지금은 더미 모델을 Tree로 해뒀지만 
	// 나중에 적당한 모델 찾아서 대체해야 한다.
	m_vecModelList.push_back(m_strCurrentModel);
	Deep_Copy_Name();
	m_vecModelPath_t.push_back(TEXT("../../Resources/Models/NonAnims/Tree/Tree.dat"));

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	// 메뉴 On Off static _bool 변수 모음
	static _bool bCheckPicking = { false };
	static _bool bSelectModel = { false };
	static _bool bSaveLoad = { false };

	// Picking 창 On / Off
	ImGui::Checkbox("Picking", &bCheckPicking);
	if(true == bCheckPicking)
	{
		Picking_Menu();
	}

	ImGui::Text("----------------------------------------");

	// Model 선택 창 On / Off
	ImGui::Checkbox("Model Select", &bSelectModel);
	if (true == bSelectModel)
	{
		Select_Model();
	}

	ImGui::Text("----------------------------------------");

	// Save Load 선택 창 On / Off
	ImGui::Checkbox("Save Load", &bSaveLoad);
	if (true == bSaveLoad)
	{
		Save_Load_Menu();
	}

	ImGui::Text("----------------------------------------");

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

	// 메뉴 On Off static _bool 변수 모음
	static _bool bInstallObject = { false };

	// Object Install 선택 창 On / Off
	ImGui::Checkbox("Object Install", &bInstallObject);
	if (true == bInstallObject)
	{
		Install_Object(m_pDummy->Get_Transform()->Get_Translation());
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

		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"), 
			m_vecMapObjectTag_t.at(m_iMapObjectIndex), &vPos)))
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

		++m_iMapObjectIndex;

	} ENDINSTANCE;
}

void CObject_Window::Select_Model()
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
			Deep_Copy_Path(wszfilePath);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// 선택된 모델을 적용
	static _bool bCheckModel = { false };
	if (ImGui::Button("SelectModel"))
	{
		// 기존에 깊은 복사로 저장해뒀던 문자열들을 가지고 프로토타입 생성
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

HRESULT CObject_Window::Save_MapObject()
{


	return S_OK;
}

HRESULT CObject_Window::Load_MapObject()
{


	return S_OK;
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

void CObject_Window::Deep_Copy_Tag(const _tchar* wszTag)
{
	// 맵 오브젝트 경로를 const _tchar* 형태로 깊은 복사
	size_t length = wcslen(wszTag);

	wstring ws(wszTag);

	_tchar* wszNew = new _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Tag)");
		return;
	}

	m_vecMapObjectTag_t.push_back(wszNew);
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
		TEXT("Layer_MapObject"), TEXT("Map_Dummy"), &vPos)))
	{
		MSG_BOX("Failed to GameObject Map_Dummy");
		return E_FAIL;
	}

	m_pDummy = static_cast<CMapDummy*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_MapObject"), TEXT("Map_Dummy")));

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
	m_vecModelList_t.clear();

	for (size_t i = 1; i < m_vecModelPath_t.size(); ++i)
	{
		delete[] m_vecModelPath_t[i];
	}
	m_vecModelPath_t.clear();

	for (auto& iter : m_vecMapObjectTag_t)
	{
		delete[] iter;
	}
	m_vecMapObjectTag_t.clear();
}
