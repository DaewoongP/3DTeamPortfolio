#include "..\Public\Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

void CTimer_Manager::Set_SlowTime(const _tchar* pTimerTag, _float fSlowPower, _float fTime)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Set_SlowedTime(fSlowPower, fTime);
}

_float CTimer_Manager::Get_TimeAcc(const wstring& wstrTimerTag) const
{
	auto iter = m_Timers.find(wstrTimerTag);
	if (m_Timers.end() == iter)
		return 0.f;

	return iter->second.m_fTimeAcc;
}

_float CTimer_Manager::Get_QueryTimeDelta(const _tchar* pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Tick(const _float& fTimeDelta)
{
	m_fWorldTimeAcc += fTimeDelta;

	m_fWorldTick = fTimeDelta;

	for (auto& Pair : m_Timers)
		Pair.second.m_fTimeAcc += fTimeDelta;
}

HRESULT CTimer_Manager::Add_Timer(const wstring& _wstrTimerTag, _bool _isRepeat, const _float fDuration)
{
	auto Timer = m_Timers.find(_wstrTimerTag);

	if (m_Timers.end() != Timer)
	{
		MSG_BOX("[CTime_Manager] Tag is duplicate Tag");
		return E_FAIL;
	}

	ALARMDESC AlarmDesc;
	AlarmDesc.m_fDuration = fDuration;
	AlarmDesc.m_isRepeat = _isRepeat;
	AlarmDesc.m_fTimeAcc = 0.f;

	m_Timers.emplace(_wstrTimerTag, AlarmDesc);

	return S_OK;
}

HRESULT CTimer_Manager::Remove_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CTime_Manager] Tag is not in Container");
		return E_FAIL;
	}

	m_Timers.erase(Timer);

	return S_OK;
}

HRESULT CTimer_Manager::Reset_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CTime_Manager] Tag is not in Container");
		return E_FAIL;
	}

	Timer->second.m_fTimeAcc = 0.f;

	return S_OK;
}

_bool CTimer_Manager::Check_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CTime_Manager] Tag is not in map");
		return false;
	}

	if (Timer->second.m_fDuration <= Timer->second.m_fTimeAcc)
	{
		if (true == Timer->second.m_isRepeat)
			Timer->second.m_fTimeAcc = 0.f;

		return true;
	}

	return false;
}

HRESULT CTimer_Manager::Add_QueryTimer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;
	
	pTimer = CTimer::Create();

	NULL_CHECK_RETURN_MSG(pTimer, E_FAIL, TEXT("Failed Create Timer"))

	m_umapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void CTimer_Manager::Tick_QueryTimer(const _tchar * pTimerTag)
{
	CTimer*		pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Tick();
}

CTimer* CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	std::lock_guard<std::mutex> lock(mtx);
	auto		iter = find_if(m_umapTimers.begin(), m_umapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_umapTimers.end())
		return nullptr;

	return iter->second;
}

HRESULT	CTimer_Manager::Clear()
{
	m_Timers.clear();

	return S_OK;
}

void CTimer_Manager::Free(void)
{
	for (auto& Pair : m_umapTimers)
	{
		Safe_Release(Pair.second);
	}

	m_umapTimers.clear();
}
