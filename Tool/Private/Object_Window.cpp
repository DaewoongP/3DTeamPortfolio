#include "..\Public\Object_Window.h"
#include "ImGuiFileDialog.h"

#include "MapDummy.h"
#include "MapObject.h"
#include "MapObject_Ins.h"
#include "Terrain.h"

#include "HelpMaker.h"

CObject_Window::CObject_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
	ZeroMemory(&m_vDummyMatrix, sizeof m_vDummyMatrix);

	// ������ �� 1�� �ʱ�ȭ
	for (size_t i = 0; i < 3; i++)
		m_vDummyMatrix[DUMMY_SCALE][i] = 1.f;
}

HRESULT CObject_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	// 0�� Non_Anim�̴�.
	if (FAILED(Save_Model_Path(0, TEXT("../../Resources/Models/MapObject/NonAnims/"))))
	{
		MSG_BOX("Failed to Save_Model_Path function");
		return S_OK;
	}

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CObject_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	ImGui::Begin("Object", nullptr, m_WindowFlag);

	// Create Dummy ��ư
	if (ImGui::Button("Create Dummy"))
	{
		if (FAILED(Create_Dummy()))
		{
			MSG_BOX("Failed to Create Dummy");
		}
	}

	ImGui::SameLine();

	// Delete Dummy ��ư
	if (ImGui::Button("Delete Dummy"))
	{
		if (FAILED(Delete_Dummy()))
		{
			MSG_BOX("Failed to Delete Dummy");
		}
	}

	// Picking â On / Off
	ImGui::Checkbox("Picking", &m_isCheckPicking);
	if(true == m_isCheckPicking)
	{
		Picking_Menu();
	}

	ImGui::Separator();

	// Model ���� â On / Off
	ImGui::Checkbox("Model Select", &m_isSelectModel);
	if (true == m_isSelectModel)
	{
		Select_Model();
	}

	ImGui::Separator();

	// ���� ��ġ�Ǿ� �ִ� �� ������Ʈ â On / Off
	ImGui::Checkbox("Current MapObject", &m_isCurrentMapObject);
	if (true == m_isCurrentMapObject)
	{
		// ��â���� ���� ������ �������� �ּ�ó����
		//ImGui::Begin("Current MapObject", nullptr);
		Current_MapObject();
		//ImGui::End();
	}

	ImGui::Separator();

	// Save Load ���� â On / Off
	ImGui::Checkbox("Save Load", &m_isSaveLoad);
	if (true == m_isSaveLoad)
	{
		Save_Load_Menu();
	}

	ImGui::Separator();

	// �ν��Ͻ� Save Load ���� â On / Off
	ImGui::Checkbox("Instance Save Load", &m_isInsSaveLoad);
	if (true == m_isInsSaveLoad)
	{
		Ins_Save_Load_Menu();
	}

	ImGui::Separator();

	// �� �귯�� �޴� â On / Off
	ImGui::Checkbox("Map Brushing", &m_isBrushingMap);
	if (true == m_isBrushingMap)
	{
		Map_Brushing_Menu();
	}

	ImGui::Separator();

	ImGui::End();
}

HRESULT CObject_Window::Render()
{
	return S_OK;
}

void CObject_Window::Picking_Menu()
{
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return;
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

	ImGui::Text("Choice Install Method");
	ImGui::RadioButton("1Click One_Install", &m_iInstallMethod, 0);
	ImGui::RadioButton("1Pressing Continuius_Install", &m_iInstallMethod, 1);
	ImGui::RadioButton("1Click One_Install for Instance", &m_iInstallMethod, 2);

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
	if (nullptr != m_pDummy)
	{
		_float3 vScale;
		memcpy(&vScale, m_vDummyMatrix[DUMMY_SCALE], sizeof _float3);

		_float3 vRotation;
		memcpy(&vRotation, m_vDummyMatrix[DUMMY_ROT], sizeof _float3);

		vRotation.x = XMConvertToRadians(vRotation.x);
		vRotation.y = XMConvertToRadians(vRotation.y);
		vRotation.z = XMConvertToRadians(vRotation.z);

		// shiftŰ�� ������ ������ ���ڿ� �� �°� ��ġ�� �ݿø���
		BEGININSTANCE; if (true == pGameInstance->Get_DIKeyState(DIK_LSHIFT, CInput_Device::KEY_PRESSING))
		{
			vPos.x = round(vPos.x);
			vPos.y = round(vPos.y);
			vPos.z = round(vPos.z);

			m_pDummy->Set_Pos(vPos);
		} ENDINSTANCE;

		_float3 vTranslation = 
		{ vPos.x + m_vDummyMatrix[DUMMY_TRANS][0], vPos.y + m_vDummyMatrix[DUMMY_TRANS][1], vPos.z + m_vDummyMatrix[DUMMY_TRANS][2] };

		m_pDummy->Get_Transform()->Set_Scale(vScale);
		m_pDummy->Get_Transform()->Set_Quaternion(m_pDummy->Get_Transform()->Get_QuaternionVector_RollPitchYaw(vRotation));
		m_pDummy->Get_Transform()->Set_Position(vTranslation);
	}

	// Object Install ���� â On / Off
	ImGui::Checkbox("Object Install", &m_isInstallObject);
	if (true == m_isInstallObject && nullptr != m_pDummy)
	{
		switch (m_iInstallMethod)
		{
		case ONE_METHOD: // Ŭ�� �� �� �� ��ġ
			Install_Object(m_pDummy->Get_Transform()->Get_Position());
			break;

		case CONTINUOUS_METHOD: // ������ ������ �� ��ġ
			Install_Continuous_Object(m_pDummy->Get_Transform()->Get_Position());
			break;

		case MULTI_METHOD: // �ѹ��� ������ ��ġ(�ν��Ͻ�)
			Install_Multi_Object(m_pDummy->Get_Transform()->Get_Position());
			break;
		}

		// ������Ʈ ���� ���� �޴�
		Delete_Object_Menu();
	}
}

