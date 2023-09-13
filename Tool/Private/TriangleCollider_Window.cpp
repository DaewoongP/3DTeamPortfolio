#include "..\Public\TriangleCollider_Window.h"
#include "ImGuiFileDialog.h"

#include "Terrain.h"
#include "Camera_Free.h"

CTriangleCollider_Window::CTriangleCollider_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
	, m_vPickPos()
{
}

HRESULT CTriangleCollider_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	m_pShaderCom = static_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Debug")));
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	BEGININSTANCE

		m_pTriangleColMesh =
		dynamic_cast<CTriangleColMesh*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_TriangleColMesh")));

	m_pTriangleColMesh->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
	ENDINSTANCE
		return S_OK;
}

void CTriangleCollider_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& CellDesc : m_Cells)
	{
		CellDesc.m_pBufferCom->Tick(fTimeDelta);
	}

	ImGui::Begin("TriangleCollider", nullptr, m_WindowFlag);

	OpenFile_Button();
	AddModel_Button();
	if (m_pTriangleColMesh != nullptr)
		m_pTriangleColMesh->Tick(fTimeDelta);
	EditVertices(fTimeDelta);
	Make_Cell();
	/* Check Option */
	ImGui::RadioButton("Nothing", &m_iSelectOption, OPTION_NOTHING); ImGui::SameLine();
	ImGui::RadioButton("MakeCell", &m_iSelectOption, OPTION_CREATE); ImGui::SameLine();
	ImGui::RadioButton("EditCell", &m_iSelectOption, OPTION_PICK);

	if (ImGui::Button("Delete_Before_CELL") && m_Cells.size())
	{
		_uint vtxCnt[3] = {0};
		_uint tempIdx[3] = {};
		for (int i = 0; i < 3; i++)
		{
			tempIdx[i] =m_Cells[m_Cells.size()-1].m_iIndices[i];
		}
		Safe_Release(m_Cells.back().m_pBufferCom);
		m_Cells.pop_back();

		for (COLCELLDESC cell : m_Cells)
		{
			for (int i = 0; i < 3; i++)
			{
				if (tempIdx[0] == cell.m_iIndices[i])
					vtxCnt[0]++;
				if (tempIdx[1] == cell.m_iIndices[i])
					vtxCnt[1]++;
				if (tempIdx[2] == cell.m_iIndices[i])
					vtxCnt[2]++;
			}
		}
		for (int i = 0; i < 3; i++)
		{
			//찾아서 없애야할듯
			if (vtxCnt[i] == 0)
			{
				m_Vertices.erase(m_Vertices.begin()+ tempIdx[i]);
			}
		}
	}

	if(ImGui::Button("Save_Collider"))
	{
		Save_MeshData();
	}

	ImGui::End();
}

HRESULT CTriangleCollider_Window::Render()
{
	if (FAILED(Render_Cells()))
		return E_FAIL;

	if (m_pTriangleColMesh != nullptr)
		m_pTriangleColMesh->Render();

	return S_OK;
}

