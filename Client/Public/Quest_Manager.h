#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Quest_Save_Fig.h"
#include "Quest_Potion.h"
#include "Quest_Town.h"
#include "Quest_Secret.h"
#include "Quest_Bone.h"

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
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);

private:
	_umap<const _tchar*, CQuest*>	m_Quests;

public:
	CQuest* Find_Quest(const _tchar* szQuestTag);

public:
	virtual void Free() override;
};

END