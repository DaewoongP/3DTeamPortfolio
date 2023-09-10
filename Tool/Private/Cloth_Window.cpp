#include "..\Public\Cloth_Window.h"
#include "Dummy_Cloth.h"
#include "Camera_Free.h"
#include "Bounding_Sphere.h"
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
		CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/test/test.dat")))))
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

	if (true == m_isPickMesh ||
		true == m_isPickCollider)
	{
		m_pDummy_Cloth->Set_Testing(false);
	}
	else
		m_pDummy_Cloth->Set_Testing(true);

	Radio_Select_MeshType(_fTimeDelta);

	Open_Model(_fTimeDelta);

	Input_Mesh_Index(_fTimeDelta);

	Pick_Mesh(_fTimeDelta);

	Current_Picked_Vertices(_fTimeDelta);

	ImGui::End();


	Input_Options(_fTimeDelta);	

}

HRESULT CCloth_Window::Render()
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

void CCloth_Window::Radio_Select_MeshType(_float fTimeDelta)
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

void CCloth_Window::Open_Model(_float fTimeDelta)
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
	
	// display
	if (ImGuiFileDialog::Instance()->Display("Open Cloth Model"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			
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
			m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);
			m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();

			m_pCamera_Free->Get_Transform()->Set_Position(_float3(0.f, 1.3f, -1.3f));
			m_pCamera_Free->Get_Transform()->LookAt(_float3(0.f, 1.f, 0.f));
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

	if (ImGui::ListBox(" ", &m_iClothVertexListIndex, VerticesName.data(), _int(VerticesName.size()), 20))
	{
		// 리스트박스 최대치 예외처리
		if (0 > m_iClothVertexListIndex)
			m_iClothVertexListIndex = 0;
		else if (VerticesName.size() <= m_iClothVertexListIndex)
			m_iClothVertexListIndex = VerticesName.size() - 1;
	}

	ImGui::Separator();
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

	ImGui::Begin("Options");

	ImGui::SeparatorText("Wind");
	ImGui::SetNextItemWidth(200.f);
	m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
	PxVec3 vWindVelocity = m_pCurrent_Cloth->getWindVelocity();
	if (ImGui::InputFloat3("", reinterpret_cast<_float*>(&vWindVelocity), "%.1f"))
	{
		m_pCurrent_Cloth->setWindVelocity(vWindVelocity);
	}
	
	ImGui::Separator();

	ImGui::End();
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