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

	m_pShaderCom = static_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Navigation")));
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

	ImGui::Begin("Navigation", nullptr, m_WindowFlag);

	OpenFile_Button();
	AddModel_Button();
	if (m_pTriangleColMesh != nullptr)
		m_pTriangleColMesh->Tick(fTimeDelta);

	/*_float3 pickPos = {};
	if (OPTION_CREATE == m_iSelectOption&&
		m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		Get_VertexIndex_By_Picking(&pickPos))
	{
		Make_Cell();
	}*/
	Make_Cell();
	/* Check Option */
	ImGui::RadioButton("Nothing", &m_iSelectOption, OPTION_NOTHING); ImGui::SameLine();
	ImGui::RadioButton("Make Navi", &m_iSelectOption, OPTION_CREATE); ImGui::SameLine();
	ImGui::RadioButton("Pick Navi", &m_iSelectOption, OPTION_PICK);

	//Pick_Navigation(fTimeDelta);
	CurrentNavigationPosition();
	Navigation_List();
	Delete_Cell();

	ImGui::End();
}

HRESULT CTriangleCollider_Window::Render()
{
	if (FAILED(Render_Cells()))
		return E_FAIL;

	if (true == m_isMakingCell)
	{
		_tchar szPickIndex[20] = TEXT("");
		_itow_s(m_iCurrentPickPointIndex, szPickIndex, sizeof(4), 10);

		_tchar szFont[20] = TEXT("Current Index : ");

		lstrcat(szFont, szPickIndex);

		_float2 vCursorPos;
		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		vCursorPos.x = (_float)pt.x;
		vCursorPos.y = (_float)pt.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), szPickIndex, vCursorPos, _float4(0.f, 0.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 2.f)))
			return E_FAIL;
	}

	for (auto& CellDesc : m_Cells)
	{
		_float4 vCenter = XMVectorZero();

		for (_uint i = 0; i < CCell::POINT_END; ++i)
		{
			vCenter += CellDesc.m_Points[CCell::POINT(i)].TransCoord();
		}

		vCenter /= CCell::POINT_END;

		_float4x4 ViewMatrix = *m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
		_float4x4 ProjMatrix = *m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

		_float4 vViewPos = XMVector3TransformCoord(vCenter, ViewMatrix);
		_float4 vProjPos = XMVector3TransformCoord(vViewPos, ProjMatrix);

		_uint iVP = 1;
		D3D11_VIEWPORT ViewPort;
		std::ZEROMEM(&ViewPort);
		m_pContext->RSGetViewports(&iVP, &ViewPort);

		_float3 vRenderCenterPos = vProjPos.xyz();
		_float2 vRender;
		// 투영 좌표가 아닌 친구들을 렌더링 하지 않게 처리.
		if (-1.f > vRenderCenterPos.x ||
			1.f < vRenderCenterPos.x ||
			-1.f > vRenderCenterPos.y ||
			1.f < vRenderCenterPos.y ||
			0.f > vRenderCenterPos.z ||
			1.f < vRenderCenterPos.z)
			continue;

		vRender.x = (vRenderCenterPos.x + 1.f) * 0.5f * ViewPort.Width;
		vRender.y = (1.f - vRenderCenterPos.y) * 0.5f * ViewPort.Height;

		_float fDepth = (1.f - vRenderCenterPos.z) * 200.f;
	}

	if (0 < m_Cells.size())
	{
		_uint iCellIndex = { 0 };
		for (auto& Point : m_Cells[m_iCurrentCellIndex].m_Points)
		{
			_float4 vPosition = Point.TransCoord();

			_float4x4 ViewMatrix = *m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
			_float4x4 ProjMatrix = *m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

			_float4 vViewPos = XMVector3TransformCoord(vPosition, ViewMatrix);
			_float4 vProjPos = XMVector3TransformCoord(vViewPos, ProjMatrix);

			_uint iVP = 1;
			D3D11_VIEWPORT ViewPort;
			ZEROMEM(&ViewPort);
			m_pContext->RSGetViewports(&iVP, &ViewPort);

			_float3 vRenderCenterPos = vProjPos.xyz();
			_float2 vRender;
			// 투영 좌표가 아닌 친구들을 렌더링 하지 않게 처리.
			if (-1.f > vRenderCenterPos.x ||
				1.f < vRenderCenterPos.x ||
				-1.f > vRenderCenterPos.y ||
				1.f < vRenderCenterPos.y ||
				0.f > vRenderCenterPos.z ||
				1.f < vRenderCenterPos.z)
			{
				++iCellIndex;
				continue;
			}

			vRender.x = (vRenderCenterPos.x + 1.f) * 0.5f * ViewPort.Width;
			vRender.y = (1.f - vRenderCenterPos.y) * 0.5f * ViewPort.Height;

			_float fDepth = (1.f - vRenderCenterPos.z) * 200.f;

			wstring wstrCellIndex = to_wstring(iCellIndex++);
			
			if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), wstrCellIndex.c_str(), vRender, _float4(1.f, 0.f, 1.f, 1.f), 0.f, _float2(0.f, 0.f), 1.f)))
				return E_FAIL;
		}
	}
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