void CObject_Window::Install_Object(_float3 vPos)
{
	ImGui::Text("Press H to Install");

	// ���� �ȿ� ���� ��� HŰ�� ���� ��ġ
	BEGININSTANCE; if (true == pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x)
	{
		// �� ������Ʈ�� ��ȣ �ٿ���
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

		// ��ȣ�� ���� �±׷� MapObject ���
		if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"), 
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return;
		}

		// �������� ��ġ�� �� ������Ʈ �ּ� ������
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, 
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iMapObjectIndex); // ������ ���� ���� �־���

		// ����� ���Ϳ� �־��ش�.
		SAVEOBJECTDESC SaveDesc;

		SaveDesc.matTransform = vWorldMatrix;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
		SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

		m_vecSaveObject.push_back(SaveDesc);

		++m_iMapObjectIndex;

	} ENDINSTANCE;
}

void CObject_Window::Install_Continuous_Object(_float3 vPos)
{
	BEGININSTANCE; m_fTimeAcc += pGameInstance->Get_World_Tick();
	
	// ���콺�� �� ������ ������ ���� ��ġ
	if (true == pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x && 
		0.1f < m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;

		// �� ������Ʈ�� ��ȣ �ٿ���
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();

		// ��ȣ�� ���� �±׷� MapObject ���
		if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return;
		}

		// �������� ��ġ�� �� ������Ʈ �ּ� ������
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iMapObjectIndex); // ������ ���� ���� �־���

		// ����� ���Ϳ� �־��ش�.
		SAVEOBJECTDESC SaveDesc;

		SaveDesc.matTransform = vWorldMatrix;
		lstrcpy(SaveDesc.wszTag, m_vecModelList_t.at(m_iModelIndex));
		SaveDesc.iTagLen = lstrlen(SaveDesc.wszTag) * 2;

		m_vecSaveObject.push_back(SaveDesc);

		++m_iMapObjectIndex;

	} ENDINSTANCE;
}

void CObject_Window::Install_Multi_Object(_float3 vPos)
{
	ImGui::Text("Press H to Install");

	// ���� �ȿ� ���� ��� HŰ�� ���� ��ġ
	BEGININSTANCE; if (true == pGameInstance->Get_DIKeyState(DIK_H, CInput_Device::KEY_DOWN) &&
		-1.f != vPos.x)
	{
		// �� ������Ʈ�� ��ȣ �ٿ���
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		_float4x4 vWorldMatrix = m_pDummy->Get_Transform()->Get_WorldMatrix();
		// ��ȣ�� ���� �±׷� MapObject ���
		if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &vWorldMatrix)))
		{
			MSG_BOX("Failed to Install MapObject_Ins");
			ENDINSTANCE;
			return;
		}

		// �������� ��ġ�� �� ������Ʈ �ּ� ������
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecModelList_t.at(m_iModelIndex));
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iMapObjectIndex); // ������ ���� ���� �־���

		// �ν��Ͻ� ����� ���Ϳ� �־��ش�.
		m_vecSaveInsObjectWorld.push_back(vWorldMatrix);

		++m_iMapObjectIndex;
		++m_iInsObjectCnt;

	} ENDINSTANCE;
}

