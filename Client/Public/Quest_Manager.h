#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Quest_Save_Fig.h"

BEGIN(Client)

class CQuest_Manager final : public CBase
{
	DECLARE_SINGLETON(CQuest_Manager)

public:
	explicit CQuest_Manager() = default;
	virtual ~CQuest_Manager() = default;

public:
	_bool Is_Quest_Finished(const _tchar* szQuestTag);
	
public:
	HRESULT Add_Quest(const _tchar* szQuestTag, class CQuest* pQuest);
	void Unlock_Quest(const _tchar* szQuestTag);
	void Clear_Quest(const _tchar* szQuestTag);

private:
	CQuest*							m_pCurrentQuest = { nullptr };
	_umap<const _tchar*, CQuest*>	m_Quests;

private:
	CQuest* Find_Quest(const _tchar* szQuestTag);

public:
	virtual void Free() override;
};

END