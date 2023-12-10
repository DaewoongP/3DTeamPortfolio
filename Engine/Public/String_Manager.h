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
	// �����迭�� ���ڿ��� �Ҵ����ִ� �Լ�
	// ������ ����Ǳ� ������ ����ִ� ���ڿ��Դϴ�.
	_char* Make_Char(const _char* pMakeChar);
	// �����迭�� ���ڿ��� �Ҵ����ִ� �Լ�
	// ������ ����Ǳ� ������ ����ִ� ���ڿ��Դϴ�.
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