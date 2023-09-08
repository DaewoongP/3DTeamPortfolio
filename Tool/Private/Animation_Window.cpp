#include "..\Public\Animation_Window.h"
#include "ImGuiFileDialog.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Notify.h"
#include "Camera_Point.h"
#include "Bounding_Sphere.h"

CAnimation_Window::CAnimation_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CAnimation_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	BEGININSTANCE;

	CCamera_Point::CAMERAPOINTDESC CameraPointDesc;

	CameraPointDesc.vPosition = _float4(0,0,0,1);
	m_pCameraPoint =
		dynamic_cast<CCamera_Point*>(
			pGameInstance->Clone_Component(LEVEL_TOOL,
				TEXT("Prototype_GameObject_Camera_Point"),
				&CameraPointDesc));

	CBounding_Sphere::BOUNDINGSPHEREDESC sphereDesc;
	sphereDesc.fRadius = 0.1f;
	sphereDesc.vPosition = _float3(0, 0, 0);
	m_pCameraPoint->Set_BoundingDesc(&sphereDesc);
	ENDINSTANCE;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CAnimation_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Animation", nullptr, m_WindowFlag);
	Export_Model();
	Create_Dummy_Button();
	OpenFile_Button();
	Select_Model();
	AddModel_Button();

	

	if (m_pDummyObject == nullptr)
	{
		ImGui::End();
		return;
	}
	CModel* pDummyModel = dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")));
	if (pDummyModel == nullptr)
	{
		ImGui::End();
		return;
	}
	
	ImGui::Separator();
	ImGui::Text("AnimationOffset");

	OffsetVectorSetting(pDummyModel);

	ImGui::Separator();
	ImGui::Text("Notify");
	Notify_InputFileds(m_szNotifyName, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);

	
	ImGui::Separator();
	ImGui::Text("Root&BoneTree");

	Bone_Tree(pDummyModel->Get_Bone_Index(0), pDummyModel);
	ImGui::InputInt("RootBone", &m_iRootIndex);

	if (ImGui::Button("Set_Root"))
	{
		pDummyModel->Set_RootBone(m_iRootIndex);
	}

	for (_uint partCnt = 0; partCnt < pDummyModel->Get_AnimationPartCount(); partCnt++)
	{
		
		ImGui::Separator();
		ImGui::Text("AnimationSetting");

		CModel::ANIMTYPE ePartCnt = static_cast<CModel::ANIMTYPE>(partCnt);

		Animation_ChildFrame(ePartCnt,m_szCurrentItem[ePartCnt], pDummyModel);
		Animation_Action_Button(ePartCnt,pDummyModel, &m_fNotifyActionTime);

		_char szUIName[MAX_PATH] = "Animation##";
		if (pDummyModel->Get_NumAnimations(ePartCnt) != 0)
		{
			sprintf_s(szUIName, "%s%d", "AnimationTag##", ePartCnt);
			WCharToChar(pDummyModel->Get_Animation(ePartCnt)->Get_AnimationName(), m_szAnimationTag[ePartCnt]);
			if (ImGui::InputText(szUIName, m_szAnimationTag[ePartCnt], 32))
			{
				_tchar wsaAnimationTag[MAX_PATH] = {};
				CharToWChar(m_szAnimationTag[ePartCnt], wsaAnimationTag);
				pDummyModel->Get_Animation(ePartCnt)->Set_AnimationName(wsaAnimationTag);
			}
			sprintf_s(szUIName, "%s%d", "SetRootAnim##", ePartCnt);
			if (ImGui::Checkbox(szUIName, pDummyModel->Get_Animation(ePartCnt)->Get_RootAnim_Point()))
			{
			}
			ImGui::SameLine();
			sprintf_s(szUIName, "%s%d", "SetLerpAnim##", ePartCnt);
			if (ImGui::Checkbox(szUIName, pDummyModel->Get_Animation(ePartCnt)->Get_LerpAnim_Point()))
			{
			}
			ImGui::SameLine();
			sprintf_s(szUIName, "%s%d", "SetLoopAnim##", ePartCnt);
			if (ImGui::Checkbox(szUIName, pDummyModel->Get_Animation(ePartCnt)->Get_LoopAnim_Point()))
			{
			}

			sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
			ImGui::SliderFloat(szUIName, pDummyModel->Get_Animation(ePartCnt)->Get_Accmulation_Pointer(), 0, pDummyModel->Get_Animation(ePartCnt)->Get_Duration());
			pDummyModel->Get_Animation(ePartCnt)->Update_KeyFrame_By_Time();
			Add_Notify_Button(ePartCnt, m_szNotifyName, pDummyModel, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);

			Edit_Notify_Button(ePartCnt, pDummyModel);
			Create_Notify_ChildFrame(ePartCnt, pDummyModel);

			
			ImGui::Separator();
			ImGui::Text("Separate_Parts");
			sprintf_s(szUIName, "Separate##%d", ePartCnt);

			sprintf_s(szUIName, "Bone_From##%d", ePartCnt);
			ImGui::InputInt(szUIName, &m_iFromBone[ePartCnt]);
			sprintf_s(szUIName, "Bone_To##%d", ePartCnt);
			ImGui::InputInt(szUIName, &m_iToBone[ePartCnt]);

			sprintf_s(szUIName, "Seprate##%d", ePartCnt);
			if (ImGui::Button(szUIName))
			{
				pDummyModel->Separate_Animation(m_iFromBone[ePartCnt], m_iToBone[ePartCnt],ePartCnt);
			}
		}
	}

	if (ImGui::Button("Reset_Dummy_WorldMatrix##Animation"))
	{
		dynamic_cast<CTransform*>(m_pDummyObject->Find_Component(TEXT("Com_Transform")))->Set_WorldMatrix(XMMatrixIdentity());
	}

	if (ImGui::Button("Delete_Dummy##Animation"))
	{
		BEGININSTANCE;
		if (FAILED(pGameInstance->Delete_Component(LEVEL_TOOL,
			TEXT("Layer_Tool"), TEXT("Dummy_Animation"))))
		{
			MSG_BOX("Failed to delete Dummy_Animation");
		} ENDINSTANCE;
		m_pDummyObject = nullptr;
	}
	ImGui::End();
}

