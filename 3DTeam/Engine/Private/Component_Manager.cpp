#include "../Public/Component_Manager.h"
#include "../Public/Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Containers(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
	{
		MSG_BOX("Failed CComponent_Manager Reserve Containers");
		return E_FAIL;
	}

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
	{
		Safe_Release(pPrototype);
		MSG_BOX("Already have Prototype In CComponent_Manager");
		return E_FAIL;
	}

	NULL_CHECK_RETURN_MSG(pPrototype, E_FAIL, TEXT("Prototype Component NULL"));

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);

	if (nullptr == pPrototype)
	{
		MSG_BOX("Failed Find Prototype");
		return nullptr;
	}
		
	CComponent* pComponent = pPrototype->Clone(pArg);

	if (nullptr == pComponent)
	{
		MSG_BOX("Failed Clone Prototype");
		return nullptr;
	}

	return pComponent;
}

void CComponent_Manager::Clear_LevelResources(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
	{
		MSG_BOX("Out of LevelIndex");
		return;
	}
	
	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;
		 
	return iter->second;
}

HRESULT CComponent_Manager::Delete_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	for (auto Pair = m_pPrototypes[iLevelIndex].begin(); Pair != m_pPrototypes[iLevelIndex].end();)
	{
		if (!lstrcmp(pPrototypeTag, Pair->first))
		{
			Safe_Release(Pair->second);
			Pair = m_pPrototypes[iLevelIndex].erase(Pair);

			return S_OK;
		}
		else
			++Pair;
	}

	return E_FAIL;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
