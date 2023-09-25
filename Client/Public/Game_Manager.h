#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include <any>

BEGIN(Client)

class CGame_Manager final : public CBase
{
	DECLARE_SINGLETON(CGame_Manager)
private:
	HRESULT Add_Desc(const _tchar* pDescTag, void* pArg);

private:
	unordered_map<const _tchar*, void*> m_GameInfos;

public:
	virtual void Free() override;
};

END