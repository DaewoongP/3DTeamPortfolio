#include "..\Public\ImWindow.h"

CImWindow::CImWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CImWindow::Initialize(ImVec2 vWindowPos, ImVec2 vWindowSize)
{
	m_vWindowPos = vWindowPos;
	m_vWindowSize = vWindowSize;

	return S_OK;
}

void CImWindow::Tick(_float fTimeDelta)
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);

	ImGui::SetNextWindowPos(m_vWindowPos);
	ImGui::SetNextWindowSize(m_vWindowSize);
}

HRESULT CImWindow::Render()
{
	return S_OK;
}

void CImWindow::Free(void)
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
