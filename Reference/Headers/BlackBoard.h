#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBlackBoard final : public CBase
{
private:
	explicit CBlackBoard() = default;
	explicit CBlackBoard(const CBlackBoard & rhs)
		: CBase(rhs)
		, m_Types(rhs.m_Types) { }
	virtual ~CBlackBoard() = default;

public:
	HRESULT Set_Type(IN const string & strTypename, IN any Type)
	{
		auto iter = m_Types.find(strTypename);

		if (iter == m_Types.end())
			m_Types.emplace(strTypename, Type);

		else
			iter->second = Type;

		return S_OK;
	}

	template<typename T>
	HRESULT Get_Type(IN const string & strTypename, OUT T & Out)
	{
		auto iter = m_Types.find(strTypename);
		if (iter == m_Types.end())
		{
			string strErrorMSG = strTypename + " is Not in BlackBoard";
			MessageBoxA(nullptr, strErrorMSG.c_str(), "System Message", MB_OK);
			return E_FAIL;
		}

		Out = any_cast<T>(iter->second);

		return S_OK;
	}

private:
	unordered_map<string, any>	m_Types;

public:
	static CBlackBoard* Create() {
		return new CBlackBoard();
	}
	virtual void Free() override {}
};

END