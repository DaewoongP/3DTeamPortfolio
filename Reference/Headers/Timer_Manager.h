#pragma once

/* =============================================== */
// 
//	�� : �ڴ��
//	�� : �ּ�ȯ
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

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
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	/* ���� ���� �����ð��� ��ȯ*/
	const _float& Get_World_TimeAcc() const { return m_fWorldTimeAcc; }
	/* ���� ���� ƽ�� ��ȯ */
	_float Get_World_Tick() const { return m_fWorldTick; }
	/* ������ �߰��� Ÿ�̸��� �����ð��� ��ȯ */
	_float Get_TimeAcc(const wstring& wstrTimerTag) const;
	// ������ TimeDelta ��ȯ
	_float Get_QueryTimeDelta(const _tchar* pTimerTag);

public:
	/* (����) ��� ��Ų Ÿ�̸Ӹ� ��ü ��ȸ �ϸ鼭 m_fTimeAcc�� ������Ŵ	*/
	void Tick(const _float& fTimeDelta);

public:
	// 
	HRESULT	Add_Timer(const wstring& wstrTimerTag, _bool m_isRepeat = false, const _float fDuration = 0.f);
	HRESULT Remove_Timer(const wstring& wstrTimerTag);
	/* Ÿ�̸��� m_fTimeAcc�� 0.f�� �ʱ�ȭ */
	HRESULT Reset_Timer(const wstring& wstrTimerTag);
	/* �������� �ð� üũ �Լ�. �ش� �±��� �ð��� Duration���� ���� ��� false�� ��ȯ.
	   ũ�ų� ������ true ��ȯ */
	_bool Check_Timer(const wstring& wstrTimerTag);
	/* ���� �����ð� �ʱ�ȭ (����� ��� ���������� �̸� ����ϼ���) */
	void Reset_World_TimeAcc() { m_fWorldTimeAcc = 0.f; }

	// Ÿ�̸� �±׷� Ÿ�̸� ����
	HRESULT		Add_QueryTimer(const _tchar* pTimerTag);
	// Ÿ�̸Ӹ� ƽ�� ���� ������Ŵ. 
	// !!! ���� (��Ͻ�Ų Ÿ�̸Ӱ� �ƴ�) ���� !!!
	void		Tick_QueryTimer(const _tchar* pTimerTag);

	// Ÿ�̸� ���� Ŭ����
	HRESULT		Clear();

private:
	_float m_fWorldTimeAcc = { 0.f };
	_float m_fWorldTick = { 0.f };
	unordered_map<wstring, ALARMDESC> m_Timers;

private:
	class CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	unordered_map<const _tchar*, class CTimer*>		m_umapTimers;

public:
	virtual	void Free() override;
};

END