void CObject_Window::Select_Model()
{
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return;
	}

	// ���� ���õ� ���� �������� ǥ��
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Current Model");
	ImGui::TextColored(ImVec4(1, 0, 0, 1), ":");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 0, 0, 1), m_strCurrentModel.c_str());

	// �� ����
	ImGui::ListBox("ModelList", &m_iModelIndex, VectorGetter, 
		static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);

	// ���õ� ���� ����
	if (ImGui::Button("SelectModel"))
	{
		if (FAILED(m_pDummy->Change_Model_Component(m_vecModelList_t.at(m_iModelIndex))))
		{
			MSG_BOX("Failed to Change Model");
			return;
		}

		// ���� ���õ� �� �̸� ����
		_char szName[MAX_PATH];
		WCharToChar(m_vecModelList_t.at(m_iModelIndex), szName);
		m_strCurrentModel.clear();
		m_strCurrentModel.append(szName);
	}
}

void CObject_Window::Current_MapObject()
{
	// ��ġ�Ǿ� �ִ� ������Ʈ ����Ʈ
	ImGui::ListBox("Map Object List", &m_iTagIndex, VectorGetter,
		static_cast<void*>(&m_vecObjectTag_s), (_int)m_vecObjectTag_s.size(), 20);

	// �޽� ��ŷ �޴� On / Off
	ImGui::Checkbox("Object Picking", &m_isPickingObject);
	if (true == m_isPickingObject)
	{
		Mesh_Picking_Menu();
	}
}

void CObject_Window::Save_Load_Menu()
{
	// open Dialog Simple
	// Load ��� ����
	if (ImGui::Button("Open Load File Dialog"))
		ImGuiFileDialog::Instance()->OpenDialog("ChooseData", "Choose File", ".ddd",
			"../../Resources/GameData/MapData/");

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseData"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// action
			// �ּ� ����
			std::string strFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			
			wstring ws;
			ws.assign(strFilePathName.begin(), strFilePathName.end());
			lstrcpy(m_wszMapLoadDataPath, ws.c_str());
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::InputTextWithHint("SaveData Path", "enter SaveDataName.ddd", szPath, IM_ARRAYSIZE(szPath)))
	{
		m_strPath = szPath;
	}

	// ���̺� ��ư ó��
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_MapObject(m_strPath)))
			MSG_BOX("Failed to Save MapObject on Menu");
	}

	ImGui::SameLine();

	// �ε� ��ư ó��
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_MapObject(m_wszMapLoadDataPath)))
			MSG_BOX("Failed to Load MapObject on Menu");
	}
}

void CObject_Window::Ins_Save_Load_Menu()
{
	// ���̺� ��ư ó��
	if (ImGui::Button("Save"))
	{
		if (FAILED(Save_MapObject_Ins()))
			MSG_BOX("Failed to Save MapObject_Ins on Menu");
	}

	ImGui::SameLine();

	// �ε� ��ư ó��
	if (ImGui::Button("Load"))
	{
		if (FAILED(Load_MapObject_Ins()))
			MSG_BOX("Failed to Load MapObject_Ins on Menu");
	}
}

void CObject_Window::Delete_Object_Menu()
{
	// �������� ��ġ�� ������Ʈ�� ����
	// ��, ������Ʈ�� �ϳ��� ��ġ�� �Ǿ��־�� �Ѵ�.
	if (ImGui::Button("Undo"))
	{
		if (0 != m_vecObjectTag_s.size())
		{
			BEGININSTANCE;
			if (FAILED(pGameInstance->Delete_Component(LEVEL_TOOL, 
				TEXT("Layer_MapObject"), m_vecMapObjectTag.back().c_str())))
			{
				MSG_BOX("Failed to delete last MapObject");
			} ENDINSTANCE;

			if (0 < m_iTagIndex)
			{
				--m_iTagIndex;
			}

			if (0 < m_iMapObjectIndex)
			{
				--m_iMapObjectIndex;
			}

			m_vecMapObjectTag.pop_back();
			m_vecObjectTag_s.pop_back();
			m_vecSaveObject.pop_back();
		}
	}

	ImGui::SameLine();

	// ��ġ�� ��ü ������Ʈ ���� ��� Ȱ��ȭ
	if (ImGui::Button("Delete All"))
	{
		m_isDeleteObject = !m_isDeleteObject;
	}

	// ���� ��ư�� ���� ���â�� Ȱ��ȭ�Ѵ�.
	// ��, ������Ʈ�� �ϳ��� ��ġ�� �Ǿ��־�� �Ѵ�.
	if (true == m_isDeleteObject && 0 != m_vecObjectTag_s.size())
	{
		ImGui::Text("Are you sure?");

		ImGui::SameLine();

		 // yes�� ������ MapObject ���� ����
		if (ImGui::Button("Yes"))
		{
			BEGININSTANCE;
			if (FAILED(pGameInstance->Clear_Layer(LEVEL_TOOL, TEXT("Layer_MapObject"))))
			{
				MSG_BOX("Failed to clear MapObject");
			} ENDINSTANCE;

			// �� �ʱ�ȭ
			m_iMapObjectIndex = 0;

			m_vecSaveObject.clear();
			m_vecObjectTag_s.clear();
			m_vecMapObjectTag.clear();

			m_isDeleteObject = false;
		}

		ImGui::SameLine();

		// no�� ������ �ƹ��ϵ� �Ͼ�� ����
		if (ImGui::Button("No"))
		{
			m_isDeleteObject = false;
		}
	}
}

