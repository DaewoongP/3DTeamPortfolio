#include "..\Public\Timer.h"

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_tFrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tFixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tLastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}



HRESULT CTimer::Initialize(void)
{
	QueryPerformanceCounter(&m_tFrameTime);
	QueryPerformanceCounter(&m_tLastTime);
	QueryPerformanceCounter(&m_tFixTime);
	QueryPerformanceFrequency(&m_CpuTick);
	
	return S_OK;
}

void Engine::CTimer::Tick(void)
{
	QueryPerformanceCounter(&m_tFrameTime);

	if (m_tFrameTime.QuadPart - m_tFixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_tFixTime = m_tFrameTime;
	}

	m_fTimeDelta = ((m_tFrameTime.QuadPart) - (m_tLastTime.QuadPart)) / _float(m_CpuTick.QuadPart);

	if (true == m_isSlowed)
	{
		// 미리 실제 지나가는 timedelta 더하고
		m_fSlowedTimeAcc += m_fTimeDelta;
		// 클라이언트로 던져줄 timedelta는 power처리해주고
		m_fTimeDelta *= m_fSlowedPower;
		// 시간이 지났으면 false
		if (m_fSlowedTimeAcc > m_fSlowedTime)
		{
			m_isSlowed = false;
			m_fSlowedTimeAcc = 0.f;
			m_fSlowedPower = 1.f;
		}
	}

	m_tLastTime = m_tFrameTime;
}

CTimer * CTimer::Create(void)
{
	CTimer *	pInstance = New CTimer;

	if (FAILED(pInstance->Initialize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTimer::Free(void)
{
}
