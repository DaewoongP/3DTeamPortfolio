#include "..\Public\Quest_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CQuest_Manager)

_uint CQuest_Manager::Get_QuestReward()
{
	return m_CurrentQuest.iRewardMoney;
}

CQuest_Manager::QUESTDESC CQuest_Manager::Get_CurrentQuest()
{
	return m_CurrentQuest;
}

HRESULT CQuest_Manager::Add_Quest(QUESTDESC QuestDesc)
{
	m_Quests.push_back(QuestDesc);

	return S_OK;
}

HRESULT CQuest_Manager::Set_Quest(const _tchar* szQuestTag)
{
	QUESTDESC QuestDesc = Find_Quest(szQuestTag);

	if (QUESTIONMARK != QuestDesc.eQuestState)
		return E_FAIL;

	lstrcpy(m_CurrentQuest.szQuestTag, QuestDesc.szQuestTag);
	m_CurrentQuest.iRewardMoney = QuestDesc.iRewardMoney;
	m_CurrentQuest.eQuestState = UNLOCK;

	return S_OK;
}

HRESULT CQuest_Manager::Update_QuestState(QUESTSTATE eState)
{
	m_CurrentQuest.eQuestState = eState;

	return S_OK;
}

CQuest_Manager::QUESTDESC& CQuest_Manager::Find_Quest(const _tchar* pQuestTag)
{
	auto iter = find_if(m_Quests.begin(), m_Quests.end(), [pQuestTag](QUESTDESC QuestDesc) {

		if (!lstrcmp(QuestDesc.szQuestTag, pQuestTag))
		{
			return true;
		}
		return false;
	});

	return (*iter);
}

void CQuest_Manager::Free()
{
}