void CObject_Window::Mesh_Picking_Menu()
{
#ifdef _DEBUG
	BEGININSTANCE; if (true == pGameInstance->Get_DIKeyState(DIK_O, CInput_Device::KEY_DOWN))
	{
		//Target_Picking�� ID3D11Texture2D�� ������
		ID3D11Texture2D* pTexture = pGameInstance->Find_RenderTarget(TEXT("Target_Picking"))->Get_Texture2D();

		// �ؽ�ó�� �ʺ�� ����
		D3D11_TEXTURE2D_DESC  TextureDesc;
		pTexture->GetDesc(&TextureDesc);

		_uint iWidth = TextureDesc.Width;
		_uint iHeight = TextureDesc.Height;

		// ���콺 ��ġ
		D3D11_VIEWPORT ViewPort;
		_uint iNumViewPorts = 1;

		ZEROMEM(&ViewPort);
		if (nullptr == m_pContext)
			return;
		m_pContext->RSGetViewports(&iNumViewPorts, &ViewPort);
		
		POINT	pt{};
		GetCursorPos(&pt);
		ScreenToClient(g_hWnd, &pt);

		D3D11_BOX findDesc;
		ZEROMEM(&findDesc);

		// �� ����ü�� ������ ���� �� �ϳ��� �ȼ��� �����´�
		findDesc.left = pt.x;
		findDesc.right = pt.x + 1;
		findDesc.top = pt.y;
		findDesc.bottom = pt.y + 1;
		findDesc.front = 0;
		findDesc.back = 1;

		// Usage ���� ����, �б� �����̴�.
		ID3D11Texture2D* pCopyTexture2D = { nullptr };
		D3D11_TEXTURE2D_DESC	TextureDescCopy;
		ZEROMEMSIZE(&TextureDescCopy, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDescCopy.Width = 1;
		TextureDescCopy.Height = 1;
		TextureDescCopy.MipLevels = 1;
		TextureDescCopy.ArraySize = 1;
		TextureDescCopy.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

		TextureDescCopy.SampleDesc.Quality = 0;
		TextureDescCopy.SampleDesc.Count = 1;

		TextureDescCopy.Usage = D3D11_USAGE_STAGING;
		TextureDescCopy.BindFlags = 0;
		TextureDescCopy.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		TextureDescCopy.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDescCopy, nullptr, &pCopyTexture2D)))
			return;

		// ���� ���� 1ĭ¥�� �ؽ�ó�� ������ ���� Ÿ�� �ؽ�ó���� ���ϴ� 1ĭ�� �־��ش�.
		m_pContext->CopySubresourceRegion(pCopyTexture2D, 0, 0, 0, 0, pTexture, 0, &findDesc);

		D3D11_MAPPED_SUBRESOURCE MappedDesc;
		ZEROMEM(&MappedDesc);

		if (FAILED(m_pContext->Map(pCopyTexture2D, 0, D3D11_MAP_READ, 0, &MappedDesc)))
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Failed to Map Picking Texture");
			return;
		}

		// �ش� Pixel Copy�� �߸� �Ǿ��� ���..
		if (MappedDesc.pData == nullptr)
		{
			m_pContext->Unmap(pCopyTexture2D, 0);
			MSG_BOX("Copy Data is nullptr");
			return;
		}

		_uint pickID = { 0 };
		pickID = ((_uint*)MappedDesc.pData)[0];

		pickID -= 4278190080;

		string s = ("GameObject_MapObject_");
		s += std::to_string(pickID);

		// ��� �ε����� ã�� ����
		m_iTagIndex = 0;
		for (auto& iter : m_vecObjectTag_s)
		{
			if (iter == s)
				break;

			++m_iTagIndex;
		}

		m_pContext->Unmap(pCopyTexture2D, 0);

		Safe_Release(pCopyTexture2D);
	}ENDINSTANCE;

	// �޽� ��ŷ�� ������Ʈ ���� ��� ���� �޴�
	ImGui::Checkbox("Change MapObject's Matrix", &m_isChangeObject);
	if (true == m_isChangeObject)
	{
		wstring ws = TEXT("GameObject_MapObject_");
		ws += std::to_wstring(m_iTagIndex);

		Change_Picking_Menu(ws.c_str(), m_iTagIndex);
	}

	// �޽� ��ŷ�� ������Ʈ ���� �޴�
	if (ImGui::Button("Delete Choice MapObject"))
	{
		Delete_Picking_Object();
	}

