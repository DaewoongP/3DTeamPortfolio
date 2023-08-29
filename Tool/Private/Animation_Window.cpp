#include "..\Public\Animation_Window.h"

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

	if (ImGui::TreeNode("MainModule"))
	{
		// 오픈파일 다이알로그로 데이터 파일을 열어줌.
		// 데이터 파일 기반으로 프로토 모델, 프로토 겜오브제를 만들어줌.
		// 프로토를 기반으로 임시모델을 생성함.
		// 노티파이용 키프레임 만큼 반복하며 버튼을 생성
		// 버튼을 누르면 버튼의 정보(시간, 타입, 뭐) 보여줌
		// 노티파이 만들기 기능 추가
		// 애니메이션 재생 보여주고 그 애니메이션 재생 시간에 노티파이 추가 가능하게
		// 콜라이더도 애님 툴에서 생성하고 보여줄수있어야함.
		// 파티클도 생성ㅇ하고 보여줄 수 있어야함.
		// 그걸 저장할 수 있어야함.
		//ImGui::TreePop(); // SubNode의 끝
	}

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
