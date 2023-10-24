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
	CQuest* pQuest = Find_Quest(szQuestTag);

	QUESTSTATE state =pQuest->Get_State();

	if (nullptr == pQuest || QUESTSTATE_CLEAR == state )
		return;

	pQuest->Set_State(QUESTSTATE::QUESTSTATE_UNLOCK);
}

void CQuest_Manager::Clear_Quest(const _tchar* szQuestTag)
{
	CQuest* pQuest = Find_Quest(szQuestTag);
	if (nullptr == pQuest)
		return;

	pQuest->Clear_Quest();
	pQuest->Set_State(QUESTSTATE::QUESTSTATE_CLEAR);
}

void CQuest_Manager::Tick(_float fTimeDelta)
{
	for (auto& Pair : m_Quests)
	{
		Pair.second->Tick(fTimeDelta);
	}
}

void CQuest_Manager::Late_Tick(_float fTimeDelta)
{
}

CQuest* CQuest_Manager::Find_Quest(const _tchar* szQuestTag)
{
	std::lock_guard<std::mutex> lock(mtx);
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
}
