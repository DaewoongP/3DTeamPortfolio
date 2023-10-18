#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CQuest_Manager final : public CBase
{
	DECLARE_SINGLETON(CQuest_Manager)
public:
	enum QUESTSTATE
	{
		QUESTIONMARK,
		UNLOCK,
		CLEAR,
		QUESTSTATE_END
	};

	typedef struct tagQuest
	{
		_tchar			szQuestTag[MAX_PATH] = TEXT("");
		QUESTSTATE		eQuestState = { QUESTSTATE_END };
		_uint			iRewardMoney = { 0 };
	}QUESTDESC;

public:
	explicit CQuest_Manager() = default;
	virtual ~CQuest_Manager() = default;
	
public:
	_uint Get_QuestReward();
	QUESTDESC Get_CurrentQuest();

public:
	HRESULT Add_Quest(QUESTDESC QuestDesc);
	// 현재 진행할 퀘스트를 바인딩한다.
	HRESULT Set_Quest(const _tchar* szQuestTag);
	HRESULT Update_QuestState(QUESTSTATE eState);

private:
	QUESTDESC		m_CurrentQuest;
	list<QUESTDESC>	m_Quests;

private:
	QUESTDESC& Find_Quest(const _tchar* pQuestTag);

public:
	virtual void Free() override;
};

END