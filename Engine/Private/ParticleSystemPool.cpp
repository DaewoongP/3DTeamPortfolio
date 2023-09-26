#include "..\Public\ParticleSystemPool.h"
#include "Component_Manager.h"

IMPLEMENT_SINGLETON(CParticleSystemPool)

HRESULT CParticleSystemPool::Reserve_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* szParticleTag, const _tchar* szParticleDirectoryPath, _uint iNumReserveParticles)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	_tchar szPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
	lstrcat(szPrototypeTag, szParticleTag);

	if (FAILED(pComponent_Manager->Add_Prototype(0, szPrototypeTag
		, CParticleSystem::Create(pDevice, pContext, szParticleDirectoryPath, 0))))
	{
		Safe_Release(pComponent_Manager);
		__debugbreak();
		return E_FAIL;
	}

	CParticleSystemQueue* pParticleQueue = CParticleSystemQueue::Create();
	
	for (_uint i = 0; i < iNumReserveParticles; ++i)
	{
		pParticleQueue->Push_Back(dynamic_cast<CParticleSystem*>(pComponent_Manager->Clone_Component(0, szPrototypeTag, nullptr)));
	}

	m_ParticlePool.emplace(szParticleTag, pParticleQueue);

	Safe_Release(pComponent_Manager);

	return S_OK;
}

void CParticleSystemPool::Play_Particle(const _tchar* szParticleTag)
{
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	CParticleSystemQueue* pParticleQueue = Find_ParticleQueue(szParticleTag);

	_tchar szComponentTag[MAX_PATH] = TEXT("Com_");
	lstrcat(szComponentTag, szParticleTag);
	lstrcat(szComponentTag, Generate_HashtagW().c_str());

	pComponent_Manager->Add_Component(pParticleQueue->Pop_Front(), 0, TEXT("Layer_Particle"), szComponentTag);

	Safe_Release(pComponent_Manager);
}

void CParticleSystemPool::Return_Particle(const _tchar* szParticleTag, CParticleSystem* pParticle)
{
	// 죽은 파티클을 다시 큐에 담는다
	Safe_AddRef(pParticle);
	Find_ParticleQueue(szParticleTag)->Push_Back(pParticle);
}

CParticleSystemQueue* CParticleSystemPool::Find_ParticleQueue(const _tchar* szParticleTag)
{
	auto iter = find_if(m_ParticlePool.begin(), m_ParticlePool.end(), CTag_Finder(szParticleTag));

	if (m_ParticlePool.end() == iter)
		return nullptr;

	return iter->second;
}

void CParticleSystemPool::Free()
{
	for (auto& pair : m_ParticlePool)
	{
		Safe_Release(pair.second);
	}

	m_ParticlePool.clear();
}
