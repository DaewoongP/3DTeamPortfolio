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
	/* ���� ���� �����ð��� ��ȯ*/
	const _float& Get_World_TimeAcc() const { return m_fWorldTimeAcc; }
	/* ������ �߰��� Ÿ�̸��� �����ð��� ��ȯ */
	_float Get_TimeAcc(const wstring& wstrTimerTag) const;

public:
	/* (����) ��� ��Ų Ÿ�̸Ӹ� ��ü ��ȸ �ϸ鼭 m_fTimeAcc�� ������Ŵ	*/
	void Tick(const _float& fTimeDelta);

public:
	HRESULT Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc);
	HRESULT Remove_Timer(const wstring& wstrTimerTag);
	/* Ÿ�̸��� m_fTimeAcc�� 0.f�� �ʱ�ȭ */
	HRESULT Reset_Timer(const wstring& wstrTimerTag);
	/* �������� �ð� üũ �Լ�. �ش� �±��� �ð��� Duration���� ���� ��� false�� ��ȯ.
	   ũ�ų� ������ true ��ȯ */
	_bool Check_Timer(const wstring& wstrTimerTag);
	/* ���� �����ð� �ʱ�ȭ (����� ��� ���������� �̸� ����ϼ���) */
	void Reset_World_TimeAcc() { m_fWorldTimeAcc = 0.f; }

private:
	_float m_fWorldTimeAcc = { 0.f };
	unordered_map<wstring, ALARMDESC> m_Timers;

public:
	virtual void Free() override {}
};

END