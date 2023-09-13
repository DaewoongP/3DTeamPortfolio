#pragma once

/* =============================================== */
//	[CTime_Manager]
//	- 타이머 관리해주는 매니저
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CTime_Manager final : public CBase
{
	DECLARE_SINGLETON(CTime_Manager)

public:
	typedef struct tagAlarmDesc
	{
		// 반복 유무. false로 설정하면 설정 시간 이후 계속 true를 반환함.
		_bool m_isRepeat = { false };
		// 설정 시간
		_float m_fDuration = { 0.f };
		// 누적 시간 (설정 안해줘도 됨)
		_float m_fTimeAcc = { 0.f };
	}ALARMDESC;

private:
	explicit CTime_Manager() = default;
	virtual ~CTime_Manager() = default;

public:
	HRESULT Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc);

private:
	_float m_fWorldTimeAcc = { 0.f };
	_float m_fWorldTick = { 0.f };
	unordered_map<wstring, ALARMDESC> m_Timers;

public:
	virtual void Free() override {}
};

END