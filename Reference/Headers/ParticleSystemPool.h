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
	void Play_Particle(const _tchar* szParticleTag, _float3 vWorldPosition);
	void Play_Particle(const _tchar* szParticleTag, _float4x4 PositionMatrix, _float4x4 ObjectMatrix);
	void Play_Particle(const _tchar* szParticleTag, _float4x4 OffsetMatrix, const _float4x4* pBindBoneMatrix, _float4x4 PivotMatrix, const _float4x4* pWorldMatrix);
	void Return_Particle(CParticleSystem* pParticle);

private:
	unordered_map<const _tchar*, class CParticleSystemQueue*>	m_ParticlePool;

private:
	class CParticleSystemQueue* Find_ParticleQueue(const _tchar* szParticleTag);

public:
	virtual void Free() override;
};

// 큐 레퍼런스로 가져오기 애매해서 만들었습니다.
class CParticleSystemQueue final : public CBase
{
private:
	explicit CParticleSystemQueue() = default;
	virtual ~CParticleSystemQueue() = default;

public:
	inline void Push_Back(CParticleSystem* pParticle) {
		m_Particles.push(pParticle);
#ifdef _DEBUG
		cout << "Push_Back : " << m_Particles.size() << endl;
#endif // _DEBUG
		
	}
	inline CParticleSystem* Pop_Front() {
		if (true == m_Particles.empty())
			return nullptr;
		
		CParticleSystem* pRetParticle = m_Particles.front();
		m_Particles.pop();
#ifdef _DEBUG
		cout << "Pop_Front : " << m_Particles.size() << endl;
#endif // _DEBUG
		
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