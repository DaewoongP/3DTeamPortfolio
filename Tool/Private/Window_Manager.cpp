#include "..\Public\Window_Manager.h"
#include "ImWindow.h"

IMPLEMENT_SINGLETON(CWindow_Manager)

HRESULT CWindow_Manager::Initialize()
{
	return S_OK;
}

void CWindow_Manager::Tick(_float fTimeDelta)
{
	for (auto& pair : m_ImWindows)
		pair.second->Tick(fTimeDelta);
}

HRESULT CWindow_Manager::Render()
{
	for (auto& pair : m_ImWindows)
	{
		if (FAILED(pair.second->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWindow_Manager::Add_Window(const _tchar* pWindowTag, CImWindow* pWindow)
{
	if (nullptr == pWindow)
		return E_FAIL;

	m_ImWindows.emplace(pWindowTag, pWindow);

	return S_OK;
}

CImWindow* CWindow_Manager::Find_Window(const _tchar* pWindowTag)
{
	auto pair = find_if(m_ImWindows.begin(), m_ImWindows.end(), CTag_Finder(pWindowTag));

	if (m_ImWindows.end() == pair)
		return nullptr;

	return pair->second;
}

void CWindow_Manager::Free(void)
{
	for (auto& pair : m_ImWindows)
		Safe_Release(pair.second);

	m_ImWindows.clear();
}