void CTriangleCollider_Window::OpenFile_Button()
{
	// open Dialog Simple
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose File", ".dat,.gcm", "../../Resources/Models/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			_char fileName[MAX_PATH] = "";
			_splitpath_s(strFilePathName.c_str(), nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

			_tchar wszfilePath[MAX_PATH] = {};
			_tchar wszfileName[MAX_PATH] = {};
			CharToWChar(strFilePathName.c_str(), wszfilePath);
			CharToWChar(fileName, wszfileName);
			_tchar wszModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_");
			char szModelTag[MAX_PATH] = {};
			lstrcat(wszModelTag, wszfileName);
			WCharToChar(wszModelTag, szModelTag);

			//모델이름 저장
			m_vecModelList_t.push_back(wszModelTag);
			m_vecModelList.push_back(szModelTag);

			_float4x4 PivotMatrix = XMMatrixIdentity();
			PivotMatrix = XMMatrixIdentity();
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, m_vecModelList_t[m_iMaxModelIndex].c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, wszfilePath, PivotMatrix))))
			{
				MSG_BOX("Failed to Create Model");
			}

			m_pTriangleColMesh->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
			lstrcpy(m_wszCurrentDummyModelTag, m_vecModelList_t[m_iModelIndex].c_str());

			//다른 dat 파일도 읽어옵시다.
			_tcscat_s(wszfileName, MAX_PATH, TEXT("_COL"));
			_tchar* lastBackslash = _tcsrchr(wszfilePath, TEXT('\\'));
			if (lastBackslash) {
				size_t insertPos = lastBackslash - wszfilePath;
				_tcscpy_s(wszfilePath + insertPos, MAX_PATH - insertPos, TEXT("_COL/"));
				_tchar wszColFullPath[MAX_PATH] = {};
				lstrcat(wszColFullPath, wszfilePath);
				lstrcat(wszColFullPath, wszfileName);
				lstrcat(wszColFullPath, TEXT(".dat"));
				lstrcpy(m_CurrentFilePath_COL, wszColFullPath);
				//위 파일 이름으로 정점 인덱스 데이터를 불러옵니다.
				if (FAILED(Read_File_Data(wszColFullPath)))
				{
					MSG_BOX("Failed to Create ColData");
				}
			}

			

			Safe_Release(pGameInstance);
			m_iMaxModelIndex++;
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void CTriangleCollider_Window::AddModel_Button()
{
	if (ImGui::Button("AddModelToDummy"))
	{
		if (m_pTriangleColMesh == nullptr)
		{
			MSG_BOX("Failed to Add Model");
			return;
		}

		m_pTriangleColMesh->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
		lstrcpy(m_wszCurrentDummyModelTag, m_vecModelList_t[m_iModelIndex].c_str());
		m_pTriangleColMesh->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		CModel* pCurrentModel = dynamic_cast<CModel*>(m_pTriangleColMesh->Find_Component(TEXT("Com_Model")));
	}
}

_bool CTriangleCollider_Window::Get_VertexIndex_By_Picking(_Inout_ _float4* pPickPosition)
{
	if (nullptr == m_pTriangleColMesh)
		return false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (false == pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		Safe_Release(pGameInstance);
		return false;
	}

	vector<_float3> Vertices = m_pTriangleColMesh->Get_VertexPositions();

	vector<_ulong> Indices = m_pTriangleColMesh->Get_Indices();

	if (Vertices.size() == 0 || Indices.size() == 0)
		return false;

	_float4 vMouseOrigin, vMouseDirection;
	if (FAILED(pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
	{
		Safe_Release(pGameInstance);
		return false;
	}
	vMouseDirection.Normalize();

	Safe_Release(pGameInstance);

	_float fDist = 1000.f;
	_float fReturnDist = fDist;

	_float3 vVertex0, vIntersectVertex0;
	_float3 vVertex1, vIntersectVertex1;
	_float3 vVertex2, vIntersectVertex2;

	_bool    isIntersects = { false };
	for (_uint i = 0; i < Indices.size() - 3;)
	{
		vVertex0 = Vertices[Indices[i++]];
		vVertex1 = Vertices[Indices[i++]];
		vVertex2 = Vertices[Indices[i++]];

		if (TriangleTests::Intersects(vMouseOrigin, vMouseDirection, vVertex0, vVertex1, vVertex2, fDist))
		{
			if (fDist < fReturnDist)
			{
				fReturnDist = fDist;
				isIntersects = true;
				//카메라 위치에서 dir로 dist만큼 간 점.
				*pPickPosition = vMouseOrigin + (vMouseDirection * fDist);
			}
		}
	}
	return isIntersects;
}

_bool CTriangleCollider_Window::Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _uint* iIndex)
{
	_bool bReturnData = { false };
	_float fDistance = { 1000.f };
	_float fReturnDist = { 10000.f };

	_uint iSphereIndex = { 0 };
	for (auto& CellDesc : m_Cells)
	{
		for (auto& pSphere : CellDesc.m_pBufferCom->Get_BoundingSpheres())
		{
			if (true == pSphere->Get_Bounding()->Intersects(vOrigin, vDirection, fDistance))
			{
				if (fDistance < fReturnDist)
				{
					fReturnDist = fDistance;
					*iIndex = CellDesc.m_iIndices[iSphereIndex];
				}
				bReturnData = true;
			}
			++iSphereIndex;
		}
		iSphereIndex = 0;
	}
	return bReturnData;
}

void CTriangleCollider_Window::EditVertices(_float fTimeDelta)
{
	if (OPTION_PICK == m_iSelectOption)
	{
		if (m_pGameInstance->Get_DIKeyState(DIK_X, CInput_Device::KEY_DOWN))
		{
			m_EidtMoveType = EDIT_X;
		}
		else if (m_pGameInstance->Get_DIKeyState(DIK_Y, CInput_Device::KEY_DOWN))
		{
			m_EidtMoveType = EDIT_Y;
		}
		else if (m_pGameInstance->Get_DIKeyState(DIK_Z, CInput_Device::KEY_DOWN))
		{
			m_EidtMoveType = EDIT_Z;
		}

		if (m_pGameInstance->Get_DIKeyState(DIK_G, CInput_Device::KEY_DOWN))
		{
			m_isMoveTrigger = !m_isMoveTrigger;
		}
		
		if (m_Vertices.size() > 0&& m_isMoveTrigger)
		{
			//움직여줍니다.
			_long dwMouseMove = { 0 };
			dwMouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);
			if (0 != dwMouseMove)
			{
				_float3 vPosition = m_Vertices[m_vPickIndex];
				if(m_EidtMoveType == EDIT_X)
					vPosition.x -= _float(dwMouseMove * fTimeDelta * 0.8f);
				if (m_EidtMoveType == EDIT_Y)
					vPosition.y -= _float(dwMouseMove * fTimeDelta * 0.8f);
				if (m_EidtMoveType == EDIT_Z)
					vPosition.z -= _float(dwMouseMove * fTimeDelta * 0.8f);
				m_Vertices[m_vPickIndex] = vPosition;
				if (FAILED(ReMake_Cell()))
					return;
			}
		}
	}
}