HRESULT CAnimation_Window::Render()
{
	return S_OK;
}

void CAnimation_Window::Create_Dummy_Button()
{
	if (ImGui::Button("Create Dummy"))
	{
		if(m_pDummyObject!=nullptr)
		{
			MSG_BOX("Failed because of Dummy is Not Null");
			return;
		}

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Add_Component(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy"), TEXT("Layer_Tool"), TEXT("Dummy_Animation"), nullptr)))
		{
			MSG_BOX("Failed to Created Dummy Clone");
		}

		m_pDummyObject = dynamic_cast<CDummy*>(pGameInstance->Find_Component_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Dummy_Animation")));
		Safe_Release(pGameInstance);
	}
}

void CAnimation_Window::OpenFile_Button()
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

void CAnimation_Window::AddModel_Button()
{
	if (ImGui::Button("AddModelToDummy"))
	{
		if (m_pDummyObject == nullptr)
		{
			MSG_BOX("Failed to Add Model");
			return;
		}

		m_pDummyObject->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
		lstrcpy(m_wszCurrentDummyModelTag, m_vecModelList_t[m_iModelIndex].c_str());
		m_pDummyObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
		CModel* pCurrentModel = dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")));
		_tchar wszAnimationName[MAX_PATH] = {};
		_tchar wszTypeComboName[MAX_PATH] = {};
		lstrcpy(wszAnimationName, pCurrentModel->Get_Animation(0)->Get_AnimationName());
		lstrcpy(wszTypeComboName, TEXT("Speed"));

		m_iRootIndex = pCurrentModel->Get_RootBoneIndex();

		WCharToChar(wszAnimationName, m_szCurrentItem[0]);
		WCharToChar(wszTypeComboName, m_szCurrentItemType);
	}
}

void CAnimation_Window::Animation_ComboBox(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "AnimComboBox##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	if (ImGui::BeginCombo(szUIName, szCurrentItem))
	{
		_char szAnimationName[MAX_PATH] = "";
		_tchar wszAnimationName[MAX_PATH] = {};
		for (_uint i = 0; i < pDummyModel->Get_NumAnimations(ePartCnt); i++)
		{
			ZEROMEM(szAnimationName);
			ZEROMEM(wszAnimationName);

			lstrcpy(wszAnimationName, pDummyModel->Get_Animation(i, ePartCnt)->Get_AnimationName());
			WCharToChar(wszAnimationName, szAnimationName);
			bool is_selected = (!strcmp(szCurrentItem, szAnimationName));
			ImGui::Text(szAnimationName);

			ImGui::SameLine();
			_char szUIName[MAX_PATH] = "Select##_Anim";
			sprintf_s(szUIName, "%s%d##%s", szUIName, ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				strcpy_s(szCurrentItem, sizeof(szAnimationName), szAnimationName);
				ImGui::SetItemDefaultFocus();
				pDummyModel->Reset_Animation(i, ePartCnt);
			}

			ImGui::SameLine();
			sprintf_s(szUIName, "Delete##_Anim%d%s" ,ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				pDummyModel->Delete_Animation(i, ePartCnt);
			}
		}
		ImGui::EndCombo();
	}
}

void CAnimation_Window::Animation_ChildFrame(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "AnimChildFrame##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	const auto  draw_childframe_size = ImVec2(400, 260);
	ImGui::BeginChildFrame(ImGui::GetID(szUIName), draw_childframe_size,ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	{
		_char szAnimationName[MAX_PATH] = "";
		_tchar wszAnimationName[MAX_PATH] = {};
		for (_uint i = 0; i < pDummyModel->Get_NumAnimations(ePartCnt); i++)
		{
			ZEROMEM(szAnimationName);
			ZEROMEM(wszAnimationName);

			lstrcpy(wszAnimationName, pDummyModel->Get_Animation(i, ePartCnt)->Get_AnimationName());
			WCharToChar(wszAnimationName, szAnimationName);
			bool is_selected = (!strcmp(szCurrentItem, szAnimationName));
			ImGui::Text(szAnimationName);

			ImGui::SameLine();
			_char szUIName[MAX_PATH] = "Select##_Anim";
			sprintf_s(szUIName, "%s%d##%s", szUIName, ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				strcpy_s(szCurrentItem, sizeof(szAnimationName), szAnimationName);
				ImGui::SetItemDefaultFocus();
				pDummyModel->Reset_Animation(i, ePartCnt);
			}

			ImGui::SameLine();
			sprintf_s(szUIName, "Delete##_Anim%d%s", ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				pDummyModel->Delete_Animation(i, ePartCnt);
			}
		}
	}
	ImGui::EndChildFrame();
}

void CAnimation_Window::Animation_Table(CModel::ANIMTYPE ePartCnt, _char* szCurrentItem, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "AnimChildFrame##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	const auto  draw_childframe_size = ImVec2(3000, 260);
	ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_SizingFixedFit;

	ImGui::BeginTable(szUIName, 4, flags, ImVec2(0, 300),0.f);
	{
		ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, ImGui::GetID(szUIName));
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
		ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);
		ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f);

		ImGui::TableHeadersRow();

		_char szAnimationName[MAX_PATH] = "";
		_tchar wszAnimationName[MAX_PATH] = {};
		for (_uint i = 0; i < pDummyModel->Get_NumAnimations(ePartCnt); i++)
		{
			ZEROMEM(szAnimationName);
			ZEROMEM(wszAnimationName);
			
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			sprintf_s(szUIName, "%d", i);
			ImGui::Text(szUIName);
			
			ImGui::TableSetColumnIndex(1);
			lstrcpy(wszAnimationName, pDummyModel->Get_Animation(i, ePartCnt)->Get_AnimationName());
			WCharToChar(wszAnimationName, szAnimationName);
			bool is_selected = (!strcmp(szCurrentItem, szAnimationName));
			ImGui::Text(szAnimationName);

			ImGui::TableSetColumnIndex(2);
			_char szUIName[MAX_PATH] = "Select##_Anim";
			sprintf_s(szUIName, "%s%d##%s", szUIName, ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				strcpy_s(szCurrentItem, sizeof(szAnimationName), szAnimationName);
				ImGui::SetItemDefaultFocus();
				pDummyModel->Reset_Animation(i, ePartCnt);
			}

			ImGui::TableSetColumnIndex(3);
			sprintf_s(szUIName, "Delete##_Anim%d%s", ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				pDummyModel->Delete_Animation(i, ePartCnt);
			}
		}
		ImGui::EndTable();
	}
	
}

