#include "..\Public\String_Manager.h"

IMPLEMENT_SINGLETON(CString_Manager)

_char* CString_Manager::Make_Char(const _char* pMakeChar)
{
	_char* pNewChar = New _char[MAX_PATH];
	ZeroMemory(pNewChar, MAX_PATH);

	strcpy_s(pNewChar, MAX_PATH, pMakeChar);

	m_Characters.push_back(pNewChar);

	return pNewChar;
}

_tchar* CString_Manager::Make_WChar(const _tchar* pMakeWChar)
{
	_tchar* pNewChar = New _tchar[MAX_PATH];
	ZeroMemory(pNewChar, MAX_PATH);

	lstrcpy(pNewChar, pMakeWChar);

	m_WideCharacters.push_back(pNewChar);

	return pNewChar;
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
