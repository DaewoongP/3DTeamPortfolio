#pragma once

#include "Base.h"

BEGIN(Engine)

class CParticleModule abstract : public CBase
{
protected:
	explicit CParticleModule();
	explicit CParticleModule(const CParticleModule& _rhs);
	virtual ~CParticleModule() = default;

public:
	virtual HRESULT Initialize_Prototype();

public:
	virtual HRESULT SaveModule(const _tchar* _pDirectoyPath) PURE;
	virtual HRESULT LoadModule(const _tchar* _pDirectoyPath) PURE;

protected:
	_bool m_isCloned = { false };

public:
	virtual void Free();
	virtual CParticleModule* Clone() PURE;
};

END