#endif
}

void CObject_Window::Change_Picking_Menu(const _tchar* wszTag, _uint iTagNum)
{
	ImGui::Text("Press J to Change Matrix");

	BEGININSTANCE; m_pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
		TEXT("Layer_MapObject"), wszTag)); 

	if (nullptr == m_pObject)
	{
		MSG_BOX("Failed to Find Picking Object");
		m_isChangeObject = false;
		ENDINSTANCE;
		return;
	}

	if (true == pGameInstance->Get_DIKeyState(DIK_J, CInput_Device::KEY_DOWN))
	{
		m_pObject->Get_Transform()->Set_WorldMatrix(m_pDummy->Get_Transform()->Get_WorldMatrix());
		m_vecSaveObject.at(m_iTagIndex).matTransform = m_pDummy->Get_Transform()->Get_WorldMatrix();
	} ENDINSTANCE;	
}

void CObject_Window::Delete_Picking_Object()
{
	if (0 < m_vecObjectTag_s.size())
	{
		BEGININSTANCE;
		if (FAILED(pGameInstance->Delete_Component(LEVEL_TOOL,
			TEXT("Layer_MapObject"), m_vecMapObjectTag.at(m_iTagIndex).c_str())))
		{
			MSG_BOX("Failed to delete Picking MapObject");
			return;
		} ENDINSTANCE;

		m_vecMapObjectTag.erase(m_vecMapObjectTag.begin() + m_iTagIndex);
		m_vecObjectTag_s.erase(m_vecObjectTag_s.begin() + m_iTagIndex);
		m_vecSaveObject.erase(m_vecSaveObject.begin() + m_iTagIndex);
	}
}

void CObject_Window::Map_Brushing_Menu()
{
	_float3 vPos = Find_PickingPos();

	BEGININSTANCE; CTerrain* pTerrain = static_cast<CTerrain*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, 
		TEXT("Layer_Tool"), TEXT("GameObject_Terrain")));

	ImGui::DragFloat("Brush Size", &m_fBrushSize, 0.1f, 0.1f, 100.f);

	// ���� Ŀ���� �����Ӱ� ũ�� ����
	pTerrain->Set_CurrentBrushingPoint(vPos);
	pTerrain->Set_CurrentBrushingSize(m_fBrushSize);

	// ���� �ؽ�ó�� �귯�� �ؽ�ó ����
	const char* GroundTexture[] = { "FarGrass", "Grass_Spring", "DeadGrass", "DeadGrassLeaves" };
	const char* BrushTexture[] = { "FarGrass", "Grass_Spring", "DeadGrass", "DeadGrassLeaves" };

	ImGui::Combo("GroundTexture", &m_iDiffuseIndex, GroundTexture, IM_ARRAYSIZE(GroundTexture));
	pTerrain->Set_DiffuseTextureIndex((_uint)m_iDiffuseIndex);

	ImGui::Combo("BrushTexture", &m_iBrushIndex, BrushTexture, IM_ARRAYSIZE(BrushTexture));
	pTerrain->Set_BrushTextureIndex((_uint)m_iBrushIndex);

	m_fTimeAccBrush += pGameInstance->Get_World_Tick();

	// ���콺�� �� ������ ������ ���� ��ġ
	if (true == pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_PRESSING) &&
		pGameInstance->IsMouseInClient(m_pContext, g_hWnd) &&
		-1.f != vPos.x &&
		0.1f < m_fTimeAccBrush)
	{
		pTerrain->Set_BrushDesc(vPos, m_fBrushSize, m_iBrushIndex);
		m_fTimeAccBrush = 0.f;
	} ENDINSTANCE;
}

