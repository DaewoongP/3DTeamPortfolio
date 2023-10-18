#include "..\Public\Quest_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CQuest_Manager)

_bool CQuest_Manager::Is_Quest_Finished(const _tchar* szQuestTag)
{
	CQuest* pQuest = Find_Quest(szQuestTag);
	if (nullptr == pQuest)
		return false;

	return pQuest->Is_Finished();
}

HRESULT CQuest_Manager::Add_Quest(const _tchar* szQuestTag, class CQuest* pQuest)
{
	m_Quests.emplace(szQuestTag, pQuest);

	return S_OK;
}

void CQuest_Manager::Unlock_Quest(const _tchar* szQuestTag)
{
	m_pCurrentQuest = Find_Quest(szQuestTag);

	if (nullptr == m_pCurrentQuest)
		return;

	Safe_AddRef(m_pCurrentQuest);

	m_pCurrentQuest->Set_State(QUESTSTATE::QUESTSTATE_UNLOCK);
}

void CQuest_Manager::Clear_Quest(const _tchar* szQuestTag)
{
	m_pCurrentQuest->Set_State(QUESTSTATE::QUESTSTATE_CLEAR);
	Safe_Release(m_pCurrentQuest);
}

CQuest* CQuest_Manager::Find_Quest(const _tchar* szQuestTag)
{
	auto Pair = find_if(m_Quests.begin(), m_Quests.end(), CTag_Finder(szQuestTag));

	if (m_Quests.end() == Pair)
		return nullptr;

	return Pair->second;
}

void CQuest_Manager::Free()
{
	for (auto& Pair : m_Quests)
	{
		Safe_Release(Pair.second);
	}

	m_Quests.clear();

	Safe_Release(m_pCurrentQuest);
}
