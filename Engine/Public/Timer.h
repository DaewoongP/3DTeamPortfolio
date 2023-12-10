#pragma once

/* =============================================== */
// 
//	�� : �ڴ��
//	�� :
//
/* =============================================== */

#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer() = default;

public:
	_float		Get_TimeDelta(void) { return m_fTimeDelta; }
	void		Set_SlowedTime(_float fSlowPower, _float fTime) { 
		m_fSlowedPower = fSlowPower;
		m_fSlowedTime = fTime;
		m_isSlowed = true;
	}

public:
	HRESULT		Initialize(void);
	void		Tick(void);
	
private:
	LARGE_INTEGER			m_tFrameTime;
	LARGE_INTEGER			m_tFixTime;
	LARGE_INTEGER			m_tLastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;

private:
	_float					m_fSlowedPower = { 1.f };
	_bool					m_isSlowed = { false };
	_float					m_fSlowedTime = { 0.f };
	_float					m_fSlowedTimeAcc = { 0.f };

public:
	static CTimer*		Create(void);
	virtual void		Free(void) override;
};

END
