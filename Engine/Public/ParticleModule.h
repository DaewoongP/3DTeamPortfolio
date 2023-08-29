#pragma once

#include "Base.h"

BEGIN(Engine)

class CParticleModule abstract : public CBase
{
protected:
	friend class CParticleSystem;

protected:
	explicit CParticleModule();
	explicit CParticleModule(const CParticleModule& _rhs);
	virtual ~CParticleModule() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pFilePath);

protected:
	_bool m_isCloned = { false };

public:
	virtual void Free();
};

END