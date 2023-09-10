#include "..\Public\Navigation_Window.h"
#include "ImGuiFileDialog.h"

#include "Terrain.h"
#include "Camera_Free.h"

CNavigation_Window::CNavigation_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
	, m_vPickPos()
{
}

HRESULT CNavigation_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
		TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));
	if (nullptr == m_pTerrain)
		return E_FAIL;

	Safe_AddRef(m_pTerrain);

	m_pShaderCom = static_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Navigation")));
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

void CNavigation_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	for (auto& CellDesc : m_Cells)
	{
		CellDesc.m_pBufferCom->Tick(fTimeDelta);
	}

	ImGui::Begin("Navigation", nullptr, m_WindowFlag);

	/* Check Option */
	ImGui::RadioButton("Nothing", &m_iSelectOption, OPTION_NOTHING); ImGui::SameLine();
	ImGui::RadioButton("Make Navi", &m_iSelectOption, OPTION_CREATE); ImGui::SameLine();
	ImGui::RadioButton("Pick Navi", &m_iSelectOption, OPTION_PICK);

	Pick_Navigation(fTimeDelta);

	CurrentNavigationPosition();

	Navigation_List();

	Delete_Cell();

	NavigationSaveLoad();

	ImGui::End();
}

HRESULT CNavigation_Window::Render()
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
			continue;

		vRender.x = (vRenderCenterPos.x + 1.f) * 0.5f * ViewPort.Width;
		vRender.y = (1.f - vRenderCenterPos.y) * 0.5f * ViewPort.Height;

		_float fDepth = (1.f - vRenderCenterPos.z) * 200.f;

		wstring wstrCellIndex = strToWStr(CellDesc.m_strIndexName);

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font_135"), wstrCellIndex.c_str(), vRender, _float4(1.f, 0.f, 0.f, 1.f), 0.f, _float2(0.f, 0.f), fDepth)))
			return E_FAIL;
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

	return S_OK;
}

_bool CNavigation_Window::Pick_Spheres(const _float4& vOrigin, const _float4& vDirection, _Inout_ vector<PICKDESC>& Out)
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
				PICKDESC InputDesc;

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

HRESULT CNavigation_Window::Pick_Navigation(_float fTimeDelta)
{
	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		_float4 vOrigin;
		_float4 vDirection;
		m_pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vOrigin, &vDirection);
		Pick_Spheres(vOrigin, vDirection, m_PickDatas);

		if (OPTION_CREATE == m_iSelectOption)
		{
			Make_Cell();
		}
		else if (OPTION_PICK == m_iSelectOption)
		{
			
		}
	}

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		OPTION_PICK == m_iSelectOption)
	{
		_long dwMouseMove = { 0 };
		dwMouseMove = m_pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);

		if (0 != dwMouseMove)
		{
			for (auto PickDesc : m_PickDatas)
			{
				// Buffer.first == 버퍼의 인덱스 (POINT A,B,C)
				if (CCell::POINT_END <= PickDesc.m_ePoint ||
					0 > PickDesc.m_ePoint)
					return E_FAIL;

				_float3 vPosition = m_Cells[PickDesc.m_iCellIndex].m_Points[PickDesc.m_ePoint];
				vPosition.y -= _float(dwMouseMove * fTimeDelta * 0.8f);
				m_Cells[PickDesc.m_iCellIndex].m_Points[PickDesc.m_ePoint] = vPosition;
				if (FAILED(Remake_Cells()))
					return E_FAIL;
			}
		}

	}

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_UP) &&
		0 < m_PickDatas.size())
	{
		CELLDESC StandardCell = m_Cells[m_PickDatas[0].m_iCellIndex];
		_float3 vStandardPosition = StandardCell.m_Points[m_PickDatas[0].m_ePoint];
		for (auto& PickDesc : m_PickDatas)
		{
			_float vDistance = _float3::Distance(vStandardPosition,
				m_Cells[PickDesc.m_iCellIndex].m_Points[PickDesc.m_ePoint]);
			if(0.3f > vDistance)
				m_Cells[PickDesc.m_iCellIndex].m_Points[PickDesc.m_ePoint] = vStandardPosition;
		}
		// 현재 움직이고 있던 버퍼 포지션 클리어.
		m_PickDatas.clear();
		m_vPickPos = _float4(0.f, 0.f, 0.f, 1.f);

		Remake_Cells();
	}

	return S_OK;
}

