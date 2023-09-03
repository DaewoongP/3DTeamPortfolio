#include "..\Public\Animation_Window.h"
#include "ImGuiFileDialog.h"
#include "GameInstance.h"
#include "Dummy.h"
#include "Notify.h"

CAnimation_Window::CAnimation_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CAnimation_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	return S_OK;
}

void CAnimation_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Animation", nullptr, m_WindowFlag);

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
	ImGui::Text("");
	ImGui::Separator();
	Notify_InputFileds(m_szNotifyName, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);

	for (_uint partCnt = 0; partCnt < pDummyModel->Get_AnimationPartCount(); partCnt++)
	{
		ImGui::Text("");
		ImGui::Separator();

		CModel::ANIMTYPE ePartCnt = static_cast<CModel::ANIMTYPE>(partCnt);

		Animation_ComboBox(ePartCnt,m_szCurrentItem[ePartCnt], pDummyModel);
		Animation_Action_Button(ePartCnt,pDummyModel, &m_fNotifyActionTime);

		_char szUIName[MAX_PATH] = "Animation##";
		if (pDummyModel->Get_NumAnimations(ePartCnt) != 0)
		{
			sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
			ImGui::SliderFloat(szUIName, pDummyModel->Get_Animation(ePartCnt)->Get_Accmulation_Pointer(), 0, pDummyModel->Get_Animation(ePartCnt)->Get_Duration());
			pDummyModel->Get_Animation(ePartCnt)->Update_KeyFrame_By_Time();
			Add_Notify_Button(ePartCnt, m_szNotifyName, pDummyModel, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);

			Edit_Notify_Button(ePartCnt, pDummyModel);
			Create_Notify_View(ePartCnt, pDummyModel);
			Bone_Tree(ePartCnt, pDummyModel->Get_Bone_Index(0), pDummyModel);

			sprintf_s(szUIName, "RootBone##%d", ePartCnt);
			ImGui::InputInt(szUIName, &m_iRootIndex[ePartCnt]);

			sprintf_s(szUIName, "Set_Root##%d", ePartCnt);
			if (ImGui::Button(szUIName))
			{
				pDummyModel->Set_RootBone(m_iRootIndex[ePartCnt]);
			}

		}
		
		
	}
	ImGui::Text("");
	ImGui::Separator();
	if (pDummyModel->Get_NumAnimations() != 0)
	{
		if (ImGui::Button("Separate"))
		{
			pDummyModel->Separate_Animation();
		}
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
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Add_GameObject(LEVEL_TOOL, TEXT("Prototype_GameObject_Dummy"), TEXT("Layer_Tool"), TEXT("Dummy"), nullptr)))
		{
			MSG_BOX("Failed to Created Dummy Clone");
		}
		m_pDummyObject = dynamic_cast<CDummy*>(pGameInstance->Find_GameObject_In_Layer(LEVEL_TOOL, TEXT("Layer_Tool"), TEXT("Dummy")));
		Safe_Release(pGameInstance);
	}
}

void CAnimation_Window::OpenFile_Button()
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
			if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, m_vecModelList_t[m_iMaxModelIndex].c_str(),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, wszfilePath, PivotMatrix))))
				//return;
			{

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
		//선택된 모델을 읽어오도록 만들어줘야함.
		m_pDummyObject->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
		m_pDummyObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
		CModel* pCurrentModel = dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")));
		_tchar wszAnimationName[MAX_PATH] = {};
		_tchar wszTypeComboName[MAX_PATH] = {};
		lstrcpy(wszAnimationName, pCurrentModel->Get_Animation(0)->Get_AnimationName());
		lstrcpy(wszTypeComboName, TEXT("Speed"));

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
				pDummyModel->Reset_Animation(i, ePartCnt, dynamic_cast<CTransform*>(m_pDummyObject->Find_Component(TEXT("Com_Transform"))));
			}

			ImGui::SameLine();
			sprintf_s(szUIName, "Delete##_Anim%d%s" ,ePartCnt, szAnimationName);
			if (ImGui::SmallButton(szUIName))
			{
				pDummyModel->Delete_Animation(i, ePartCnt);
				//애니메이션 삭제
			}
		}
		ImGui::EndCombo();
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
	_char szUIName[MAX_PATH] = "Notify##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	if (ImGui::Button(szUIName))
	{
		//모델에서 애니메이션에 접근한 뒤 수정해달라해야함.
		_tchar  wszNotifyName[MAX_PATH] = {};
		CharToWChar(szNotifyName, wszNotifyName);
		if (FAILED(pDummyModel->Get_Animation(ePartCnt)->Add_NotifyFrame(*eNotifyKeyFrameType, wszNotifyName, *fNotifyActionTime, *fNotifySpeed)))
		{
		}
	}
}

void CAnimation_Window::Edit_Notify_Button(CModel::ANIMTYPE ePartCnt, CModel* pDummyModel)
{
	_char szUIName[MAX_PATH] = "Edit##Notify##";
	sprintf_s(szUIName, "%s%d", szUIName, ePartCnt);
	if (ImGui::Button(szUIName))
	{
		//현재 선택된 노티파이 수정 만들어줘야함.
		CNotify* pNotify = pDummyModel->Get_Animation(ePartCnt)->Get_Notify_Point();
		pNotify->Edit_Frame(m_iSelectedNotifyIndex, m_eNotifyKeyFrameType, m_fNotifyActionTime, m_fNotifySpeed);
	}
}

void CAnimation_Window::Select_Model()
{
	ImGui::ListBox("AnimModelList", &m_iModelIndex, VectorGetter, static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);
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
		//똑같은 이름의 버튼이 둗개면 에러남
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
		//ImGui::SameLine();
		_char szDeleteButtonName[MAX_PATH] = "Delete##";
		sprintf_s(szDeleteButtonName, "%s%d", szDeleteButtonName, iNotifyCount);
		if (ImGui::Button(szDeleteButtonName))
		{
			//노티파이 삭제 만들어줘야함.
			pNotify->Delete_Frame(iNotifyCount);
		}
	}
}

void CAnimation_Window::Bone_Tree(_uint partCnt, CBone* bone, CModel* pDummyModel)
{
	char szBone_Name[MAX_PATH] = "";
	WCharToChar(bone->Get_Name(), szBone_Name)
	if (ImGui::TreeNode(szBone_Name))
	{
		for (auto child : *pDummyModel->Get_Bone_Vector_Point())
		{
			if (bone->Get_Index() == child->Get_ParentNodeIndex())
			{
				Bone_Tree(partCnt,child, pDummyModel);
			}
		}
		ImGui::TreePop();
	}
	else 
	{
		ImGui::SameLine();
		_char szUIName[MAX_PATH] = "Set_Root##";
		sprintf_s(szUIName, "%s%d##%s", szUIName, partCnt, szBone_Name);
		if (ImGui::SmallButton(szUIName))
		{
			m_iRootIndex[partCnt] = bone->Get_Index();
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
}
