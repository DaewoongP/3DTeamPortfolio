#include "..\Public\Cloth_Window.h"
#include "Dummy_Cloth.h"
#include "Camera_Free.h"
#include "Bounding_Sphere.h"
#include "PhysXConverter.h"
#include "Collider.h"

CCloth_Window::CCloth_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}

HRESULT CCloth_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;
	/* For.Prototype_Component_Model_CustomModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Model_CustomModel"),
		CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/DummyCloth/DummyCloth.dat")))))
	{
		MSG_BOX("Failed Create CCustomModel");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy_Cloth"),
		CDummy_Cloth::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Create Dummy_Cloth");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy_Cloth"),
		TEXT("Layer_Dummy_Cloth"), TEXT("Com_Dummy"))))
	{
		MSG_BOX("Failed Clone Dummy_Cloth");
		return E_FAIL;
	}
	
	m_pCollider = static_cast<CCollider*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Sphere_Collider")));
	m_pRenderer = static_cast<CRenderer*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Renderer")));

	m_pDummy_Cloth = static_cast<CDummy_Cloth*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Dummy_Cloth"), TEXT("Com_Dummy")));
	Safe_AddRef(m_pDummy_Cloth);
	m_pCamera_Free = static_cast<CCamera_Free*>(m_pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("GameObject_Camera_Free")));
	Safe_AddRef(m_pCamera_Free);

	return S_OK;
}

void CCloth_Window::Tick(_float _fTimeDelta)
{
	CImWindow::Tick(_fTimeDelta);
	ImGui::Begin("Cloth", nullptr, m_WindowFlag);

	Is_Testing();

	Radio_Select_MeshType();

	Button_Open_ModelDialog();

	Input_Mesh_Index(_fTimeDelta);

	Pick_Mesh(_fTimeDelta);

	Current_Picked_Vertices(_fTimeDelta);

	Make_CapsuleCollider(_fTimeDelta);

	Input_Options(_fTimeDelta);	

	Make_Data();

	ImGui::End();

	Render_Collider();
}

HRESULT CCloth_Window::Render()
{
	m_pDummy_Cloth->Render_Capsules();

	return S_OK;
}

void CCloth_Window::Is_Testing()
{
	if (true == m_isPickMesh ||
		true == m_isPickCollider)
	{
		m_pDummy_Cloth->Set_Testing(false);
	}
	else
		m_pDummy_Cloth->Set_Testing(true);
}

void CCloth_Window::Radio_Select_MeshType()
{
	ImGui::SeparatorText("Select Mesh Type");
	
	ImGui::RadioButton("Head", (_int*)(&m_eMeshType), _int(CCustomModel::HEAD));
	ImGui::SameLine();
	ImGui::RadioButton("Arm", (_int*)(&m_eMeshType), _int(CCustomModel::ARM));
	ImGui::SameLine();
	ImGui::RadioButton("Robe", (_int*)(&m_eMeshType), _int(CCustomModel::ROBE));
	ImGui::SameLine();
	ImGui::RadioButton("Top", (_int*)(&m_eMeshType), _int(CCustomModel::TOP));
	
	ImGui::RadioButton("Pants", (_int*)(&m_eMeshType), _int(CCustomModel::PANTS));
	ImGui::SameLine();
	ImGui::RadioButton("Socks", (_int*)(&m_eMeshType), _int(CCustomModel::SOCKS));
	ImGui::SameLine();
	ImGui::RadioButton("Shoes", (_int*)(&m_eMeshType), _int(CCustomModel::SHOES));

	ImGui::Separator();
}

void CCloth_Window::Button_Open_ModelDialog()
{
	if (ImGui::Button("Open Cloth Model"))
	{
		if (CCustomModel::MESH_END == m_eMeshType)
		{
			MSG_BOX("Please Select Mesh Type");
			return;
		}

		ImGuiFileDialog::Instance()->OpenDialog("Open Cloth Model", "Choose File", ".dat", "../../Resources/Models/");
	}

	ModelDialog();
}

void CCloth_Window::ModelDialog()
{
	if (ImGuiFileDialog::Instance()->Display("Open Cloth Model"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			Create_MeshParts_Prototype(ImGuiFileDialog::Instance()->GetFilePathName());

			Update_Cloth();
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void CCloth_Window::Input_Mesh_Index(_float fTimeDelta)
{
	if (nullptr == m_pDummy_Cloth ||
		nullptr == m_pDummy_Cloth->Get_CustomModel())
		return;

	ImGui::SeparatorText("Mesh Index");
	ImGui::SetNextItemWidth(200.f);
	
	string strMaxInputInt = "Max : ";
	_int iMaxNumMesh = m_pDummy_Cloth->Get_Max_NumMesh();
	_char szMaxNumMesh[MAX_STR] = "";
	_itoa_s(iMaxNumMesh - 1, szMaxNumMesh, MAX_STR, 10);

	strMaxInputInt += szMaxNumMesh;
	_int iPrevIndex = m_iMeshIndex;
	if (ImGui::InputInt(strMaxInputInt.c_str(), &m_iMeshIndex))
	{
		if (false == isValid_Mesh_Index(m_iMeshIndex))
		{
			MSG_BOX("Invalid Index");
			m_iMeshIndex = iPrevIndex;
		}

		m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);

		m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
	}

	ImGui::Separator();
}

_bool CCloth_Window::isValid_Mesh_Index(_int iIndex)
{
	if (-1 > iIndex)
		return false;
	if (m_pDummy_Cloth->Get_Max_NumMesh() <= iIndex)
		return false;
	if (CCustomModel::MESH_END == m_eMeshType)
		return false;

	return true;
}

void CCloth_Window::Pick_Mesh(_float fTimeDelta)
{
	if (nullptr == m_pDummy_Cloth)
		return;

	ImGui::SeparatorText("Setting Vertex");
	if (ImGui::Checkbox("Pick Vertex", &m_isPickMesh))
	{
		m_isPickCollider = false;
	}
	ImGui::SameLine();
	Pick_Collider(fTimeDelta);

	if (ImGui::Checkbox("Mesh HighLight", &m_isMeshHighLight))
	{
		m_pDummy_Cloth->Set_MeshHighLight(m_isMeshHighLight);
	}
	ImGui::SameLine();
	if (true == m_pDummy_Cloth->Get_WireFrame())
	{
		if (ImGui::Button("Set Default"))
		{
			m_pDummy_Cloth->Set_WireFrame(false);
		}
	}
	else
	{
		if (ImGui::Button("Set WireFrame"))
		{
			m_pDummy_Cloth->Set_WireFrame(true);
		}
	}

	if (true == m_isPickMesh)
	{
		m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);

		_uint iVertexIndex = { 0 };
		_float3 vPickPosition;

		cloth::Cloth* pCloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
		cloth::MappedRange<PxVec4>	Particles = pCloth->getCurrentParticles();
		vector<_float> InvMasses = m_pDummy_Cloth->Get_InvMasses();

		_bool isPicking = { false };

		if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) ||
			m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING) &&
			m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING))
		{
			isPicking = true;
		}

		if (true == isPicking &&
			true == m_pDummy_Cloth->Get_VertexIndex_By_Picking(&iVertexIndex, &vPickPosition))
		{
			if (iVertexIndex > InvMasses.size() - 1)
			{
				MSG_BOX("Out of Mesh Index");
				return;
			}

			// 이미 0이면 리턴
			if (0.f == InvMasses[iVertexIndex])
				return;

			InvMasses[iVertexIndex] = 0.f;

			m_pDummy_Cloth->Remake_ClothMesh(InvMasses);
			m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			string szName;
			szName = "Vertex Index : " + to_string(iVertexIndex) +
				" | x : " + to_string(vPickPosition.x) + ", y : " + to_string(vPickPosition.y) + ", z : " + to_string(vPickPosition.z);

			CLOTHVERTEX ClothVertex;
			ClothVertex.iVertexIndex = iVertexIndex;
			ClothVertex.vVertexPosition = vPickPosition;
			ClothVertex.pVertexName = pGameInstance->Make_Char(szName.c_str());

			CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
			SphereDesc.fRadius = 0.01f;
			SphereDesc.vPosition = vPickPosition;

			ClothVertex.pCollider = static_cast<CCollider*>(m_pCollider->Clone(&SphereDesc));
			m_ClothVertices.push_back(ClothVertex);

			Safe_Release(pGameInstance);
		}
	}
}

void CCloth_Window::Current_Picked_Vertices(_float fTimeDelta)
{
	if (0 > m_ClothVertices.size())
		return;

	ImGui::SeparatorText("Picked Vertices");
	ImGui::SetNextItemWidth(m_vWindowSize.x);

	vector<_char*> VerticesName;
	for (auto& ClothVertex : m_ClothVertices)
	{
		VerticesName.push_back(ClothVertex.pVertexName);
	}

	if (ImGui::ListBox(" ", &m_iClothVertexListIndex, VerticesName.data(), _int(VerticesName.size()), 5))
	{
		// 리스트박스 최대치 예외처리
		if (0 > m_iClothVertexListIndex)
			m_iClothVertexListIndex = 0;
		else if (VerticesName.size() <= m_iClothVertexListIndex)
			m_iClothVertexListIndex = VerticesName.size() - 1;
	}

	ImGui::Separator();
}

void CCloth_Window::Make_CapsuleCollider(_float fTimeDelta)
{
	ImGui::SeparatorText("Make Capsule");
	ImGui::Checkbox("Modify Capsule", &m_isModifyCapsule);

	if (true == m_isModifyCapsule)
	{
		Modify_CapsuleCollider(fTimeDelta);
		return;
	}

	// first Sphere
	ImGui::SetNextItemWidth(200.f);
	ImGui::DragFloat3("First Sphere Pos", reinterpret_cast<_float*>(&m_SourSphere.first), 0.01f);

	ImGui::SetNextItemWidth(100.f);
	ImGui::DragFloat("First Sphere Radius", &m_SourSphere.second, 0.01f, 3.f);
	// second Sphere
	ImGui::SetNextItemWidth(200.f);
	ImGui::DragFloat3("Second Sphere Pos", reinterpret_cast<_float*>(&m_DestSphere.first), 0.01f);

	ImGui::SetNextItemWidth(100.f);
	ImGui::DragFloat("Second Sphere Radius", &m_DestSphere.second, 0.01f, 3.f);

	if (ImGui::Button("Make Capsule"))
	{
		if (nullptr == m_pDummy_Cloth ||
			nullptr == m_pCurrent_Cloth)
			return;

		CAPSULE CapsuleDesc;
		CapsuleDesc.SourSphere = m_SourSphere;
		CapsuleDesc.DestSphere = m_DestSphere;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		_char szIndex[MAX_PATH] = "";
		_itoa_s(m_Capsules.size(), szIndex, MAX_PATH, 10);
		CapsuleDesc.pCapsuleIndexName = pGameInstance->Make_Char(szIndex);
		Safe_Release(pGameInstance);

		m_Capsules.push_back(CapsuleDesc);

		vector<pair<_float3, _float>> Spheres;
		for (auto& Capsule : m_Capsules)
		{
			Spheres.push_back(Capsule.SourSphere);
			Spheres.push_back(Capsule.DestSphere);
		}

		m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);
		m_pDummy_Cloth->Set_CapsuleCollider(Spheres);

		m_pDummy_Cloth->Remake_ClothMesh(m_pDummy_Cloth->Get_InvMasses());
	}

	ImGui::Separator();
}

void CCloth_Window::Modify_CapsuleCollider(_float fTimeDelta)
{
	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Modify Mode");
	
	if (0 == m_Capsules.size())
	{
		ImGui::Text("Empty Capsule");
		return;
	}

	vector<_char*> CapsuleIndexNames;
	CapsuleIndexNames.reserve(m_Capsules.size());
	for (auto& Capsule : m_Capsules)
		CapsuleIndexNames.push_back(Capsule.pCapsuleIndexName);

	if (ImGui::ListBox("Capsules", &m_iCapsuleIndex, CapsuleIndexNames.data(), _int(CapsuleIndexNames.size()), 5))
	{
		// 리스트박스 최대치 예외처리
		if (0 > m_iCapsuleIndex)
			m_iCapsuleIndex = 0;
		else if (CapsuleIndexNames.size() <= m_iCapsuleIndex)
			m_iCapsuleIndex = CapsuleIndexNames.size() - 1;
	}

	if (0 > m_iCapsuleIndex ||
		m_Capsules.size() < m_iCapsuleIndex)
		return;

	_bool isModified = { false };

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Modify Position, Radius");
	// first Sphere
	ImGui::SetNextItemWidth(200.f);
	if (ImGui::DragFloat3("First Sphere Pos", reinterpret_cast<_float*>(&m_Capsules[m_iCapsuleIndex].SourSphere.first), 0.01f))
		isModified = true;
	
	ImGui::SetNextItemWidth(100.f);
	if (ImGui::DragFloat("First Sphere Radius", &m_Capsules[m_iCapsuleIndex].SourSphere.second, 0.01f, 3.f))
		isModified = true;
	// second Sphere
	ImGui::SetNextItemWidth(200.f);
	if (ImGui::DragFloat3("Second Sphere Pos", reinterpret_cast<_float*>(&m_Capsules[m_iCapsuleIndex].DestSphere.first), 0.01f))
		isModified = true;

	ImGui::SetNextItemWidth(100.f);
	if (ImGui::DragFloat("Second Sphere Radius", &m_Capsules[m_iCapsuleIndex].DestSphere.second, 0.01f, 3.f))
		isModified = true;

	if (ImGui::Button("Delete Capsule"))
	{
		Delete_CapsuleCollider(isModified);
	}

	if (true == isModified)
		Update_Cloth();
}

void CCloth_Window::Pick_Collider(_float fTimeDelta)
{
	if (ImGui::Checkbox("Delete Vertex In List", &m_isPickCollider))
	{
		m_isPickMesh = false;
	}
	if (0 >= m_ClothVertices.size())
		return;

	if (false == m_isPickCollider)
		return;
	else
		m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);

	_bool isPicking = { false };

	if (m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN) ||
		m_pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING) &&
		m_pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING))
	{
		isPicking = true;
	}

	if (true == isPicking)
	{
		_float4 vMouseOrigin, vMouseDirection;
		_float fDist = 1000.f;
		_float fReturnDist = fDist;
		_uint iReturnIndex = { 0 };
		_bool isIntersect = { false };

		if (FAILED(m_pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vMouseOrigin, &vMouseDirection)))
			return;

		_uint iIndex = { 0 };
		for (auto& ClothVertex : m_ClothVertices)
		{
			if (true == ClothVertex.pCollider->RayIntersects(vMouseOrigin, vMouseDirection, fDist))
			{
				if (fDist < fReturnDist)
				{
					fReturnDist = fDist;
					iReturnIndex = iIndex;

					isIntersect = true;
				}
			}

			++iIndex;
		}

		if (false == isIntersect)
			return;

		// 피킹된점 삭제
		if (iReturnIndex == m_iClothVertexListIndex)
		{
			if (0 != m_iClothVertexListIndex)
				--m_iClothVertexListIndex;
			else
				m_iClothVertexListIndex = 0;
		}

		vector<_float> InvMasses = m_pDummy_Cloth->Get_InvMasses();
		InvMasses[m_ClothVertices[iReturnIndex].iVertexIndex] = 1.f;

		m_pDummy_Cloth->Remake_ClothMesh(InvMasses);
		m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
			
		Safe_Release(m_ClothVertices[iReturnIndex].pCollider);
		m_ClothVertices.erase(m_ClothVertices.begin() + iReturnIndex);
	}
	
}

void CCloth_Window::Input_Options(_float fTimeDelta)
{
	if (false == isValid_Dummy())
		return;

	ImGui::SeparatorText("Wind");
	ImGui::SetNextItemWidth(200.f);
	m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
	PxVec3 vWindVelocity = m_pCurrent_Cloth->getWindVelocity();
	if (ImGui::InputFloat3(" ", reinterpret_cast<_float*>(&vWindVelocity), "%.1f"))
	{
		m_pDummy_Cloth->Set_WindVelocity(_float3(vWindVelocity.x, vWindVelocity.y, vWindVelocity.z));
		//m_pCurrent_Cloth->setWindVelocity(vWindVelocity);
	}
	
	ImGui::Separator();

	
	if (ImGui::SliderFloat("Inv Mass", &m_fInvMass, 0.01f, 1.f, "%.2f"))
	{
		vector<_float> InvMasses = m_pDummy_Cloth->Get_InvMasses();
		fill(InvMasses.begin(), InvMasses.end(), m_fInvMass);

		for (auto& ClothVertex : m_ClothVertices)
		{
			InvMasses[ClothVertex.iVertexIndex] = 0.f;
		}

		m_pDummy_Cloth->Remake_ClothMesh(InvMasses);
	}
}

void CCloth_Window::Delete_CapsuleCollider(_bool& _isModified)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_uint iIndex = { 0 };
	for (auto iter = m_Capsules.begin(); iter != m_Capsules.end();)
	{
		if (iIndex == m_iCapsuleIndex)
		{
			pGameInstance->Delete_Char((*iter).pCapsuleIndexName);
			m_Capsules.erase(iter);

			_isModified = true;
			break;
		}

		++iIndex;
		++iter;
	}

	Safe_Release(pGameInstance);
}

_bool CCloth_Window::isValid_Dummy()
{
	if (nullptr == m_pDummy_Cloth)
		return false;
	if (nullptr == m_pDummy_Cloth->Get_CustomModel())
		return false;
	if (nullptr == m_pCurrent_Cloth)
		return false;

	return true;
}

void CCloth_Window::Make_Data()
{
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));	
	if (ImGui::Button("Save Cloth Data"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Save Cloth Data", "Save File", ".cloth", "../../Resources/GameData/ClothData/");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	if (ImGuiFileDialog::Instance()->Display("Save Cloth Data"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			_tchar szPath[MAX_PATH] = TEXT("");
			CharToWChar(ImGuiFileDialog::Instance()->GetFilePathName().c_str(), szPath);
			Save_Data(szPath);
		}

		ImGuiFileDialog::Instance()->Close();
	}

	ImGui::SameLine();
	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 1.0f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.7f, 1.0f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.8f, 1.0f, 0.8f));
	if (ImGui::Button("Load Cloth Data"))
	{
		ImGuiFileDialog::Instance()->OpenDialog("Load Cloth Data", "Load File", ".cloth", "../../Resources/GameData/ClothData/");
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	if (ImGuiFileDialog::Instance()->Display("Load Cloth Data"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			_tchar szPath[MAX_PATH] = TEXT("");
			CharToWChar(ImGuiFileDialog::Instance()->GetFilePathName().c_str(), szPath);
			Load_Data(szPath);
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void CCloth_Window::Save_Data(const _tchar* szSaveDataPath)
{
	HANDLE hFile = CreateFile(szSaveDataPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}

	_ulong	dwByte = 0;

	WriteFile(hFile, &m_iMeshIndex, sizeof(_uint), &dwByte, nullptr);

	vector<_float> InvMasses = m_pDummy_Cloth->Get_InvMasses();
	_uint iSize = _uint(InvMasses.size());
	// Vertex Num
	WriteFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& InvMass : InvMasses)
	{
		WriteFile(hFile, &InvMass, sizeof(_float), &dwByte, nullptr);
	}

	_uint iCapsuleSize = m_Capsules.size();
	WriteFile(hFile, &iCapsuleSize, sizeof(_uint), &dwByte, nullptr);

	for (auto& CapsuleDesc : m_Capsules)
	{
		WriteFile(hFile, &CapsuleDesc.SourSphere.first, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &CapsuleDesc.SourSphere.second, sizeof(_float), &dwByte, nullptr);
		WriteFile(hFile, &CapsuleDesc.DestSphere.first, sizeof(_float3), &dwByte, nullptr);
		WriteFile(hFile, &CapsuleDesc.DestSphere.second, sizeof(_float), &dwByte, nullptr);
	}

	MSG_BOX("File Save Success");

	CloseHandle(hFile);
}

void CCloth_Window::Load_Data(const _tchar* szLoadDataPath)
{
	if (nullptr == m_pCollider ||
		nullptr == m_pDummy_Cloth)
	{
		MSG_BOX("Dummy is NULL");
		return;
	}

	HANDLE hFile = CreateFile(szLoadDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return;
	}

	_ulong	dwByte = 0;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	ReadFile(hFile, &m_iMeshIndex, sizeof(_uint), &dwByte, nullptr);
	m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);

	vector<_float> InvMasses;
	_uint iSize = { 0 };
	// Vertex Num
	ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

	InvMasses.reserve(iSize);

	vector<_float3> VertexPositions = m_pDummy_Cloth->Get_VertexPositions();

	for (_uint i = 0; i < iSize; ++i)
	{
		_float fInvMass = 1.f;
		ReadFile(hFile, &fInvMass, sizeof(_float), &dwByte, nullptr);
		InvMasses.push_back(fInvMass);

		if (0.f == fInvMass)
		{
			string szName;
			szName = "Vertex Index : " + to_string(i) +
				" | x : " + to_string(VertexPositions[i].x) + ", y : " + to_string(VertexPositions[i].y) + ", z : " + to_string(VertexPositions[i].z);

			CLOTHVERTEX ClothVertex;
			ClothVertex.iVertexIndex = i;
			ClothVertex.vVertexPosition = VertexPositions[i];
			ClothVertex.pVertexName = pGameInstance->Make_Char(szName.c_str());

			CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
			SphereDesc.fRadius = 0.01f;
			SphereDesc.vPosition = VertexPositions[i];

			ClothVertex.pCollider = static_cast<CCollider*>(m_pCollider->Clone(&SphereDesc));
			m_ClothVertices.push_back(ClothVertex);
		}

	}

	_uint iCapsuleSize = { 0 };
	ReadFile(hFile, &iCapsuleSize, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iCapsuleSize; ++i)
	{
		CAPSULE CapsuleDesc;
		ZEROMEM(&CapsuleDesc);

		ReadFile(hFile, &CapsuleDesc.SourSphere.first, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &CapsuleDesc.SourSphere.second, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &CapsuleDesc.DestSphere.first, sizeof(_float3), &dwByte, nullptr);
		ReadFile(hFile, &CapsuleDesc.DestSphere.second, sizeof(_float), &dwByte, nullptr);

		_char szIndex[MAX_PATH] = "";
		_itoa_s(m_Capsules.size(), szIndex, MAX_PATH, 10);
		CapsuleDesc.pCapsuleIndexName = pGameInstance->Make_Char(szIndex);

		m_Capsules.push_back(CapsuleDesc);
	}

	Safe_Release(pGameInstance);

	// 디버그 렌더 갱신


	vector<pair<_float3, _float>> Spheres;
	for (auto& Capsule : m_Capsules)
	{
		Spheres.push_back(Capsule.SourSphere);
		Spheres.push_back(Capsule.DestSphere);
	}

	m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);
	m_pDummy_Cloth->Set_CapsuleCollider(Spheres);

	m_pDummy_Cloth->Remake_ClothMesh(InvMasses);

	MSG_BOX("File Load Success");

	CloseHandle(hFile);
}

void CCloth_Window::Update_Cloth()
{
	vector<pair<_float3, _float>> Spheres;
	for (auto& Capsule : m_Capsules)
	{
		Spheres.push_back(Capsule.SourSphere);
		Spheres.push_back(Capsule.DestSphere);
	}

	m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);
	m_pDummy_Cloth->Set_CapsuleCollider(Spheres);

	m_pDummy_Cloth->Remake_ClothMesh(m_pDummy_Cloth->Get_InvMasses());
	m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
}

void CCloth_Window::Create_MeshParts_Prototype(string strFilePathName)
{
	_char fileName[MAX_PATH] = "";
	_splitpath_s(strFilePathName.c_str(), nullptr, 0, nullptr, 0, fileName, MAX_PATH, nullptr, 0);

	_tchar wszfilePath[MAX_PATH] = {};
	CharToWChar(strFilePathName.c_str(), wszfilePath);
	CharToWChar(fileName, m_wszDummyName);
	_tchar wszModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_");
	_char szModelTag[MAX_PATH] = {};
	lstrcat(wszModelTag, m_wszDummyName);
	WCharToChar(wszModelTag, szModelTag);

	_tchar DefaultTag[MAX_PATH] = TEXT("MeshParts_Default");
	lstrcat(DefaultTag, Generate_HashtagW().c_str());
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, wszModelTag,
		CMeshParts::Create(m_pDevice, m_pContext, wszfilePath, DefaultTag))))
	{
		MSG_BOX("Failed to Create CCustomModel");
	}

	m_pDummy_Cloth->Set_Model_Component(m_eMeshType, wszModelTag);
}

HRESULT CCloth_Window::Render_Collider()
{
#ifdef _DEBUG
	_uint iIndex = { 0 };

	for (auto& ClothVertex : m_ClothVertices)
	{
		if (iIndex == m_iClothVertexListIndex)
		{
			ClothVertex.pCollider->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
		}
		else
		{
			ClothVertex.pCollider->Set_Color(_float4(0.f, 1.f, 0.f, 1.f));
		}

		if (nullptr != m_pRenderer)
		{
			m_pRenderer->Add_DebugGroup(ClothVertex.pCollider);
		}

		++iIndex;
	}
#endif // _DEBUG
	return S_OK;
}

CCloth_Window* CCloth_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	CCloth_Window* pInstance = New CCloth_Window(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vWindowPos, _vWindowSize)))
	{
		MSG_BOX("Failed Create CCloth_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCloth_Window::Free(void)
{
	__super::Free();

	for (auto& ClothVertex : m_ClothVertices)
	{
		Safe_Release(ClothVertex.pCollider);
	}
	m_ClothVertices.clear();

	Safe_Release(m_pDummy_Cloth);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
}