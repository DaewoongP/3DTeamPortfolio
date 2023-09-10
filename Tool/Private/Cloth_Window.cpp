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

	Radio_Select_MeshType(_fTimeDelta);

	Open_Model(_fTimeDelta);

	Input_Mesh_Index(_fTimeDelta);

	Pick_Mesh(_fTimeDelta);

	Current_Picked_Vertices(_fTimeDelta);

	Pick_Collider(_fTimeDelta);

	ImGui::End();
	
	RECT rc;
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(ImVec2(_float(rc.left), _float(rc.bottom)));
	ImGui::SetNextWindowSize(ImVec2(_float(g_iWinSizeX), 200.f));

	ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	Input_Wind(_fTimeDelta);
	
	ImGui::End();
}

HRESULT CCloth_Window::Render()
{
	CBounding_Sphere::BOUNDINGSPHEREDESC SphereDesc;
	SphereDesc.fRadius = 0.01f;
	_uint iIndex = { 0 };
	for (auto& PickPair : m_PickVetices)
	{
		SphereDesc.vPosition = PickPair.second;
		m_pCollider->Set_BoundingDesc(&SphereDesc);

#ifdef _DEBUG
		if (iIndex == m_iPickVertexIndex)
		{
			m_pCollider->Set_Color(_float4(0.f, 0.f, 1.f, 1.f));
		}
		else
		{
			m_pCollider->Set_Color(_float4(0.f, 1.f, 0.f, 1.f));
		}
#endif // _DEBUG

		m_pCollider->Render();
		++iIndex;
	}
	
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
	ImGui::RadioButton("Upper Body", (_int*)(&m_eMeshType), _int(CCustomModel::UPPERBODY));
	
	ImGui::RadioButton("Under Body", (_int*)(&m_eMeshType), _int(CCustomModel::UNDERBODY));
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
	ImGui::Checkbox("Pick Vertex", &m_isPickMesh);
	ImGui::SameLine();
	if (ImGui::Button("Reset Vertex"))
	{
		m_pDummy_Cloth->Reset_Position();
	}

	if (true == m_isPickMesh)
	{
		m_pDummy_Cloth->Set_MeshIndex(m_iMeshIndex);
		m_pDummy_Cloth->Set_Testing(false);

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

			InvMasses[iVertexIndex] = 0.f;

			m_pDummy_Cloth->Remake_ClothMesh(InvMasses);
			m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			string szName;
			szName = "Vertex Index : " + to_string(iVertexIndex) + 
				" | x : " + to_string(vPickPosition.x) + ", y : " + to_string(vPickPosition.y) + ", z : " + to_string(vPickPosition.z);

			m_PickVetices.push_back({ iVertexIndex, vPickPosition });
			m_PickVerticesName.push_back(pGameInstance->Make_Char(szName.c_str()));

			Safe_Release(pGameInstance);
		}
	}
	else
	{
		m_pDummy_Cloth->Set_Testing(true);
	}
}

void CCloth_Window::Current_Picked_Vertices(_float fTimeDelta)
{
	if (0 > m_PickVetices.size() ||
		0 > m_PickVerticesName.size())
		return;

	ImGui::SeparatorText("Picked Vertices");
	ImGui::SetNextItemWidth(m_vWindowSize.x);
	if (ImGui::ListBox(" ", &m_iPickVertexIndex, m_PickVerticesName.data(), _int(m_PickVerticesName.size()), 20))
	{
		// 리스트박스 최대치 예외처리
		if (0 > m_iPickVertexIndex)
			m_iPickVertexIndex = 0;
		else if (m_PickVerticesName.size() <= m_iPickVertexIndex)
			m_iPickVertexIndex = m_PickVerticesName.size() - 1;
	}

	ImGui::Separator();
}

void CCloth_Window::Pick_Collider(_float fTimeDelta)
{
	for (auto& VertexPair : m_PickVetices)
	{

	}
}

void CCloth_Window::Input_Wind(_float fTimeDelta)
{
	if (false == isValid_Dummy())
		return;

	ImGui::SeparatorText("Wind");
	ImGui::SetNextItemWidth(200.f);
	m_pCurrent_Cloth = m_pDummy_Cloth->Get_CurrentMesh_Cloth();
	PxVec3 vWindVelocity = m_pCurrent_Cloth->getWindVelocity();
	if (ImGui::InputFloat3("", reinterpret_cast<_float*>(&vWindVelocity), "%.1f"))
	{
		m_pCurrent_Cloth->setWindVelocity(vWindVelocity);
	}

	ImGui::Separator();
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

	Safe_Release(m_pDummy_Cloth);
	Safe_Release(m_pCamera_Free);
	Safe_Release(m_pCollider);
}