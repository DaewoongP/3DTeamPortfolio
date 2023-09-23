#pragma once

/* =============================================== */
// 
//	정 : 박대웅
//	부 : 주성환
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
		// 반복 유무. false로 설정하면 설정 시간 이후 계속 true를 반환함.
		_bool m_isRepeat = { false };
		// 설정 시간
		_float m_fDuration = { 0.f };
		// 누적 시간 (설정 안해줘도 됨)
		_float m_fTimeAcc = { 0.f };
	}ALARMDESC;

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	/* 현재 월드 누적시간을 반환*/
	const _float& Get_World_TimeAcc() const { return m_fWorldTimeAcc; }
	/* 현재 월드 틱을 반환 */
	_float Get_World_Tick() const { return m_fWorldTick; }
	/* 기존에 추가한 타이머의 누적시간을 반환 */
	_float Get_TimeAcc(const wstring& wstrTimerTag) const;
	// 현재의 TimeDelta 반환
	_float Get_QueryTimeDelta(const _tchar* pTimerTag);

public:
	/* (주의) 등록 시킨 타이머를 전체 순회 하면서 m_fTimeAcc를 증가시킴	*/
	void Tick(const _float& fTimeDelta);

public:
	// 
	HRESULT	Add_Timer(const wstring& wstrTimerTag, _bool m_isRepeat = false, const _float fDuration = 0.f);
	HRESULT Remove_Timer(const wstring& wstrTimerTag);
	/* 타이머의 m_fTimeAcc를 0.f로 초기화 */
	HRESULT Reset_Timer(const wstring& wstrTimerTag);
	/* 실질적인 시간 체크 함수. 해당 태그의 시간이 Duration보다 작은 경우 false를 반환.
	   크거나 같으면 true 반환 */
	_bool Check_Timer(const wstring& wstrTimerTag);
	/* 월드 누적시간 초기화 (사용할 경우 팀원들한테 미리 얘기하세요) */
	void Reset_World_TimeAcc() { m_fWorldTimeAcc = 0.f; }

	// 타이머 태그로 타이머 생성
	HRESULT		Add_QueryTimer(const _tchar* pTimerTag);
	// 타이머를 틱에 맞춰 루프시킴. 
	// !!! 주의 (등록시킨 타이머가 아님) 주의 !!!
	void		Tick_QueryTimer(const _tchar* pTimerTag);

	// 타이머 레벨 클리어
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
