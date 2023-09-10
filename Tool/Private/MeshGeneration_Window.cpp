#include "MeshGeneration_Window.h"
#include "ImGuiFileDialog.h"
#include "GameInstance.h"
#include "RectDebug.h"
#include "PhysXRender.h"
#include "Dummy_Generation.h"
#include "Bounding_Sphere.h"

CMeshGeneration_Window::CMeshGeneration_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CMeshGeneration_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy_Generation"), TEXT("Layer_Tool"), TEXT("Dummy_Generation"), nullptr)))
	{
		MSG_BOX("Failed to Created Dummy Clone");
	}
	m_pDummyObject = dynamic_cast<CDummy_Generation*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Dummy_Generation")));
	
	if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_RectDebug"), TEXT("Layer_Tool"), TEXT("RectDebug"), nullptr)))
	{
		MSG_BOX("Failed to Created RectDebug Clone");
	}
	m_pRectDebug = dynamic_cast<CRectDebug*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("RectDebug")));

	//if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_PhysxRenderer"), TEXT("Layer_Tool"), TEXT("PhysxRenderer"), nullptr)))
	//{
	//	MSG_BOX("Failed to Created PhysxRenderer Clone");
	//}
	//m_pPhysXRender = dynamic_cast<CPhysXRender*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("PhysxRenderer")));
	Safe_Release(pGameInstance);

	return S_OK;
}

void CMeshGeneration_Window::OpenFile_Button()
{
	// open Dialog Simple
	if (ImGui::Button("Open File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseModel", "Choose File", ".dat", "../../Resources/Models/");

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
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, wszfilePath, PivotMatrix))))
			{
				MSG_BOX("Failed to Create Model");
			}
			Safe_Release(pGameInstance);
			m_iMaxModelIndex++;
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void CMeshGeneration_Window::Select_Model()
{
	ImGui::Text("AnimModelList");
	ImGui::ListBox("##AnimModelListBox", &m_iModelIndex, VectorGetter, static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);
}

void CMeshGeneration_Window::AddModel_Button()
{
	if (ImGui::Button("AddModelToDummy"))
	{
		if (m_pDummyObject == nullptr)
		{
			MSG_BOX("Failed to Add Model");
			return;
		}

		if (m_pDummyObject->Find_Component(TEXT("Com_Model")) == nullptr)
		{
			m_pDummyObject->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
			lstrcpy(m_wszCurrentDummyModelTag, m_vecModelList_t[m_iModelIndex].c_str());
			m_pDummyObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		}
		else 
		{
			if (FAILED(m_pDummyObject->Change_Model_Component(m_vecModelList_t[m_iModelIndex].c_str())))
			{
				MSG_BOX("Failed to Change Model");
				return;
			}
			lstrcpy(m_wszCurrentDummyModelTag, m_vecModelList_t[m_iModelIndex].c_str());
		}
		
	}
}

void CMeshGeneration_Window::Setting_VerticesPoints()
{
	if (ImGui::InputInt2("Range##point_range", m_vPoint_Range))
	{
		//사각형 범위 확장시켜줘야함.
		m_pRectDebug->Set_Scale(_int2(m_vPoint_Range[0], m_vPoint_Range[1]));
	}
	ImGui::InputInt2("Count##point_count", m_vPoint_Count);
}

void CMeshGeneration_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Generation_Window", nullptr, m_WindowFlag);	
	OpenFile_Button();
	Select_Model();
	AddModel_Button();
	Setting_VerticesPoints();
	ImGui::End();
}

HRESULT CMeshGeneration_Window::Render()
{
	return S_OK;
}

CMeshGeneration_Window* CMeshGeneration_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CMeshGeneration_Window* pInstance = New CMeshGeneration_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CMeshGeneration_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CMeshGeneration_Window::Free(void)
{
	__super::Free();
}
