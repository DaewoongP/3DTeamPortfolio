#pragma once

/* =============================================== */
//	[CDecorator]
//	- 비헤비어 노드가 실행하기전에 먼저 체크 할 조건식
// 
//	정 : 주성환
//	부 :
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