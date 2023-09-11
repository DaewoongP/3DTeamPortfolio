#pragma once

/* =============================================== */
//	[CTime_Manager]
//	- Ÿ�̸� �������ִ� �Ŵ���
// 
//	�� : �ּ�ȯ
//	�� :
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
		// �ݺ� ����. false�� �����ϸ� ���� �ð� ���� ��� true�� ��ȯ��.
		_bool m_isRepeat = { false };
		// ���� �ð�
		_float m_fDuration = { 0.f };
		// ���� �ð� (���� �����൵ ��)
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