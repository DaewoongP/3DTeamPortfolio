#pragma once
#include "Base.h"

BEGIN(Engine)

class CString_Manager final : public CBase
{
	DECLARE_SINGLETON(CString_Manager)

private:
	explicit CString_Manager() = default;
	virtual ~CString_Manager() = default;

public:
	// 동적배열로 문자열을 할당해주는 함수
	// 게임이 종료되기 전까지 살아있는 문자열입니다.
	_char* Make_Char(const _char* pMakeChar);
	// 동적배열로 문자열을 할당해주는 함수
	// 게임이 종료되기 전까지 살아있는 문자열입니다.
	_tchar* Make_WChar(const _tchar* pMakeWChar);

	HRESULT Delete_Char(_char* pChar);
	HRESULT Delete_WChar(_tchar* pWChar);

private:
	list<_char*> m_Characters;
	list<_tchar*> m_WideCharacters;

public:
	virtual void Free() override;
};

END