#include "Dummy_UI_Group.h"
#include "GameInstance.h"
#include "Dummy_UI.h"

CDummy_UI_Group::CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_UI_Group::CDummy_UI_Group(const CDummy_UI_Group& rhs)
	: CGameObject(rhs)
{
}

void CDummy_UI_Group::Set_Parent()
{
	if (nullptr == m_pParent)
		return;

	for (auto& iter : m_Childs)
	{
		iter->Set_Parent(m_pParent);
	}
}

HRESULT CDummy_UI_Group::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_UI_Group::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	lstrcpy(m_wszUIGroupName, (_tchar*)pArg);
	
	return S_OK;
}

void CDummy_UI_Group::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Set_Parent();

	if (nullptr != m_pParent)
	{
		m_pParent->Tick(fTimeDelta);
	}

	for (auto& iter : m_Childs)
	{
		iter->Tick(fTimeDelta);
	}


}

void CDummy_UI_Group::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pParent)
	{
		m_pParent->Late_Tick(fTimeDelta);
	}

	for (auto& iter : m_Childs)
	{
		iter->Late_Tick(fTimeDelta);
	}
}

HRESULT CDummy_UI_Group::Save(const _tchar* pFileDirectory)
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

HRESULT CDummy_UI_Group::Load(const _tchar* pFileDirectory)
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

void CDummy_UI_Group::Delete(CDummy_UI* pDeleteUI)
{
	if (wcscmp(m_pParent->Get_Tag(), pDeleteUI->Get_Tag()) == 0)
	{
		m_pParent = nullptr;
		return;
	}

	_int iIndex = 0;
	for (auto& iter : m_Childs)
	{
		if (wcscmp(iter->Get_Tag(), pDeleteUI->Get_Tag()) == 0)
		{
			m_Childs.erase(m_Childs.begin() + iIndex);
			return;
		}
	}

}

void CDummy_UI_Group::Clear()
{
	if (nullptr != m_pParent)
	{
		m_pParent = nullptr;
	}

	if (m_Childs.size() > 0)
	{
		m_Childs.clear();
	}
}

CDummy_UI_Group* CDummy_UI_Group::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_UI_Group* pInstance = New CDummy_UI_Group(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_UI_Group");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_UI_Group::Clone(void* pArg)
{
	CDummy_UI_Group* pInstance = New CDummy_UI_Group(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_UI_Group");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_UI_Group::Free()
{
	__super::Free();

	Safe_Release(m_pParent);

	for (auto& iter : m_Childs)
	{
		Safe_Release(iter);
	}
}