HRESULT CNavigation_Window::Make_Cell()
{
	/* 현재 옵션이 셀 생성인 경우에만 */
	if (OPTION_CREATE != m_iSelectOption)
		return S_OK;

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) &&
		m_pGameInstance->IsMouseInClient(m_pContext, g_hWnd))
	{
		m_isMakingCell = true;

		/* 만약 피킹한 셀의 콜라이더 구체를 피킹하지 않았다면 */
		if (0 == m_PickDatas.size())
		{
			if (FAILED(m_pTerrain->Picking_On_Terrain(&m_vPickPos)))
			{
				MSG_BOX("Failed Make_Cell : Failed Picking_On_Terrain");
				return E_FAIL;
			}
		}
		else /* 콜라이더 구체를 피킹한 경우 */
		{
			CELLDESC CellDesc = m_Cells[m_PickDatas.front().m_iCellIndex];
			m_vPickPos = CellDesc.m_Points[m_PickDatas.front().m_ePoint].TransCoord();
		}

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

			CELLDESC CellDesc;
			CellDesc.m_strIndexName = to_string(m_Cells.size());
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
	}

	return S_OK;
}

HRESULT CNavigation_Window::CurrentNavigationPosition()
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

HRESULT CNavigation_Window::Navigation_List()
{
	ImGui::SetNextItemWidth(150.f);

	if (ImGui::BeginListBox("Cell Index"))
	{
		m_iSelecIndex = 0;

		for (auto& CellDesc : m_Cells)
		{
			if (ImGui::Selectable(CellDesc.m_strIndexName.c_str()))
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

HRESULT CNavigation_Window::Delete_Cell()
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

HRESULT CNavigation_Window::Remake_Cells()
{
	for (auto& CellDesc : m_Cells)
	{
		CellDesc.m_pBufferCom->Begin(CellDesc.m_Points.data());
		CellDesc.m_pBufferCom->End();
	}

	return S_OK;
}

HRESULT CNavigation_Window::NavigationSaveLoad()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("Navigation Save"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("SaveNavigationDialog", "Choose Folder", ".Navi", "Navigation.Navi");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NavigationSaveButton();

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));
	if (ImGui::Button("Navigation Load"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("LoadNavigationDialog", "Choose File", ".Navi", ".");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	NavigationLoadButton();

	return S_OK;
}

HRESULT CNavigation_Window::NavigationSaveButton()
{
	// display
	if (ImGuiFileDialog::Instance()->Display("SaveNavigationDialog"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			_tchar wszPath[MAX_PATH] = TEXT("");
			CharToWChar(filePath.c_str(), wszPath);
			if (FAILED(NavigationWrite_File(wszPath)))
				MSG_BOX("Failed File Write");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CNavigation_Window::NavigationWrite_File(const _tchar* pPath)
{
	HANDLE hFile = CreateFile(pPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;

	_uint iSize = (_uint)m_Cells.size();
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& CellDesc : m_Cells)
	{
		WriteFile(hFile, &CellDesc.m_eFlag, sizeof(CELLFLAG), &dwByte, nullptr);
		WriteFile(hFile, CellDesc.m_Points.data(), sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
	}

	CloseHandle(hFile);

	MSG_BOX("File Save Success");

	return S_OK;
}

HRESULT CNavigation_Window::NavigationLoadButton()
{
	// display
	if (ImGuiFileDialog::Instance()->Display("LoadNavigationDialog"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			map<string, string> strMap = ImGuiFileDialog::Instance()->GetSelection();
			string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			_tchar wszName[MAX_PATH] = TEXT("");
			CharToWChar(filePathName.c_str(), wszName);
			if (FAILED(NavigationRead_File(wszName)))
				MSG_BOX("Failed File Read");
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CNavigation_Window::NavigationRead_File(const _tchar* pFileName)
{
	/* 기존 데이터 살리고 싶으면 여기서 처리하셂. */
	m_Cells.clear();

	HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong	dwByte = 0;
	_uint iSize = 0;
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iSize; ++i)
	{
		CELLDESC CellDesc;
		CellDesc.m_strIndexName = to_string(i);

		ReadFile(hFile, &CellDesc.m_eFlag, sizeof(CELLFLAG), &dwByte, nullptr);
		ReadFile(hFile, CellDesc.m_Points.data(), sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		
		CellDesc.m_pBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vCell.data());
		if (nullptr == CellDesc.m_pBufferCom)
		{
			MSG_BOX("Failed NavigationRead_File : CellDesc.m_pBufferCom is nullptr");
			return E_FAIL;
		}
		m_Cells.push_back(CellDesc);
	}

	Remake_Cells();

	CloseHandle(hFile);

	MSG_BOX("File Load Success");

	return S_OK;
}

void CNavigation_Window::CCWSort_Cell(_float3* pPoints)
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

HRESULT CNavigation_Window::Render_Cells()
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

CNavigation_Window* CNavigation_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CNavigation_Window* pInstance = New CNavigation_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed to Created CNavigation_Window");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation_Window::Free()
{
	__super::Free();

	Safe_Release(m_pTerrain);

	for (auto& CellDesc : m_Cells)
		Safe_Release(CellDesc.m_pBufferCom);
	m_Cells.clear();

	Safe_Release(m_pShaderCom);
}
