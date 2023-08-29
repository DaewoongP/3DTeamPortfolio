#include "Alarm.h"

CAlarm::CAlarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CAlarm::CAlarm(const CAlarm& rhs)
	: CComponent(rhs)
{
}

void CAlarm::Tick(_float fTimeDelta)
{
	for (auto& Pair : m_Timers)
		Pair.second.m_fTimeAcc += fTimeDelta;
}

HRESULT CAlarm::Add_Timer(const wstring& wstrTimerTag, const ALARMDESC& AlarmDesc)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() != Timer)
	{
		MSG_BOX("[CAlarm] Tag is duplicate Tag");
		return E_FAIL;
	}

	m_Timers.emplace(wstrTimerTag, AlarmDesc);

	return S_OK;
}

HRESULT CAlarm::Remove_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CAlarm] Tag is not in map");
		return E_FAIL;
	}

	m_Timers.erase(Timer);

	return S_OK;
}

HRESULT CAlarm::Reset_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CAlarm] Tag is not in map");
		return E_FAIL;
	}

	Timer->second.m_fTimeAcc = 0.f;

	return S_OK;
}

_bool CAlarm::Check_Timer(const wstring& wstrTimerTag)
{
	auto Timer = m_Timers.find(wstrTimerTag);

	if (m_Timers.end() == Timer)
	{
		MSG_BOX("[CAlarm] Tag is not in map");
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

CAlarm* CAlarm::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAlarm* pInstance = new CAlarm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAlarm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAlarm* CAlarm::Clone(void* pArg)
{
	CAlarm* pInstance = new CAlarm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAlarm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlarm::Free()
{
	__super::Free();
}