HRESULT CObject_Window::Save_MapObject(string szMapDataPath)
{
	_tchar wszPath[MAX_PATH] = TEXT("../../Resources/GameData/MapData/");

	lstrcpy(m_wszMapSaveDataPath, TEXT(""));
	lstrcat(m_wszMapSaveDataPath, wszPath);
	wstring ws(szMapDataPath.begin(), szMapDataPath.end());
	lstrcat(m_wszMapSaveDataPath, ws.c_str());
	lstrcat(m_wszMapSaveDataPath, TEXT(".ddd"));

	HANDLE hFile = CreateFile(m_wszMapSaveDataPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Save MapObject");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	for (auto& iter : m_vecSaveObject)
	{
		if (!WriteFile(hFile, &iter.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.vPos");
			return E_FAIL;
		}			

		if (!WriteFile(hFile, &iter.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{ 
			MSG_BOX("Failed to Write m_vecSaveObject.iTagLen");
			return E_FAIL;
		}

		if (!WriteFile(hFile, &iter.wszTag, iter.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecSaveObject.wszTag");
			return E_FAIL;
		}
	}

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CObject_Window::Load_MapObject(const _tchar* wszMapDataPath)
{
	m_vecSaveObject.clear();

	HANDLE hFile = CreateFile(wszMapDataPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	while (true)
	{
		SAVEOBJECTDESC SaveDesc;
		ZEROMEM(&SaveDesc);

		if (!ReadFile(hFile, &SaveDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos");
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &SaveDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen");
			CloseHandle(hFile);
		}

		if (!ReadFile(hFile, &SaveDesc.wszTag, SaveDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		m_vecSaveObject.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	// �ε��� �����͸� ������� �ִ� �κ�
	for (size_t i = 0; i < m_vecSaveObject.size(); i++)
	{
		// �� ������Ʈ�� ��ȣ �ٿ���
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		// ��ȣ�� ���� �±׷� MapObject ���
		BEGININSTANCE if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &m_vecSaveObject[i].matTransform)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return E_FAIL;
		} ENDINSTANCE;

		// �������� ��ġ�� �� ������Ʈ �ּ� ������
		m_pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName));

		m_pObject->Add_Model_Component(m_vecSaveObject[i].wszTag);
		m_pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		m_pObject->Set_Color(m_iMapObjectIndex); // ������ ���� ���� �־���

		++m_iMapObjectIndex;
	}	

	return S_OK;
}

HRESULT CObject_Window::Save_MapObject_Ins()
{
	_tchar dataFile[MAX_PATH] = TEXT("../../Resources/GameData/MapData/MapData_Ins.ddd");

	HANDLE hFile = CreateFile(dataFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Save MapData_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	// �ѹ��� �� ������ �𵨸� ����
	for (_uint i = 0; i < 1; ++i)
	{
		if (!WriteFile(hFile, &m_iInsObjectCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_iInsObjectCnt");
			return E_FAIL;
		}
		
		// ���⼭ ���� ���Ϳ� �����ص� ������ �����Ѵ�.
		for (size_t i = 0; i < m_iInsObjectCnt; i++)
		{
			if (!WriteFile(hFile, &m_vecSaveInsObjectWorld[i], sizeof(_float4x4), &dwByte, nullptr))
			{
				MSG_BOX("Failed to Write m_vecSaveInsObjectWorld");
				return E_FAIL;
			}
		}

		_float4x4 vMatrix = XMMatrixIdentity();
		if (!WriteFile(hFile, &vMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write vMatrix");
			return E_FAIL;
		}

		// ���� ���õ� �� ���ڿ��� ����
		_uint iLength = lstrlen(m_vecModelList_t.at(m_iModelIndex)) * 2;

		if (!WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write iLength");
			return E_FAIL;
		}

		_tchar wszTag[MAX_PATH] = TEXT("");

		lstrcpy(wszTag, m_vecModelList_t.at(m_iModelIndex));

		// ���� ���õ� �� ���ڿ�
		if (!WriteFile(hFile, &wszTag, iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Write m_vecModelList_t.at(m_iModelIndex)");
			return E_FAIL;
		}
	}

	m_iInsObjectCnt = 0;
	m_vecSaveInsObjectWorld.clear();

	MSG_BOX("Save Success");

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CObject_Window::Load_MapObject_Ins()
{
	for (auto& iter : m_vecSaveInsObject)
	{
		Safe_Delete_Array(iter.pMatTransform);
	}

	m_vecSaveInsObject.clear();
	m_vecSaveInsObjectWorld.clear();

	_tchar dataFile[MAX_PATH] = TEXT("../../Resources/GameData/MapData/MapData_Ins.ddd");

	HANDLE hFile = CreateFile(dataFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;

	while (true)
	{
		SAVEINSOBJECTDESC SaveDesc;
		ZEROMEM(&SaveDesc);

		if (!ReadFile(hFile, &SaveDesc.iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.iInstanceCnt");
			return E_FAIL;
		}

		// ����Ǿ��ִ� �ν��Ͻ� ������ŭ �����Ҵ�
		if (0 != SaveDesc.iInstanceCnt)
		{
			SaveDesc.pMatTransform = new _float4x4[SaveDesc.iInstanceCnt];

			for (size_t i = 0; i < SaveDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &SaveDesc.pMatTransform[i], sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read m_vecSaveInsObject.pMatTransform");
					return E_FAIL;
				}
			}
		}

		if (!ReadFile(hFile, &SaveDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.matTransform");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &SaveDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.iTagLen");
		}

		if (!ReadFile(hFile, &SaveDesc.wszTag, SaveDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveInsObject.wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		m_vecSaveInsObject.push_back(SaveDesc);
	}

	MSG_BOX("Load Successed");

	CloseHandle(hFile);

	// �ε��� �����͸� ������� �ִ� �κ�
	for (size_t i = 0; i < m_vecSaveInsObject.size(); i++)
	{
		// �� ������Ʈ�� ��ȣ �ٿ���
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (m_iMapObjectIndex));
		Deep_Copy_Tag(wszobjName);

		// ���⼭ ������Ÿ�� �±� ���ڿ� �����������
		wstring ws = TEXT("Prototype_Component_Model_Instance_");
		wstring wsTag = TEXT("Prototype_Component_Model_");
		wstring wsSave(m_vecSaveInsObject.at(i).wszTag);
		_uint iLength = wsTag.size();

		// �� �̸�
		wstring wsModelName = wsSave.substr(iLength);
		ws += wsModelName;

		wstring wsPath = TEXT("../../Resources/Models/MapObject/NonAnims/");
		wsPath += wsModelName;
		wsPath += TEXT("/");
		wsPath += wsModelName;
		wsPath += TEXT(".dat");

		// �ν��Ͻ� �� ������Ÿ�� ����
		_float4x4 PivotMatrix = XMMatrixIdentity();
		BEGININSTANCE; if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, ws.c_str(),
			CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, wsPath.c_str(),
				m_vecSaveInsObject.at(i).pMatTransform, m_vecSaveInsObject.at(i).iInstanceCnt, PivotMatrix), true)))
		{
			MSG_BOX("Failed to Create New CModel_Instance Prototype");
		}

		// ��ȣ�� ���� �±׷� MapObject_Ins ���
		if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL,
			TEXT("Prototype_GameObject_MapObject_Ins"), TEXT("Layer_MapObject"),
			m_vecMapObjectTag.at(m_iMapObjectIndex).c_str(), &m_vecSaveInsObject[i].matTransform)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return E_FAIL;
		}

		// �������� ��ġ�� �� ������Ʈ �ּ� ������
		m_pObjIns = static_cast<CMapObject_Ins*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL,
			TEXT("Layer_MapObject"), wszobjName)); ENDINSTANCE;

		if (nullptr != m_pObjIns)
		{
			m_pObjIns->Add_Model_Component(ws.c_str());
			m_pObjIns->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMeshInstance"));
			m_pObjIns->Set_Color(m_iMapObjectIndex); // ������ ���� ���� �־���
		}

		++m_iMapObjectIndex;
	}

	return S_OK;
}

void CObject_Window::Deep_Copy_Name(const _tchar* wszName)
{
	size_t length = 0;
	wstring ws;

	// �� �̸��� const _tchar* ���·� ���� ����
	if (nullptr != wszName)
	{
		length = wcslen(wszName);
		ws.append(wszName);
	}

	else
	{
		length = m_strCurrentModel.size();
		ws.append(m_strCurrentModel.begin(), m_strCurrentModel.end());
	}

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Name)");
		return;
	}

	m_vecModelList_t.push_back(wszNew);
}

void CObject_Window::Deep_Copy_Path(const _tchar* wszPath)
{
	// �� ��θ� const _tchar* ���·� ���� ����
	size_t length = wcslen(wszPath);

	wstring ws(wszPath);

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(Path)");
		return;
	}

	_uint iCount = 0;

	for (auto& iter : m_vecModelPath_t)
	{
		// �ߺ��Ǵ� ���ڿ��� ���� ��� ����
		if (0 == lstrcmp(wszNew, iter))
		{
			++iCount;
		}
	}
	
	// �ߺ��̶�� �������� �ʴ´�.
	if (0 == iCount)
	{
		m_vecModelPath_t.push_back(wszNew);
	}

	else
	{
		delete[] wszNew;
	}		
}

void CObject_Window::Deep_Copy_Tag(const _tchar* wszTag)
{
	m_vecMapObjectTag.push_back(wszTag);

	// ���� ����� ���ϰ� ��ȯ�� ����
	size_t length = wcslen(wszTag);

	char c[MAX_PATH] = "";;
	WCharToChar(wszTag, c);

	string s(c);

	m_vecObjectTag_s.push_back(s);
}

const _tchar* CObject_Window::Deep_Copy(const _tchar* wszString)
{
	// �� ������Ʈ ��θ� const _tchar* ���·� ���� ����
	size_t length = wcslen(wszString);

	wstring ws(wszString);

	_tchar* wszNew = New _tchar[length + 1];

	if (0 != wcscpy_s(wszNew, length + 1, ws.c_str()))
	{
		MSG_BOX("Falied to Deep Copy(String)");
		return nullptr;
	}

	return wszNew;
}

_float3 CObject_Window::Find_PickingPos()
{
	if (nullptr == m_pDummy)
	{
		ImGui::Text("Dummy does not exist");
		return _float3(-1.f, -1.f, -1.f);
	}

	_float4 vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_float4 vRayDir = { 0.f, 0.f, 0.f, 0.f };

	BEGININSTANCE; pGameInstance->Get_WorldMouseRay(m_pContext, g_hWnd, &vRayPos, &vRayDir);

	CVIBuffer_Terrain* pTerrain = static_cast<CVIBuffer_Terrain*>(
		static_cast<CTerrain*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"),
			TEXT("GameObject_Terrain")))->Get_Buffer()); ENDINSTANCE;

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

			return _float3(vFinalPos.x, vFinalPos.y, vFinalPos.z);
		}
	}

	return _float3(-1.f, -1.f, -1.f);
}

