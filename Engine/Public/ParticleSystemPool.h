#pragma once
#include "Base.h"
#include "ParticleSystem.h"

BEGIN(Engine)

class CParticleSystemPool final : public CBase
{
	DECLARE_SINGLETON(CParticleSystemPool)
private:
	explicit CParticleSystemPool() = default;
	virtual ~CParticleSystemPool() = default;

public:
	HRESULT Reserve_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szParticleTag, const _tchar* szParticleDirectoryPath, _uint iNumReserveParticles);
	void Play_Particle(const _tchar* szParticleTag);
	void Return_Particle(const _tchar* szParticleTag, CParticleSystem* pParticle);

private:
	unordered_map<const _tchar*, class CParticleSystemQueue*>	m_ParticlePool;

private:
	class CParticleSystemQueue* Find_ParticleQueue(const _tchar* szParticleTag);

public:
	virtual void Free() override;
};

// ť ���۷����� �������� �ָ��ؼ� ��������ϴ�.
class CParticleSystemQueue final : public CBase
{
private:
	explicit CParticleSystemQueue() = default;
	virtual ~CParticleSystemQueue() = default;

public:
	inline void Push_Back(CParticleSystem* pParticle) {
		m_Particles.push(pParticle);
	}
	inline CParticleSystem* Pop_Front() {
		CParticleSystem* pRetParticle = m_Particles.front();
		m_Particles.pop();
		return pRetParticle;
	}

private:
	queue<CParticleSystem*> m_Particles;

public:
	inline static CParticleSystemQueue* Create() {
		return New CParticleSystemQueue();
	}
	virtual void Free() override {
		while (!m_Particles.empty())
		{
			Safe_Release(m_Particles.front());
			m_Particles.pop();
		}
	}
};

END