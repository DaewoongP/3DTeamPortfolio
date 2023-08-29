#include "..\Public\Effect_Window.h"

CEffect_Window::CEffect_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImWindow(pDevice, pContext)
{
}

HRESULT CEffect_Window::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	if (FAILED(__super::Initialize(vWindowPos, vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;
	
	m_fDuration = 5.0f;
	m_isLooping = true;
	m_isPrewarm = false;
	m_fStartDelay = 0.f;
	m_fStartLifeTime = 5.f;
	m_fStartSpeed = 5.f;

	return S_OK;
}

void CEffect_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	ImGui::Begin("Effect", nullptr, m_WindowFlag);

	if (ImGui::TreeNode("MainModule"))
	{
		ImGui::DragFloat("Duration", &m_fDuration, 0.01f);
		//Clamp(m_fDuration, 0.f, FLT_MAX);

		ImGui::Checkbox("Looping", &m_isLooping);
		ImGui::Checkbox("Prewarm", &m_isPrewarm);
		ImGui::DragFloat("StartDelay", &m_fStartDelay, 0.01f);
		ImGui::DragFloat("StartLifeTime", &m_fStartLifeTime, 0.01f);
		ImGui::DragFloat("StartSpeed", &m_fStartSpeed, 0.01f);
		ImGui::TreePop(); // SubNodeÀÇ ³¡
	}

	ImGui::End();

	//ImGui::ShowDemoWindow();
}

HRESULT CEffect_Window::Render()
{
	return S_OK;
}

CEffect_Window* CEffect_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	CEffect_Window* pInstance = New CEffect_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vWindowPos, vWindowSize)))
	{
		MSG_BOX("Failed Create CEffect_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CEffect_Window::Free(void)
{
	__super::Free();
}