void CAnimation_Window::Animation_Action_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel, _float* fNotifyActionTime)
{
	_uint iAnimCnt = pDummyModel->Get_NumAnimations(ePartCnt);
	if (iAnimCnt == 0)
		return;
	_char szUIName[MAX_PATH] = "";
	if(pDummyModel->Get_Animation(ePartCnt)->Get_Paused_State())
		sprintf_s(szUIName, "Stop##%d", ePartCnt);
	else
		sprintf_s(szUIName, "Play##%d", ePartCnt);

	if (ImGui::Button(szUIName))
	{
		pDummyModel->Get_Animation(ePartCnt)->Set_Pause(!pDummyModel->Get_Animation(ePartCnt)->Get_Paused_State());
		*fNotifyActionTime = pDummyModel->Get_Animation(ePartCnt)->Get_Accmulation();
	}
}

void CAnimation_Window::Notify_InputFileds(_char* szNotifyName, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed)
{
	_char szUIName[MAX_PATH] = "NotifyName##";
	ImGui::InputText(szUIName, szNotifyName, 32);
	const char* items[] = { "Speed","Notify","Sound" };

	sprintf_s(szUIName, "%s", "KeyFrameType##");
	if (ImGui::BeginCombo(szUIName, m_szCurrentItemType))
	{
		for (int i = 0; i < IM_ARRAYSIZE(items); i++)
		{
			bool is_selected = (items[i] == m_szCurrentItemType);
			if (ImGui::Selectable(items[i], is_selected))
			{
				*eNotifyKeyFrameType = static_cast<KEYFRAME::KEYFRAMETYPE>(i);
				strcpy_s(m_szCurrentItemType, sizeof(m_szCurrentItemType), items[i]);
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	sprintf_s(szUIName, "%s", "ActionTime##");
	ImGui::InputFloat(szUIName, fNotifyActionTime);
	if (*eNotifyKeyFrameType == KEYFRAME::KEYFRAMETYPE::KF_SPEED)
	{
		sprintf_s(szUIName, "%s", "Speed##");
		ImGui::InputFloat(szUIName, fNotifySpeed);
	}
}

void CAnimation_Window::Add_Notify_Button(CModel::ANIMTYPE ePartCnt, _char* szNotifyName, CModel* pDummyModel, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed)
{
	_char szUIName[MAX_PATH] = "Add_Notify##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	if (ImGui::Button(szUIName))
	{
		_tchar  wszNotifyName[MAX_PATH] = {};
		CharToWChar(szNotifyName, wszNotifyName);
		
		

		switch (*eNotifyKeyFrameType)
		{
		case KEYFRAME_GCM::KF_SPEED:
		{
			SPEEDFRAME_GCM* NotifyKeyFrameDesc = new SPEEDFRAME_GCM{};
			NotifyKeyFrameDesc->eKeyFrameType = *eNotifyKeyFrameType;
			NotifyKeyFrameDesc->fTime = *fNotifyActionTime;
			lstrcpy(NotifyKeyFrameDesc->szName, wszNotifyName);
			NotifyKeyFrameDesc->fSpeed = *fNotifySpeed;
			if (FAILED(pDummyModel->Get_Animation(ePartCnt)->Add_NotifyFrame(NotifyKeyFrameDesc)))
			{
				MSG_BOX("Failed To Add Notify");
			}
			Safe_Delete(NotifyKeyFrameDesc);
			break;
		}
		case KEYFRAME_GCM::KF_NOTIFY:
		{
			NOTIFYFRAME_GCM* NotifyKeyFrameDesc = new NOTIFYFRAME_GCM{};
			NotifyKeyFrameDesc->eKeyFrameType = *eNotifyKeyFrameType;
			NotifyKeyFrameDesc->fTime = *fNotifyActionTime;
			lstrcpy(NotifyKeyFrameDesc->szName, wszNotifyName);
			//reinterpret_cast<NOTIFYFRAME_GCM*>(&NotifyKeyFrameDesc)->fSpeed;
			if (FAILED(pDummyModel->Get_Animation(ePartCnt)->Add_NotifyFrame(NotifyKeyFrameDesc)))
			{
				MSG_BOX("Failed To Add Notify");
			}
			Safe_Delete(NotifyKeyFrameDesc);
			break;
		}
		case KEYFRAME_GCM::KF_SOUND:
		{
			NOTIFYFRAME_GCM* NotifyKeyFrameDesc = new NOTIFYFRAME_GCM{};
			NotifyKeyFrameDesc->eKeyFrameType = *eNotifyKeyFrameType;
			NotifyKeyFrameDesc->fTime = *fNotifyActionTime;
			lstrcpy(NotifyKeyFrameDesc->szName, wszNotifyName);
			//reinterpret_cast<NOTIFYFRAME_GCM*>(&NotifyKeyFrameDesc)->fSpeed;
			if (FAILED(pDummyModel->Get_Animation(ePartCnt)->Add_NotifyFrame(NotifyKeyFrameDesc)))
			{
				MSG_BOX("Failed To Add Notify");
			}
			Safe_Delete(NotifyKeyFrameDesc);
			break;
		}
		}
	}
}

void CAnimation_Window::Edit_Notify_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel)
{
	ImGui::SameLine();
	_char szUIName[MAX_PATH] = "Edit_Notify##Notify##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	if (ImGui::Button(szUIName))
	{
		CNotify* pNotify = pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point();
		pNotify->Edit_Frame(m_iSelectedNotifyIndex[ePartCnt], m_eNotifyKeyFrameType, m_fNotifyActionTime, m_fNotifySpeed);
	}
}

void CAnimation_Window::Select_Model()
{
	ImGui::Text("AnimModelList");
	ImGui::ListBox("##AnimModelListBox", &m_iModelIndex, VectorGetter, static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);
}

void CAnimation_Window::Export_Model()
{
	if (ImGui::Button("Export_Model"))
	{
		if (m_pDummyObject == nullptr)
		{
			MSG_BOX("Failed to Export Because of No Dummy");
			return;
		}
		if (nullptr == m_pDummyObject->Find_Component(TEXT("Com_Model")))
		{
			MSG_BOX("Failed to Export Because of No Model");
			return;
		}
		dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")))->Convert_Animations_GCM();
		
		wchar_t temp[MAX_PATH] = {};
		wcscpy_s(temp, m_wszCurrentDummyModelTag);
		const wchar_t* found = wcsstr(temp, TEXT("Prototype_Component_Model_"));
		if (found != nullptr) {
			wcscpy_s(temp, found + wcslen(TEXT("Prototype_Component_Model_")));
		}
		dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")))->Write_File_GCM(CModel::TYPE_ANIM, temp);
	}
}

void CAnimation_Window::OffsetVectorSetting(CModel* pDummyModel)
{
	if (pDummyModel->Get_NumAnimations() == 0)
		return;

	_float data[3] = {};
	_float3 pos = pDummyModel->Get_Animation()->Get_OffsetPosition();
	memcpy(data, &pos, sizeof(_float) * 3);
	if (ImGui::DragFloat3("#OffsetVectorSetting", data,0.01f))
	{
		pDummyModel->Get_Animation()->Set_OffsetPosition(data);
	}

	if (m_pCameraPoint == nullptr)
		return;

	_float4 vCombinedPosition = { data[0],data[1],data[2],1};
	vCombinedPosition = XMVector3TransformCoord(vCombinedPosition.xyz(), m_pDummyObject->Get_Transform()->Get_WorldMatrix());
	m_pCameraPoint->Set_Position(vCombinedPosition);
	BEGININSTANCE
	m_pCameraPoint->Tick(pGameInstance->Get_TimeDelta(TEXT("MainTimer")));
	ENDINSTANCE
}

void CAnimation_Window::Create_Notify_ChildFrame(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "AnimNotifyChildFrame##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	const auto  draw_childframe_size = ImVec2(400, 260);
	ImGui::BeginChildFrame(ImGui::GetID(szUIName), draw_childframe_size, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
	{
		for (_uint iNotifyCount = 0; iNotifyCount < pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point()->Get_NotifyFrameCount(); iNotifyCount++)
		{
			CNotify* pNotify = pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point();
			_char  szNotifyButtonName[MAX_PATH];
			ZEROMEM(szNotifyButtonName);

			WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), szNotifyButtonName);
			sprintf_s(szNotifyButtonName, "%s##%d%d", szNotifyButtonName, ePartCnt, iNotifyCount);
			KEYFRAME::KEYFRAMETYPE eNotifyType = pNotify->Find_Frame(iNotifyCount)->eKeyFrameType;

			if (ImGui::ColorButton("Type",
				(eNotifyType == KEYFRAME::KF_NOTIFY) ? (ImVec4(0.7f, 0.f, 0.f, 1)) :
				((eNotifyType == KEYFRAME::KF_SOUND) ? (ImVec4(0.0f, 0.7f, 0.f, 1)) :
					(ImVec4(0.0f, 0.f, 0.7f, 1)))))
			{
			}
			ImGui::SameLine();

			if (ImGui::Button(szNotifyButtonName))
			{
				//정보갱신
				m_fNotifyActionTime = pNotify->Find_Frame(iNotifyCount)->fTime;
				WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), m_szNotifyName);
				m_eNotifyKeyFrameType = pNotify->Find_Frame(iNotifyCount)->eKeyFrameType;
				if (m_eNotifyKeyFrameType == KEYFRAME::KF_SPEED)
				{
					m_fNotifySpeed = static_cast<SPEEDFRAME*>(pNotify->Find_Frame(iNotifyCount))->fSpeed;
				}
				m_iSelectedNotifyIndex[ePartCnt] = iNotifyCount;
			}
			ImGui::SameLine();
			ImGui::Text((eNotifyType == KEYFRAME::KF_NOTIFY) ? ("Notify") :
				((eNotifyType == KEYFRAME::KF_SOUND) ? (("Sound")) :
					(("Speed"))));

			_char  szTimeMessage[MAX_PATH] = "Action Time : ";
			sprintf_s(szTimeMessage, "%s%f", szTimeMessage, pNotify->Find_Frame(iNotifyCount)->fTime);
			ImGui::SameLine();  ImGui::Text(szTimeMessage);
			ImGui::SameLine();
			if (eNotifyType == KEYFRAME::KF_SPEED)
			{
				_char  szSpeedMessage[MAX_PATH] = "Speed : ";
				SPEEDFRAME* frame = static_cast<SPEEDFRAME*>(pNotify->Find_Frame(iNotifyCount));
				sprintf_s(szSpeedMessage, "%s%f", szSpeedMessage, frame->fSpeed);
				ImGui::SameLine();  ImGui::Text(szSpeedMessage);
			}
			ImGui::SameLine();
			_char szDeleteButtonName[MAX_PATH] = "Delete##";
			sprintf_s(szDeleteButtonName, "%s%d", szDeleteButtonName, iNotifyCount);
			if (ImGui::Button(szDeleteButtonName))
			{
				pNotify->Delete_Frame(iNotifyCount);
			}
		}
	}
	ImGui::EndChildFrame();
}

