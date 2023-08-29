#pragma once

#include "ParticleModule.h"

BEGIN(Engine)

class CMainModule : public CParticleModule
{
private:
	explicit CMainModule();
	explicit CMainModule(const CMainModule& _rhs);
	virtual ~CMainModule() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pFilePath);

private:
	_float m_fDuration = { 5.0f };
	_bool m_isLooping = { false };
	_bool m_isPrewarm = { false };
	_float m_fStartDelay = { 0.f };
	_float m_fStartLifeTime = { 5.f };
	_float m_fStartSpeed = { 5.f };

public:
	static CMainModule* Create(const _tchar* _pFilePath);
	virtual void Free();
};

END