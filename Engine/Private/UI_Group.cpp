#include "UI_Group.h"
#include "GameInstance.h"
#include "UI.h"

CUI_Group::CUI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Group::CUI_Group(const CUI_Group& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Group::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Group::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	return S_OK;
}

void CUI_Group::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Set_Parent();

	//if (nullptr != m_pParent)
	//{
	//	m_pParent->Tick(fTimeDelta);
	//}

	//for (auto& iter : m_Childs)
	//{
	//	iter->Tick(fTimeDelta);
	//}
}

void CUI_Group::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//if (nullptr != m_pParent)
	//{
	//	m_pParent->Late_Tick(fTimeDelta);
	//}

	//for (auto& iter : m_Childs)
	//{
	//	iter->Late_Tick(fTimeDelta);
	//}
}

void CUI_Group::Set_Parent()
{
	if (nullptr == m_pParent)
		return;

	for (auto& iter : m_Childs)
	{
		iter->Set_Parent(m_pParent);
	}
}

HRESULT CUI_Group::Save(const _tchar* pFileDirectory)
{
	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pFileDirectory, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Save");
		CloseHandle(hFile);
		return E_FAIL;
	}
	// ±×·ìÀÇ Tag
	_tchar wszGroupName[MAX_PATH] = TEXT("");
	lstrcpy(wszGroupName, m_wszUIGroupName);
	DWORD dwStrByte = sizeof(_tchar) * (lstrlen(wszGroupName) + 1);
	WriteFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	WriteFile(hFile, wszGroupName, dwStrByte, &dwByte, nullptr);

	m_pParent->Save(hFile, dwByte);

	_uint iSize = m_Childs.size();
	WriteFile(hFile, &iSize, sizeof iSize, &dwByte, nullptr);
	for (auto& Child : m_Childs)
	{
		Child->Save(hFile, dwByte);
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CUI_Group::Load(const _tchar* pFileDirectory)
{
	_ulong dwByte = 0;
	DWORD dwStrByte;

	HANDLE hFile = CreateFile(pFileDirectory, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	m_wszUIProtoTypeTag = new _tchar[MAX_PATH];

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, m_wszUIGroupName, dwStrByte, &dwByte, nullptr);
		
	m_pParent->Load(hFile, dwByte);

	_uint iSize = m_Childs.size();
	ReadFile(hFile, &iSize, sizeof iSize, &dwByte, nullptr);
	for (auto& Child : m_Childs)
	{
		Child->Load(hFile, dwByte);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CUI_Group::Load(const _tchar* pFileDirectory, CUI* pParent, vector<CUI*>& pChild)
{
	_ulong dwByte = 0;
	DWORD dwStrByte;

	HANDLE hFile = CreateFile(pFileDirectory, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MSG_BOX("Failed Load");
		CloseHandle(hFile);
		return E_FAIL;
	}

	m_wszUIProtoTypeTag = new _tchar[MAX_PATH];

	ReadFile(hFile, &dwStrByte, sizeof(_ulong), &dwByte, nullptr);
	ReadFile(hFile, m_wszUIGroupName, dwStrByte, &dwByte, nullptr);

	pParent->Load(hFile, dwByte);

	_uint iSize = pChild.size();
	ReadFile(hFile, &iSize, sizeof iSize, &dwByte, nullptr);
	for (auto& Child : pChild)
	{
		Child->Load(hFile, dwByte);
	}

	CloseHandle(hFile);
	return S_OK;
}

void CUI_Group::Free()
{
	__super::Free();
	
	Safe_Release(m_pParent);
	
	for (auto& iter : m_Childs)
	{
		Safe_Release(iter);
	}
}
