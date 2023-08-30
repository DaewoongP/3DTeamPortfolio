#pragma once
#include "ParticleModule.h"
#include "VIBuffer_Color_Instance.h"

BEGIN(Engine)

struct ENGINE_DLL MAIN_MODULE
{
	_float fDuration = { 5.0f };
	_bool isLooping = { false };
	_bool isPrewarm = { false };
	_float2 fStartDelay = { 0.f, 0.f };
	_float2 fStartLifeTime = { 5.f, 5.f };
	_float2 fStartSpeed = { 5.f, 5.f };
};

class CParticleSystem;

class CMainModule : public CParticleModule
{
	friend class CParticleSystem;
private:
	explicit CMainModule();
	explicit CMainModule(const CMainModule& _rhs);
	virtual ~CMainModule() = default;

public:
	MAIN_MODULE* Get_DescPtr() { return &m_MainModuleDesc; }
	
public:
	virtual HRESULT Initialize_Prototype(CParticleSystem* pParticleSystem);
	void Tick(_float _fTimeDelta, vector<CVIBuffer_Color_Instance::COLORINSTANCE>& _ColInsts);

private:
	virtual HRESULT SaveModule(const _tchar* _pDirectoyPath) override;
	virtual HRESULT LoadModule(const _tchar* _pDirectoyPath) override;

	void ResetParticle(MAIN_MODULE& particleDesc);
	void ResetAllParticle();

private:
	CParticleSystem* m_pParticleSystem = { nullptr };
	MAIN_MODULE m_MainModuleDesc;
	vector<MAIN_MODULE>   m_ParticleDescs;

public:
	static CMainModule* Create(CParticleSystem* pParticleSystem);
	virtual void Free();
};

END