HRESULT CTriangleCollider_Window::Save_MeshData()
{
	HANDLE hFile = CreateFile(m_CurrentFilePath_COL, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;
	_uint iMeshCount = 1;
	WriteFile(hFile, &iMeshCount, sizeof(_uint), &dwByte, nullptr);

	//저장하기 위한 콜라이더 매쉬를 생성합니다.
	COLMESH Mesh;
	Mesh.iNumVertices = m_Vertices.size();
	Mesh.iNumFaces = m_Cells.size();

	Mesh.vPositions = new XMFLOAT3[Mesh.iNumVertices];
	for (int i = 0; i < Mesh.iNumVertices; i++)
	{
		Mesh.vPositions[i] = m_Vertices[i];
	}

	Mesh.Faces = new FACE[Mesh.iNumFaces];
	for (int i = 0; i < Mesh.iNumFaces; i++)
	{
		//여기 지금 이상하
		Mesh.Faces[i].iIndices = new _uint[3];
		Mesh.Faces[i].iIndices[0] = m_Cells[i].m_iIndices[0];
		Mesh.Faces[i].iIndices[1] = m_Cells[i].m_iIndices[1];
		Mesh.Faces[i].iIndices[2] = m_Cells[i].m_iIndices[2];
		Mesh.Faces[i].iNumIndices = 3;
	}

	// Mesh NumVertices
	WriteFile(hFile, &(Mesh.iNumVertices), sizeof(_uint), &dwByte, nullptr);
	// Mesh NumFaces
	WriteFile(hFile, &(Mesh.iNumFaces), sizeof(_uint), &dwByte, nullptr);

	//0다음 1번부터 이상한게 들어가는데?
	for (_uint j = 0; j < Mesh.iNumFaces; ++j)
	{
		FACE Face = Mesh.Faces[j];
		// Face NumIndices
		WriteFile(hFile, &(Face.iNumIndices), sizeof(_uint), &dwByte, nullptr);
		// Face Indices
		WriteFile(hFile, Face.iIndices, sizeof(_uint) * Face.iNumIndices, &dwByte, nullptr);
	}
	// Mesh Positions
	WriteFile(hFile, Mesh.vPositions, sizeof(_float3) * Mesh.iNumVertices, &dwByte, nullptr);
	
	CloseHandle(hFile);

	for (_uint j = 0; j < Mesh.iNumFaces; ++j)
	{
		Safe_Delete_Array(Mesh.Faces[j].iIndices);
	}
	Safe_Delete_Array(Mesh.Faces);

	Safe_Delete_Array(Mesh.vPositions);

	return S_OK;
}

HRESULT CTriangleCollider_Window::Read_File_Data(_tchar* pModelFilePath)
{
	HANDLE hFile = CreateFile(pModelFilePath,
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_ulong	dwStrByte = 0;

	// Meshes NumMeshes
	_uint iNumMeshes = { 0 };
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		COLMESH Mesh;
		// Mesh NumVertices
		ReadFile(hFile, &(Mesh.iNumVertices), sizeof(_uint), &dwByte, nullptr);

		// Mesh NumFaces
		ReadFile(hFile, &(Mesh.iNumFaces), sizeof(_uint), &dwByte, nullptr);

		Mesh.Faces = new FACE[Mesh.iNumFaces];
		ZeroMemory(Mesh.Faces, sizeof(FACE) * (Mesh.iNumFaces));

		for (_uint j = 0; j < Mesh.iNumFaces; ++j)
		{
			FACE Face;
			ZEROMEM(&Face);

			// Face NumIndices
			ReadFile(hFile, &(Face.iNumIndices), sizeof(_uint), &dwByte, nullptr);

			// Face Indices
			Face.iIndices = new _uint[Face.iNumIndices];
			ZeroMemory(Face.iIndices, sizeof(_uint) * (Face.iNumIndices));
			ReadFile(hFile, Face.iIndices, sizeof(_uint) * (Face.iNumIndices), &dwByte, nullptr);

			Mesh.Faces[j] = Face;
		}

		// Mesh Positions
		Mesh.vPositions = new _float3[Mesh.iNumVertices];
		ZeroMemory(Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices));
		ReadFile(hFile, Mesh.vPositions, sizeof(_float3) * (Mesh.iNumVertices), &dwByte, nullptr);

		//매쉬에 들어있던 모든 정점들을 추가해줍니다.
		for (int i = 0; i < Mesh.iNumVertices; i++)
			m_Vertices.push_back(Mesh.vPositions[i]);

		//위 정점들을 기반으로 포인트들을 만들어줍니다.
		Create_COLCELL(&Mesh);
		Release_Mesh(&Mesh);

	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CTriangleCollider_Window::Release_Mesh(COLMESH* Mesh)
{
	for (_uint j = 0; j < Mesh->iNumFaces; ++j)
	{
		Safe_Delete_Array(Mesh->Faces[j].iIndices);
	}
	Safe_Delete_Array(Mesh->Faces);
	Safe_Delete_Array(Mesh->vPositions);
	return S_OK;
}

HRESULT CTriangleCollider_Window::Create_COLCELL(COLMESH* PMesh)
{
	_uint indicesNum = PMesh->iNumFaces;
	for (_uint i = 0; i < indicesNum; ++i)
	{
		COLCELLDESC CellDesc;

		CellDesc.m_iIndices[0] = PMesh->Faces[i].iIndices[0];
		CellDesc.m_iIndices[1] = PMesh->Faces[i].iIndices[1];
		CellDesc.m_iIndices[2] = PMesh->Faces[i].iIndices[2];

		_float3 data[3] = {};
		data[0] = m_Vertices[CellDesc.m_iIndices[0]];
		data[1] = m_Vertices[CellDesc.m_iIndices[1]];
		data[2] = m_Vertices[CellDesc.m_iIndices[2]];
		CellDesc.m_pBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, data);
		if (nullptr == CellDesc.m_pBufferCom)
		{
			MSG_BOX("Failed NavigationRead_File : CellDesc.m_pBufferCom is nullptr");
			return E_FAIL;
		}
		m_Cells.push_back(CellDesc);
	}
	//Remake_Cells();
	return S_OK;
}

HRESULT CTriangleCollider_Window::Make_Cell()
{
	if (!m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		return S_OK;


	if (false == m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		return S_OK;
	}

	vector<_float3> Vertices = m_pTriangleColMesh->Get_VertexPositions();
	vector<_ulong> Indices = m_pTriangleColMesh->Get_Indices();

	if (Vertices.size() == 0 || Indices.size() == 0)
		return S_OK;

	_float4 vMouseOrigin, vMouseDirection;
	if (FAILED(m_pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
	{
		return S_OK;
	}
	vMouseDirection.Normalize();


	if (Pick_Spheres(vMouseOrigin, vMouseDirection, &m_vPickIndex))
	{
	}
	else if (OPTION_CREATE == m_iSelectOption && 
		Get_VertexIndex_By_Picking(&m_vPickPos))
	{
		//m_vPickPos위치에 새로운 정점 생성해줘야함.
		m_Vertices.push_back(m_vPickPos.xyz());
		m_vPickIndex = m_Vertices.size() - 1;
	}
	else
		return S_OK;

	//셀만들기 시작
	if (OPTION_CREATE == m_iSelectOption)
	{
		m_isMakingCell = true;

		if (true == m_pGameInstance->Get_DIKeyState(DIK_LCONTROL))
		{
			m_vPickPos.x = roundf(m_vPickPos.x);
			m_vPickPos.y = roundf(m_vPickPos.y);
			m_vPickPos.z = roundf(m_vPickPos.z);
		}

		if (CCell::POINT_END > m_iCurrentPickPointIndex)
		{
			//현재 피킹 인덱스가 3 미만이면 새로운 그거 추가함.
			if (m_vPickIndex != -1)
			{
				m_vCell[m_iCurrentPickPointIndex++] = m_vPickIndex;
			}
		}

		// 3번의 피킹이 끝나면 벡터컨테이너에 값을 넣고 피킹 인덱스 초기화
		if (CCell::POINT_END == m_iCurrentPickPointIndex)
		{
			_float3 data[3] = {};

			CCWSort_Cell(m_vCell);
			COLCELLDESC CellDesc;
			for (int i = 0; i < 3; i++)
			{
				CellDesc.m_iIndices[i] = m_vCell[i];
			}

			//포지션배열 던져줘야함
			data[0] = m_Vertices[m_vCell[0]];
			data[1] = m_Vertices[m_vCell[1]];
			data[2] = m_Vertices[m_vCell[2]];
			CellDesc.m_pBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, data);
			if (nullptr == CellDesc.m_pBufferCom)
			{
				MSG_BOX("Failed Make_Cell : CellDesc.m_pBufferCom is nullptr");
				return E_FAIL;
			}

			m_Cells.push_back(CellDesc);

			m_iCurrentPickPointIndex = CCell::POINT_A;

			m_isMakingCell = false;
		}
		return S_OK;
	}
}

HRESULT CTriangleCollider_Window::ReMake_Cell()
{
	for (auto& CellDesc : m_Cells)
	{
		_float3 pointArr[3] = {};
		pointArr[0] = m_Vertices[CellDesc.m_iIndices[0]];
		pointArr[1] = m_Vertices[CellDesc.m_iIndices[1]];
		pointArr[2] = m_Vertices[CellDesc.m_iIndices[2]];
		CellDesc.m_pBufferCom->Begin(pointArr);
		CellDesc.m_pBufferCom->End();
	}
	return S_OK;
}

void CTriangleCollider_Window::CCWSort_Cell(_uint* pPoints)
{
	_float4 vAB, vAC, vCross;
	_float4 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vAB = _float4(m_Vertices[pPoints[CCell::POINT_B]] - m_Vertices[pPoints[CCell::POINT_A]]);
	vAC = _float4(m_Vertices[pPoints[CCell::POINT_C]] - m_Vertices[pPoints[CCell::POINT_A]]);

	vCross = XMVector3Normalize(XMVector3Cross(vAB, vAC));

	// not CCW
	if (0 > XMVectorGetX(XMVector3Dot(vUp, vCross)))
	{
		UINT vSour = pPoints[CCell::POINT_B];
		pPoints[CCell::POINT_B] = pPoints[CCell::POINT_C];
		pPoints[CCell::POINT_C] = vSour;
	}
}

HRESULT CTriangleCollider_Window::Render_Cells()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float4		vColor = _float4(0.f, 1.f, 0.f, 1.f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("Debug")))
		return E_FAIL;

	for (auto& CellDesc : m_Cells)
	{
		if (FAILED(CellDesc.m_pBufferCom->Render()))
			return E_FAIL;
	}

#ifdef _DEBUG
	for (auto& CellDesc : m_Cells)
	{
		for (auto& Sphere : CellDesc.m_pBufferCom->Get_BoundingSpheres())
		{
#ifdef _DEBUG
			if (FAILED(Sphere->Render()))
				return E_FAIL;
#endif // _DEBUG
		}
	}
#endif // _DEBUG	

	vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	WorldMatrix._42 = 0.1f;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("Debug")))
		return E_FAIL;

	if (0 < m_Cells.size())
	{
		if (FAILED(m_Cells[m_iCurrentCellIndex].m_pBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

CTriangleCollider_Window* CTriangleCollider_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CTriangleCollider_Window* pInstance = New CTriangleCollider_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed to Created CTriangleCollider_Window");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTriangleCollider_Window::Free()
{
	__super::Free();

	for (auto& CellDesc : m_Cells)
		Safe_Release(CellDesc.m_pBufferCom);
	m_Cells.clear();
	Safe_Release(m_pTriangleColMesh);
	Safe_Release(m_pShaderCom);
}