HRESULT CObject_Window::Create_Dummy()
{
	// �̹� ���̰� �ִٸ� ����
	if (nullptr != m_pDummy)
	{
		MSG_BOX("Dummy Already exists");
		return S_OK;
	}

	_float3 vPos = { 5.f, 0.f, 5.f };

	BEGININSTANCE; if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_MapDummy"),
		TEXT("Layer_Tool"), TEXT("Map_Dummy"), &vPos)))
	{
		MSG_BOX("Failed to GameObject Map_Dummy");
		return E_FAIL;
	}

	m_pDummy = static_cast<CMapDummy*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Map_Dummy")));
	m_pDummy->Add_Model_Component(TEXT("Prototype_Component_Model_Tree"));
	m_pDummy->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh")); ENDINSTANCE;

	return S_OK;
}

HRESULT CObject_Window::Delete_Dummy()
{
	// �̹� ���̰� ���ٸ� ����
	if (nullptr == m_pDummy)
	{
		MSG_BOX("Dummy Already deleted");
		return S_OK;
	}

	BEGININSTANCE;
	if (FAILED(pGameInstance->Delete_Component(LEVEL_TOOL,
		TEXT("Layer_Tool"), TEXT("Map_Dummy"))))
	{
		MSG_BOX("Failed to delete MapDummy");
		return E_FAIL;
	} ENDINSTANCE;

	m_pDummy = nullptr;

	return S_OK;
}

