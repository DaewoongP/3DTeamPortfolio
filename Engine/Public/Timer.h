#pragma once

/* =============================================== */
// 
//	Á¤ : ¹Ú´ë¿õ
//	ºÎ :
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

public:
	HRESULT		Initialize(void);
	void		Tick(void);
	
private:
	LARGE_INTEGER			m_tFrameTime;
	LARGE_INTEGER			m_tFixTime;
	LARGE_INTEGER			m_tLastTime;
	LARGE_INTEGER			m_CpuTick;

	_float					m_fTimeDelta;

public:
	static CTimer*		Create(void);
	virtual void		Free(void) override;
};

END
