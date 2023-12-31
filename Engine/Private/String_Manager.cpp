#include "..\Public\String_Manager.h"

IMPLEMENT_SINGLETON(CString_Manager)

_char* CString_Manager::Make_Char(const _char* pMakeChar)
{
	_uint iStrLen = strlen(pMakeChar) + 1;
	_char* pNewChar = New _char[iStrLen];
	ZeroMemory(pNewChar, iStrLen);

	strcpy_s(pNewChar, iStrLen, pMakeChar);

	m_Characters.push_back(pNewChar);

	return pNewChar;
}

_tchar* CString_Manager::Make_WChar(const _tchar* pMakeWChar)
{
	_uint iStrLen = lstrlen(pMakeWChar) + 1;
	_tchar* pNewChar = New _tchar[iStrLen];
	ZeroMemory(pNewChar, iStrLen);

	lstrcpy(pNewChar, pMakeWChar);

	m_WideCharacters.push_back(pNewChar);

	return pNewChar;
}

HRESULT CString_Manager::Delete_Char(_char* pChar)
{
	m_Characters.remove(pChar);
	Safe_Delete_Array(pChar);

	return S_OK;
}

HRESULT CString_Manager::Delete_WChar(_tchar* pWChar)
{
	m_WideCharacters.remove(pWChar);
	Safe_Delete_Array(pWChar);

	return S_OK;
}

void CString_Manager::Free()
{
	for (auto& Character : m_Characters)
	{
		Safe_Delete_Array(Character);
	}
	
	m_Characters.clear();

	for (auto& WideCharacter : m_WideCharacters)
	{
		Safe_Delete_Array(WideCharacter);
	}

	m_WideCharacters.clear();
}