HRESULT CObject_Window::Save_Model_Path(_uint iType, const _tchar* pFilePath)
{
	// ���丮 ��θ� ��ȸ�� iterator
	fs::directory_iterator iter(fs::absolute(pFilePath));

	while (iter != fs::end(iter))
	{
		// ���� ���丮 ��θ� ����ִ� ���� (iterator�� ����)
		const fs::directory_entry& entry = *iter;

		// ���� entry ������ ���丮���� Ȯ�� �� ���丮�̸� ���
		if (fs::is_directory(entry.path()))
		{
			Save_Model_Path(0, entry.path().c_str());
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

				Deep_Copy_Path(pathresult.c_str());

				// ��ο��� �� �̸� �κи� �߶󳻴� �κ�
				string s = entry.path().string();

				size_t path_length = pathresult.length();
				size_t current = s.find("MapObject\\NonAnims") + 19;

				// 1�� �и�, ���⼭ �� �̸� ���� ��ΰ� ���;� ��.
				string result = s.substr(current, path_length);

				// ���ϸ� Lod�� �� �ִ��� ã�´�.
				if (std::string::npos != result.find(("_Lod")))
				{
					// ���� �ִٸ� ������ Ÿ���� ������ �ʴ´�.
					iter++;
					continue;
				}

				size_t current1 = result.find("\\");

				// 2�� �и�, ���⼭ �� �̸��� ���;� ��
				string result1 = result.substr(0, current1);

				// �� ����Ʈ�� ����� ���ڿ� ������ ����
				m_vecModelList.push_back(result1);

				// ���� ������Ʈ �� �̸����� ����
				string modelname = ("Prototype_Component_Model_");
				modelname += result1;

				wstring wmodelname(modelname.begin(), modelname.end());
				Deep_Copy_Name(wmodelname.c_str());

				// ������Ÿ�� ����
				_float4x4 PivotMatrix;
				BEGININSTANCE; if (FAILED(pGameInstance->Add_Prototype(LEVEL_TOOL, m_vecModelList_t.back(),
					CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, m_vecModelPath_t.back(), PivotMatrix), true)))
				{
					MSG_BOX("Failed to Create New Model Prototype");
				} ENDINSTANCE;
			}
		}

		iter++;
	}

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

	// ���� �Ҵ� ����� ���ڿ� ����
	for (auto& iter : m_vecModelList_t)
	{
		delete[] iter;
	}
	m_vecModelList_t.clear();

	for (auto& iter : m_vecModelPath_t)
	{
		delete[] iter;
	}
	m_vecModelPath_t.clear();

	m_vecObjectTag_s.clear();
	m_vecMapObjectTag.clear();

	for (auto& iter : m_vecSaveInsObject)
	{
		Safe_Delete_Array(iter.pMatTransform);
	}
}
