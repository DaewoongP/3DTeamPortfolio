#pragma once

/* =============================================== */
//	[CDecorator]
//	- ������ ��尡 �����ϱ����� ���� üũ �� ���ǽ�
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CDecorator : public CBase
{
protected:
	explicit CDecorator() = default;
	virtual ~CDecorator() = default;

public:
	HRESULT Initialize(function<_bool(class CBlackBoard*)> Func) {
		m_Function = Func;
		return S_OK;
	}
	HRESULT Bind_Decoration(function<_bool(class CBlackBoard*)> Func);
	_bool Is_Execute(class CBlackBoard* pBlackBoard);

protected:
	function<_bool(class CBlackBoard*)> m_Function = { nullptr };

public:
	static CDecorator* Create(function<_bool(class CBlackBoard*)> Func);
	virtual void Free() override {}
};

END