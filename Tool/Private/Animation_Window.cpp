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
		// �������� ���̾˷α׷� ������ ������ ������.
		// ������ ���� ������� ������ ��, ������ �׿������� �������.
		// �����並 ������� �ӽø��� ������.
		// ��Ƽ���̿� Ű������ ��ŭ �ݺ��ϸ� ��ư�� ����
		// ��ư�� ������ ��ư�� ����(�ð�, Ÿ��, ��) ������
		// ��Ƽ���� ����� ��� �߰�
		// �ִϸ��̼� ��� �����ְ� �� �ִϸ��̼� ��� �ð��� ��Ƽ���� �߰� �����ϰ�
		// �ݶ��̴��� �ִ� ������ �����ϰ� �����ټ��־����.
		// ��ƼŬ�� �������ϰ� ������ �� �־����.
		// �װ� ������ �� �־����.
		//ImGui::TreePop(); // SubNode�� ��
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
