#pragma once
#include "ParticleModule.h"

BEGIN(Engine)

struct ENGINE_DLL MAIN_MODULE
{
	_float fDuration = { 5.0f };
	_bool isLooping = { false };
	_bool isPrewarm = { false };
	_float fStartDelay = { 0.f };
	_float fStartLifeTime = { 5.f };
	_float fStartSpeed = { 5.f };
};

class CMainModule : public CParticleModule
{
protected:
	friend class CParticleSystem;

private:
	explicit CMainModule();
	explicit CMainModule(const CMainModule& _rhs);
	virtual ~CMainModule() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pFilePath);

private:
	MAIN_MODULE m_MainModuleDesc;

public:
	static CMainModule* Create(const _tchar* _pFilePath);
	virtual void Free();
};

END