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
			//위 데이터를 기반으로 모델 프로토 생성
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

	// 모든 오브젝트 tick render latetick 꺼주기
	// 데이터 파일 기반으로 프로토 모델 만들어줌. << 완
	// 버튼을 통해 임시 겜 오브젝트 생성  << 완
	// 생성된 겜 오브제를 보관해야함. <<
	// 보관된 겜 오브제에 임시모델을 추가 <<
	// 노티파이용 키프레임 만큼 반복하며 버튼을 생성
	// 버튼을 누르면 버튼의 정보(시간, 타입, 뭐) 보여줌
	// 노티파이 만들기 기능 추가
	// 애니메이션 재생 보여주고 그 애니메이션 재생 시간에 노티파이 추가 가능하게
	// 콜라이더도 애님 툴에서 생성하고 보여줄수있어야함.
	// 파티클도 생성ㅇ하고 보여줄 수 있어야함.
	// 그걸 저장할 수 있어야함.
	//ImGui::TreePop(); // SubNode의 끝

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
