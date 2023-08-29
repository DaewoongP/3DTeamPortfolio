#include "..\Public\ParticleModule.h"

CParticleModule::CParticleModule()
{
}

CParticleModule::CParticleModule(const CParticleModule& _rhs)
	: m_isCloned(true)
{
}

HRESULT CParticleModule::Initialize_Prototype(const _tchar* _pFilePath)
{
	return S_OK;
}

void CParticleModule::Free()
{
}