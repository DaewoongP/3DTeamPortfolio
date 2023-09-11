#include "Time_Manager.h"

IMPLEMENT_SINGLETON(CTime_Manager)

_float CTime_Manager::Get_TimeAcc(const wstring& wstrTimerTag) const
{
}

void CTime_Manager::Tick(const _float& fTimeDelta)
{
	m_fWorldTimeAcc += fTimeDelta;

	m_fWorldTick = fTimeDelta;

	for (auto& Pair : m_Timers)
		Pair.second.m_fTimeAcc += fTimeDelta;
}

HRESULT CTime_Manager::Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() != Timer)
	{
		MSG_BOX("[CTime_Manager] Tag is duplicate Tag");
		return E_FAIL;
	}

	m_Timers.emplace(wstrTimerTag, AlarmDesc);

	return S_OK;
}

HRESULT CTime_Manager::Remove_Timer(const wstring& wstrTimerTag)
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

HRESULT CTime_Manager::Reset_Timer(const wstring& wstrTimerTag)
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

_bool CTime_Manager::Check_Timer(const wstring& wstrTimerTag)
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