_bool CTriangleCollider_Window::Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _Inout_ vector<PICKCOLDESC>& Out)
{
	Out.clear();

	_bool bReturnData = { false };
	_float fDistance = { 0.f };

	_uint iSphereIndex = { 0 };
	for (auto& CellDesc : m_Cells)
	{
		for (auto& pSphere : CellDesc.m_pBufferCom->Get_BoundingSpheres())
		{
			if (true == pSphere->Get_Bounding()->Intersects(vOrigin, vDirection, fDistance))
			{
				PICKCOLDESC InputDesc;

				InputDesc.m_iCellIndex = iSphereIndex / CCell::POINT_END;
				InputDesc.m_ePoint = CCell::POINT(iSphereIndex % CCell::POINT_END);
				InputDesc.m_fDistance = fDistance;

				Out.push_back(InputDesc);
				bReturnData = true;
			}
			++iSphereIndex;
		}
	}

	return bReturnData;
}

HRESULT CTriangleCollider_Window::Save_MeshData()
{

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
		
		CellDesc.m_Points[0] = PMesh->vPositions[PMesh->Faces[i].iIndices[0]];
		CellDesc.m_Points[1] = PMesh->vPositions[PMesh->Faces[i].iIndices[1]];
		CellDesc.m_Points[2] = PMesh->vPositions[PMesh->Faces[i].iIndices[2]];

		CellDesc.m_pBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vCell.data());
		if (nullptr == CellDesc.m_pBufferCom)
		{
			MSG_BOX("Failed NavigationRead_File : CellDesc.m_pBufferCom is nullptr");
			return E_FAIL;
		}
		m_Cells.push_back(CellDesc);
	}
	Remake_Cells();
	return S_OK;
}

HRESULT CTriangleCollider_Window::Make_Cell()
{
	/* 현재 옵션이 셀 생성인 경우에만 */
	if (OPTION_CREATE != m_iSelectOption)
		return S_OK;

	if (!m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		return S_OK;

	

	/* 만약 피킹한 셀의 콜라이더 구체를 피킹하지 않았다면 */
	if (0 == m_PickDatas.size())
	{
		//위치가 이상한지 이상한곳에서 생성되는중임.
		if (!Get_VertexIndex_By_Picking(&m_vPickPos))
			return S_OK;
	}
	else /* 콜라이더 구체를 피킹한 경우 */
	{
		COLCELLDESC CellDesc = m_Cells[m_PickDatas.front().m_iCellIndex];
		m_vPickPos = CellDesc.m_Points[m_PickDatas.front().m_ePoint].TransCoord();
	}
	m_isMakingCell = true;
	// 컨트롤을 누르고 피킹하면 반올림하여 처리
	if (true == m_pGameInstance->Get_DIKeyState(DIK_LCONTROL))
	{
		m_vPickPos.x = roundf(m_vPickPos.x);
		m_vPickPos.y = roundf(m_vPickPos.y);
		m_vPickPos.z = roundf(m_vPickPos.z);
	}

	// 피킹처리
	if (CCell::POINT_END > m_iCurrentPickPointIndex)
	{
		m_vCell[m_iCurrentPickPointIndex++] = m_vPickPos.xyz();
	}

	// 3번의 피킹이 끝나면 벡터컨테이너에 값을 넣고 피킹 인덱스 초기화
	if (CCell::POINT_END == m_iCurrentPickPointIndex)
	{
		CCWSort_Cell(m_vCell.data());

		COLCELLDESC CellDesc;
		CellDesc.m_Points = m_vCell;
		CellDesc.m_pBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vCell.data());
		if (nullptr == CellDesc.m_pBufferCom)
		{
			MSG_BOX("Failed Make_Cell : CellDesc.m_pBufferCom is nullptr");
			return E_FAIL;
		}

		m_Cells.push_back(CellDesc);

		m_iCurrentPickPointIndex = CCell::POINT_A;

		m_PickDatas.clear();
		m_isMakingCell = false;
	}
	return S_OK;
}

