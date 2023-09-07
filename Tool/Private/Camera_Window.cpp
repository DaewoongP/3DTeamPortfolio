#include "..\Public\Camera_Window.h"
#include "GameInstance.h"
#include "ImGuiFileDialog.h"

#include "Main_Camera.h"

CCamera_Window::CCamera_Window(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CImWindow(_pDevice, _pContext)
{
}

HRESULT CCamera_Window::Initialize(ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	if (FAILED(__super::Initialize(_vWindowPos, _vWindowSize)))
		return E_FAIL;

	m_WindowFlag = ImGuiWindowFlags_NoResize;

	m_pCutSceneCameraTool = CCutScene_Camera_Tool::Create(m_pDevice, m_pContext);

	BEGININSTANCE;
	//메인 카메라 추가
	{
		CCamera::CAMERADESC CameraDesc;

		CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
		CameraDesc.m_fFovY = XMConvertToRadians(90.f);
		CameraDesc.m_fNear = 0.1f;
		CameraDesc.m_fFar = 1000.f;

		m_pMainCamera = CMain_Camera::Create(m_pDevice, m_pContext, &CameraDesc);

		m_pMainCamera->Set_MoveSpeed(5.0f);

		pGameInstance->Add_MainCamera((CCamera*)m_pMainCamera);
	}
	ENDINSTANCE;

	return S_OK;
}

void CCamera_Window::Tick(_float _fTimeDelta)
{
	CImWindow::Tick(_fTimeDelta);
	ImGui::Begin("Camera", nullptr, m_WindowFlag);
	Camera_Speed();

	m_pCutSceneCameraTool->Tick(_fTimeDelta);

	ImGui::End();
}

HRESULT CCamera_Window::Render()
{
	return S_OK;
}

void CCamera_Window::Camera_Speed()
{
	_float fMoveSpeed = m_pMainCamera->Get_MoveSeed();

	if (ImGui::DragFloat("Main Camera Speed", &fMoveSpeed, 0.1f, 0.0f, 1000.0f))
	{
		m_pMainCamera->Set_MoveSpeed(fMoveSpeed);
	}
}

CCamera_Window* CCamera_Window::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ImVec2 _vWindowPos, ImVec2 _vWindowSize)
{
	CCamera_Window* pInstance = New CCamera_Window(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_vWindowPos, _vWindowSize)))
	{
		MSG_BOX("Failed Create CCamera_Window");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CCamera_Window::Free(void)
{
	__super::Free();

	Safe_Release(m_pCutSceneCameraTool);
	

}