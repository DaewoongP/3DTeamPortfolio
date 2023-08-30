#include "..\Public\Animation_Window.h"
#include "ImGuiFileDialog.h"
#include "GameInstance.h"
#include "CDummy.h"
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
			CharToWChar(strFilePathName.c_str(), wszfilePath);
			//���⼭ ������ �� �±״� ��ϵž��Ѵ�.
			//�� �����͸� ������� �� ������ ����
			_float4x4 PivotMatrix = XMMatrixIdentity();
			PivotMatrix = XMMatrixIdentity();
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Dummy01"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, wszfilePath, PivotMatrix))))
				return;
			Safe_Release(pGameInstance);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	if (ImGui::Button("AddModelToDummy"))
	{
		//���õ� ���� �о������ ����������.
		m_pDummyObject->Add_Model_Component(TEXT("Prototype_Component_Model_Dummy01"));
		m_pDummyObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxAnimMesh"));
	}

	if (m_pDummyObject != nullptr)
	{
		//���̰� �ִϸ��̼��� �������ִ� ģ����
		//���� �ƴ϶��
		CModel* pDummyModel = dynamic_cast<CModel*>(m_pDummyObject->Find_Component(TEXT("Com_Model")));

		if (pDummyModel != nullptr)
		{
			static _char szCurrentItem[MAX_PATH];
			
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
						strcpy_s(szCurrentItem, szAnimationName);
						ImGui::SetItemDefaultFocus();
						pDummyModel->Set_CurrentAnimIndex(i);
					}
				}
				ImGui::EndCombo();
			}
			_uint iAnimCnt = pDummyModel->Get_NumAnimations();
			if (iAnimCnt == 0)
				return;
			if (pDummyModel->Get_Animation()->Get_Paused_State() ? ImGui::Button("Stop") : ImGui::Button("Play"))
			{
				pDummyModel->Get_Animation()->Set_Pause(!pDummyModel->Get_Animation()->Get_Paused_State());
			}

			_float AnimPlaygague = pDummyModel->Get_Animation()->Get_Ratio_Accumulation_Duration();
			ImGui::ProgressBar(AnimPlaygague, ImVec2(-1, 0));
			

			static _float fNotifyActionTime;
			static _char  szNotifyName[MAX_PATH];
			static KEYFRAME::KEYFRAMETYPE eNotifyKeyFrameType;

			//��Ƽ���� �̸� �����ϴ� �ع�
			ImGui::InputText("NotifyName", szNotifyName, 32);
			//��Ƽ���� �̳� Ÿ�� �Է½�Ű�� �ع�
			const char* items[] = { "Speed","Notify","Sound"};
			static const char* szCurrentItemType;
			if (ImGui::BeginCombo("KeyFrameType", szCurrentItemType))
			{
				for (int i = 0; i < IM_ARRAYSIZE(items); i++)
				{
					bool is_selected = (items[i] == szCurrentItemType);
					if (ImGui::Selectable(items[i], is_selected))
					{
						eNotifyKeyFrameType = static_cast<KEYFRAME::KEYFRAMETYPE>(i);
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			//��Ƽ���� �ð��� �Է½�Ű�� �ع�
			ImGui::InputFloat("ActionTime", &fNotifyActionTime);
			//��Ƽ���� �߰��ϴ� ��ư
			if (ImGui::Button("Add Notify"))
			{
				//�𵨿��� �ִϸ��̼ǿ� ������ �� �����ش޶��ؾ���.
				_tchar  wszNotifyName[MAX_PATH] = {};
				CharToWChar(szNotifyName, wszNotifyName);
				if (FAILED(pDummyModel->Get_Animation()->Add_NotifyFrame(eNotifyKeyFrameType, wszNotifyName, fNotifyActionTime)))
				{
				}
			}

			_uint iMaxNotifyCount = pDummyModel->Get_Animation()->Get_Notify_Point()->Get_NotifyFrameCount();
			for (_uint iNotifyCount = 0; iNotifyCount < iMaxNotifyCount; iNotifyCount++)
			{
				CNotify* pNotify = pDummyModel->Get_Animation()->Get_Notify_Point();
				_char  szNotifyButtonName[MAX_PATH];
				ZEROMEM(szNotifyButtonName);
				
				WCharToChar(pNotify->Find_Frame_Key(iNotifyCount), szNotifyButtonName);
				sprintf_s(szNotifyButtonName,"%s_%d", szNotifyButtonName, iNotifyCount);
				//�Ȱ��� �̸��� ��ư�� �P���� ������
				KEYFRAME::KEYFRAMETYPE eNotifyType =  pNotify->Find_Frame(iNotifyCount)->eKeyFrameType;
				;
				if (ImGui::ColorButton(szNotifyButtonName, 
					(eNotifyType==KEYFRAME::KF_NOTIFY) ? (ImVec4(0.7f,0.f,0.f,1)):
					((eNotifyType == KEYFRAME::KF_SOUND) ? (ImVec4(0.0f, 0.7f, 0.f, 1)) : 
						(ImVec4(0.0f, 0.f, 0.7f, 1)))))
				{

				}
				ImGui::SameLine();
				if (ImGui::Button(szNotifyButtonName))
				{

				}

			}
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
	// ��ư�� ������ ��ư�� ����(�ð�, Ÿ��, ��) ������
	// ��ư�� ������ �̳� Ÿ�Կ� ���� ���� << ��
	// ��Ƽ���� ����� ��� �߰�
	// �ִϸ��̼� ��� �����ְ� �� �ִϸ��̼� ��� �ð��� ��Ƽ���� �߰� �����ϰ�
	// �ݶ��̴��� �ִ� ������ �����ϰ� �����ټ��־����.
	// ��ƼŬ�� �������ϰ� ������ �� �־����.
	// �װ� ������ �� �־����.

	ImGui::End();
}

HRESULT CAnimation_Window::Render()
{
	return S_OK;
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