HRESULT CTriangleCollider_Window::CurrentNavigationPosition()
{
	if (0 == m_Cells.size())
	{
		ImGui::Text("Cell Empty.");
		return S_OK;
	}

	ImGui::SetNextItemWidth(150.f);

	if (ImGui::DragFloat3("Cell A",
		reinterpret_cast<_float*>(&m_Cells[m_iCurrentCellIndex].m_Points[CCell::POINT_A]), 0.01f, 0.f, 5000.f, "%.2f"))
	{
		CCWSort_Cell(m_Cells[m_iCurrentCellIndex].m_Points.data());
		Remake_Cells();
	}
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::DragFloat3("Cell B",
		reinterpret_cast<_float*>(&m_Cells[m_iCurrentCellIndex].m_Points[CCell::POINT_B]), 0.01f, 0.f, 5000.f, "%.2f"))
	{
		CCWSort_Cell(m_Cells[m_iCurrentCellIndex].m_Points.data());
		Remake_Cells();
	}
	ImGui::SetNextItemWidth(150.f);
	if (ImGui::DragFloat3("Cell C",
		reinterpret_cast<_float*>(&m_Cells[m_iCurrentCellIndex].m_Points[CCell::POINT_C]), 0.01f, 0.f, 5000.f, "%.2f"))
	{
		CCWSort_Cell(m_Cells[m_iCurrentCellIndex].m_Points.data());
		Remake_Cells();
	}

	return S_OK;
}

HRESULT CTriangleCollider_Window::Navigation_List()
{
	ImGui::SetNextItemWidth(150.f);

	if (ImGui::BeginListBox("Cell Index"))
	{
		m_iSelecIndex = 0;

		for (auto& CellDesc : m_Cells)
		{
			//if (ImGui::Selectable(CellDesc.m_strIndexName.c_str()))
			{
				m_iCurrentCellIndex = m_iSelecIndex;

				CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
				if (nullptr == pCamera)
					return E_FAIL;

				// 셀의 중심값을 바라보게 카메라 처리
				_float4 vTransform;
				ZEROMEM(&vTransform);
				for (_uint i = 0; i < CCell::POINT_END; ++i)
				{
					vTransform += m_Cells[m_iCurrentCellIndex].m_Points[CCell::POINT(i)].TransCoord();
				}

				_float4 vCamPos;
				_float4 vPos = vCamPos = vTransform / 3.f;

				vCamPos.y += 5.f;
				vCamPos.z -= 5.f;
				//pCamera->Set_CameraView(vCamPos, vPos, _float4(0.f, 1.f, 0.f, 0.f));

				break;
			}
			++m_iSelecIndex;
		}
		ImGui::EndListBox();
	}

	return S_OK;
}

HRESULT CTriangleCollider_Window::Delete_Cell()
{
	if (ImGui::Button("Delete Cell", ImVec2(100, 20)) && m_Cells.size() > 0)
	{
		Safe_Release(m_Cells[m_iCurrentCellIndex].m_pBufferCom);

		auto iter = m_Cells.begin();
		m_Cells.erase(iter + m_iCurrentCellIndex);

		if (m_Cells.size() == m_iCurrentCellIndex)
			m_iCurrentCellIndex = (0 == m_iCurrentCellIndex) ? 0 : --m_iCurrentCellIndex;
	}

	return S_OK;
}

HRESULT CTriangleCollider_Window::Remake_Cells()
{
	for (auto& CellDesc : m_Cells)
	{
		CellDesc.m_pBufferCom->Begin(CellDesc.m_Points.data());
		CellDesc.m_pBufferCom->End();
	}

	return S_OK;
}

void CTriangleCollider_Window::CCWSort_Cell(_float3* pPoints)
{
	_float4 vAB, vAC, vCross;
	_float4 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vAB = _float4(pPoints[CCell::POINT_B] - pPoints[CCell::POINT_A]);
	vAC = _float4(pPoints[CCell::POINT_C] - pPoints[CCell::POINT_A]);

	vCross = XMVector3Normalize(XMVector3Cross(vAB, vAC));

	// not CCW
	if (0 > XMVectorGetX(XMVector3Dot(vUp, vCross)))
	{
		_float3 vSour = pPoints[CCell::POINT_B];
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

	if (FAILED(m_pShaderCom->Begin("Cell")))
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
			if (FAILED(Sphere->Render()))
				return E_FAIL;
		}
	}
#endif // _DEBUG	

	vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	WorldMatrix._42 = 0.1f;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin("Cell")))
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
