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

	Animation_ComboBox(m_szCurrentItem, pDummyModel);
	Animation_Action_Button(pDummyModel, &m_fNotifyActionTime);
	ImGui::SliderFloat("Animation", pDummyModel->Get_Animation()->Get_Accmulation_Pointer(), 0, pDummyModel->Get_Animation()->Get_Duration());
	pDummyModel->Get_Animation()->Update_KeyFrame_By_Time();
	Notify_InputFileds(m_szNotifyName, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);
	Add_Notify_Button(m_szNotifyName, pDummyModel, &m_eNotifyKeyFrameType, &m_fNotifyActionTime, &m_fNotifySpeed);

	if (ImGui::Button("Edit##Notify"))
	{
		//���� ���õ� ��Ƽ���� ���� ����������.
		CNotify* pNotify = pDummyModel->Get_Animation()->Get_Notify_Point();
		pNotify->Edit_Frame(m_iSelectedNotifyIndex, m_eNotifyKeyFrameType, m_fNotifyActionTime, m_fNotifySpeed);
	}

	for (_uint iNotifyCount = 0; iNotifyCount < pDummyModel->Get_Animation()->Get_Notify_Point()->Get_NotifyFrameCount(); iNotifyCount++)
	{
		CNotify* pNotify = pDummyModel->Get_Animation()->Get_Notify_Point();
		_char  szNotifyButtonName[MAX_PATH];
		ZEROMEM(szNotifyButtonName);

		WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), szNotifyButtonName);
		sprintf_s(szNotifyButtonName, "%s_%d", szNotifyButtonName, iNotifyCount);
		//�Ȱ��� �̸��� ��ư�� �P���� ������
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
			//��������
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
		sprintf_s(szDeleteButtonName, "%s,%d", szDeleteButtonName, iNotifyCount);
		if (ImGui::Button(szDeleteButtonName))
		{
			//��Ƽ���� ���� ����������.
			pNotify->Delete_Frame(iNotifyCount);
		}
	}
	// ��� ������Ʈ tick render latetick ���ֱ�
	// ������ ���� ������� ������ �� �������. << ��
	// ��ư�� ���� �ӽ� �� ������Ʈ ����  << ��
	// ������ �� �������� �����ؾ���. << ��
	// ������ �� �������� �ӽø��� �߰� << ��
	// ���̿� �������� �߰��� ��� �ִϸ��̼� ����� �� �� �־����. << ��
	// �޺��ڽ� Ŭ���� ���� �ִϸ��̼� ������ �����ؾ��� << ��
	// �ִϸ��̼��� ���� ���൵�� �� �� �־����. << ��
	// ��Ƽ���̰� ����ֵ��� ����������. << ��
	// ��Ƽ���̸� �����ϱ����� ������ ��ǲ â�� �־���� << ��
	// �߰� ��ư�� ������ ��Ƽ���̸� �ִϸ��̼ǿ� �߰� << ��
	// �߰� �� �����������. << ��
	// ��Ƽ���̿� Ű������ ��ŭ �ݺ��ϸ� ��ư�� ���� << �� 
	// ��ư�� ������ ��ư�� ����(�ð�, Ÿ��, ��) ������ << ��
	// �̹� ������ ��Ƽ���̸� ������ �� �־����. << ��
	// Ű������ ���� �־������. << ��
	// ��ư�� ������ �̳� Ÿ�Կ� ���� ���� << ��
	// ��Ƽ���� ����� ��� �߰� << ��
	// �ִϸ��̼� �ð� float���� �ٲ㼭 �巡�׵� �����ϰ� << ��
	// �ִϸ��̼� ��� �����ְ� �� �ִϸ��̼� ��� �ð��� ��Ƽ���� �߰� �����ϰ� << ��
	// ���ǵ� lerp ���༭ �ڿ������� ���������� ����������.
	// �ݶ��̴��� �ִ� ������ �����ϰ� �����ټ��־����.
	// ��ƼŬ�� �������ϰ� ������ �� �־����.
	// �װ� ������ �� �־����.

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

			//���̸� ����
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
		//���õ� ���� �о������ ����������.
		m_pDummyObject->Add_Model_Component(m_vecModelList_t[m_iModelIndex].c_str());
		m_pDummyObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
		CModel* pCurrentModel = dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")));
		_tchar wszAnimationName[MAX_PATH] = {};
		_tchar wszTypeComboName[MAX_PATH] = {};
		lstrcpy(wszAnimationName, pCurrentModel->Get_Animation(0)->Get_AnimationName());
		lstrcpy(wszTypeComboName, TEXT("Speed"));

		WCharToChar(wszAnimationName, m_szCurrentItem);
		WCharToChar(wszTypeComboName, m_szCurrentItemType);
	}
}