void CAnimation_Window::Create_Notify_View(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel)
{
	for (_uint iNotifyCount = 0; iNotifyCount < pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point()->Get_NotifyFrameCount(); iNotifyCount++)
	{
		CNotify* pNotify = pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point();
		_char  szNotifyButtonName[MAX_PATH];
		ZEROMEM(szNotifyButtonName);

		WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), szNotifyButtonName);
		sprintf_s(szNotifyButtonName, "%s_%d", szNotifyButtonName, iNotifyCount);
		KEYFRAME::KEYFRAMETYPE eNotifyType = pNotify->Find_Frame(iNotifyCount)->eKeyFrameType;

		if (ImGui::ColorButton("Type",
			(eNotifyType == KEYFRAME::KF_NOTIFY) ? (ImVec4(0.7f, 0.f, 0.f, 1)) :
			((eNotifyType == KEYFRAME::KF_SOUND) ? (ImVec4(0.0f, 0.7f, 0.f, 1)) :
				(ImVec4(0.0f, 0.f, 0.7f, 1)))))
		{
		}
		ImGui::SameLine();

		if (ImGui::Button(szNotifyButtonName))
		{
			//정보갱신
			m_fNotifyActionTime = pNotify->Find_Frame(iNotifyCount)->fTime;
			WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), m_szNotifyName);
			m_eNotifyKeyFrameType = pNotify->Find_Frame(iNotifyCount)->eKeyFrameType;
			if (m_eNotifyKeyFrameType == KEYFRAME::KF_SPEED)
			{
				m_fNotifySpeed = static_cast<SPEEDFRAME*>(pNotify->Find_Frame(iNotifyCount))->fSpeed;
			}
			m_iSelectedNotifyIndex[ePartCnt] = iNotifyCount;
		}
		ImGui::SameLine();
		ImGui::Text((eNotifyType == KEYFRAME::KF_NOTIFY) ? ("Notify") :
			((eNotifyType == KEYFRAME::KF_SOUND) ? (("Sound")) :
				(("Speed"))));

		_char  szTimeMessage[MAX_PATH] = "Action Time : ";
		sprintf_s(szTimeMessage, "%s%f", szTimeMessage, pNotify->Find_Frame(iNotifyCount)->fTime);
		ImGui::SameLine();  ImGui::Text(szTimeMessage);
		ImGui::SameLine();
		if (eNotifyType == KEYFRAME::KF_SPEED)
		{
			_char  szSpeedMessage[MAX_PATH] = "Speed : ";
			SPEEDFRAME* frame = static_cast<SPEEDFRAME*>(pNotify->Find_Frame(iNotifyCount));
			sprintf_s(szSpeedMessage, "%s%f", szSpeedMessage, frame->fSpeed);
			ImGui::SameLine();  ImGui::Text(szSpeedMessage);
		}
		_char szDeleteButtonName[MAX_PATH] = "Delete##";
		sprintf_s(szDeleteButtonName, "%s%d", szDeleteButtonName, iNotifyCount);
		if (ImGui::Button(szDeleteButtonName))
		{
			pNotify->Delete_Frame(iNotifyCount);
		}
	}
}

void CAnimation_Window::Bone_Tree(CBone* bone, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "";
	_char szBone_Name[MAX_PATH] = "";
	WCharToChar(bone->Get_Name(), szBone_Name);

	sprintf_s(szUIName, "%s##%d", szBone_Name,0);
	if (ImGui::TreeNode(szUIName))
	{
		for (auto child : *pDummyModel->Get_Bone_Vector_Point())
		{
			if (bone->Get_Index() == child->Get_ParentNodeIndex())
			{
				Bone_Tree(child, pDummyModel);
			}
		}
		ImGui::TreePop();
	}
	else 
	{
		ImGui::SameLine();
		sprintf_s(szUIName, "Set_Root##%d##%s", 0, szBone_Name);
		if (ImGui::SmallButton(szUIName))
		{
			m_iRootIndex = bone->Get_Index();
		}
	}
}

CAnimation_Window* CAnimation_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CAnimation_Window* pInstance = New CAnimation_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CAnimation_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CAnimation_Window::Free(void)
{
	__super::Free();

	Safe_Release(m_pCameraPoint);
}
