#include "..\Public\Animation_Window.h"
#include "ImGuiFileDialog.h"
#include "Model.h"

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
		//m_pDummyObject = pGameInstance->
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
			_tchar wszfileName[MAX_PATH] = {};
			CharToWChar(strFilePathName.c_str(), wszfilePath);
			CharToWChar(fileName, wszfileName);
			_tchar wszModelTag[MAX_PATH] = TEXT("Prototype_Component_Model_");
			lstrcat(wszModelTag, wszfileName);
			//�� �����͸� ������� �� ������ ����
			_float4x4 PivotMatrix = XMMatrixIdentity();
			PivotMatrix = XMMatrixIdentity();
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_Prototype_Component(LEVEL_TOOL, wszModelTag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, wszfilePath, PivotMatrix))))
				return;
			Safe_Release(pGameInstance);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// ��� ������Ʈ tick render latetick ���ֱ�
	// ������ ���� ������� ������ �� �������. << ��
	// ��ư�� ���� �ӽ� �� ������Ʈ ����  << ��
	// ������ �� �������� �����ؾ���. <<
	// ������ �� �������� �ӽø��� �߰� <<
	// ��Ƽ���̿� Ű������ ��ŭ �ݺ��ϸ� ��ư�� ����
	// ��ư�� ������ ��ư�� ����(�ð�, Ÿ��, ��) ������
	// ��Ƽ���� ����� ��� �߰�
	// �ִϸ��̼� ��� �����ְ� �� �ִϸ��̼� ��� �ð��� ��Ƽ���� �߰� �����ϰ�
	// �ݶ��̴��� �ִ� ������ �����ϰ� �����ټ��־����.
	// ��ƼŬ�� �������ϰ� ������ �� �־����.
	// �װ� ������ �� �־����.
	//ImGui::TreePop(); // SubNode�� ��

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