void CAnimation_Window::Animation_ComboBox(_char* szCurrentItem, CModel* pDummyModel)
{
	if (ImGui::BeginCombo("AnimComboBox", szCurrentItem))
	{
		_char szAnimationName[MAX_PATH] = "";
		_tchar wszAnimationName[MAX_PATH] = {};
		_uint iAnimCnt = pDummyModel->Get_NumAnimations();
		for (_uint i = 0; i < iAnimCnt; i++)
		{
			ZEROMEM(szAnimationName);
			ZEROMEM(wszAnimationName);

			lstrcpy(wszAnimationName, pDummyModel->Get_Animation(i)->Get_AnimationName());
			WCharToChar(wszAnimationName, szAnimationName);
			bool is_selected = (!strcmp(szCurrentItem, szAnimationName));
			if (ImGui::Selectable(szAnimationName, is_selected))
			{
				strcpy_s(szCurrentItem, sizeof(szAnimationName), szAnimationName);
				ImGui::SetItemDefaultFocus();

				pDummyModel->Reset_Animation(i,CModel::UPPERBODY, dynamic_cast<CTransform*>(m_pDummyObject->Find_Component(TEXT("Com_Transform"))));
			}
		}
		ImGui::EndCombo();
	}
}

void CAnimation_Window::Animation_Action_Button(CModel* pDummyModel, _float* fNotifyActionTime)
{
	_uint iAnimCnt = pDummyModel->Get_NumAnimations();
	if (iAnimCnt == 0)
		return;
	if (pDummyModel->Get_Animation()->Get_Paused_State() ? ImGui::Button("Stop") : ImGui::Button("Play"))
	{
		pDummyModel->Get_Animation()->Set_Pause(!pDummyModel->Get_Animation()->Get_Paused_State());
		*fNotifyActionTime = pDummyModel->Get_Animation()->Get_Accmulation();
	}
}

void CAnimation_Window::Notify_InputFileds(_char* szNotifyName, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed)
{
	ImGui::InputText("NotifyName", szNotifyName, 32);
	const char* items[] = { "Speed","Notify","Sound" };

	if (ImGui::BeginCombo("KeyFrameType", m_szCurrentItemType))
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
	ImGui::InputFloat("ActionTime", fNotifyActionTime);
	if (*eNotifyKeyFrameType == KEYFRAME::KEYFRAMETYPE::KF_SPEED)
	{
		ImGui::InputFloat("Speed", fNotifySpeed);
	}
}

void CAnimation_Window::Add_Notify_Button(_char* szNotifyName, CModel* pDummyModel, KEYFRAME::KEYFRAMETYPE* eNotifyKeyFrameType, _float* fNotifyActionTime, _float* fNotifySpeed)
{
	if (ImGui::Button("Add Notify"))
	{
		//�𵨿��� �ִϸ��̼ǿ� ������ �� �����ش޶��ؾ���.
		_tchar  wszNotifyName[MAX_PATH] = {};
		CharToWChar(szNotifyName, wszNotifyName);
		if (FAILED(pDummyModel->Get_Animation()->Add_NotifyFrame(*eNotifyKeyFrameType, wszNotifyName, *fNotifyActionTime, *fNotifySpeed)))
		{
		}
	}
}

void CAnimation_Window::Edit_Notify_Button()
{
}

void CAnimation_Window::Select_Model()
{
	ImGui::ListBox("AnimModelList", &m_iModelIndex, VectorGetter, static_cast<void*>(&m_vecModelList), (_int)m_vecModelList.size(), 15);
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
