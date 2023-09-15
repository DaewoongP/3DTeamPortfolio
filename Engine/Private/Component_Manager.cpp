#include "../Public/Component_Manager.h"
#include "../Public/Component.h"
#include "Layer.h"
#include "Level_Manager.h"

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

	m_pLayers = new LAYERS[iNumLevels];

	m_pCurrentLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype, _bool isFailedSkip)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
	{
		if (true == isFailedSkip)
		{
			Safe_Release(pPrototype);
			return S_OK;
		}

		Safe_Release(pPrototype);
		MSG_BOX("Already have Prototype In CComponent_Manager");
		return E_FAIL;
	}

	if (nullptr == pPrototype)
	{
		Safe_Release(pPrototype);
		MSG_BOX("Prototype Component NULL");
		return E_FAIL;
	}

	pPrototype->Set_PrototypeTag(pPrototypeTag);

	m_pPrototypes[iLevelIndex].emplace(pPrototype->Get_PrototypeTag(), pPrototype);

	return S_OK;
}

HRESULT CComponent_Manager::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, const _tchar* pComponentTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	NULL_CHECK_RETURN(pPrototype, E_FAIL);

	CComponent* pComponent = nullptr;
	pComponent = pPrototype->Clone(pArg);

	NULL_CHECK_RETURN(pComponent, E_FAIL);

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	pComponent->Set_Tag(pComponentTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		if (FAILED(pLayer->Add_Component(pComponent->Get_Tag(), pComponent)))
		{
			Safe_Release(pLayer);
			Safe_Release(pComponent);

			return E_FAIL;
		}

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		if (FAILED(pLayer->Add_Component(pComponent->Get_Tag(), pComponent)))
		{
			Safe_Release(pComponent);

			return E_FAIL;
		}
	}

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

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
	
	for (auto& Pair : m_pCurrentLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pCurrentLayers[iLevelIndex].clear();

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();	
}

HRESULT CComponent_Manager::Delete_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return E_FAIL;

	return pLayer->Delete_Component(pComponentTag);
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTag_Finder(pPrototypeTag));
	
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;
		 
	return iter->second;
}

CComponent* CComponent_Manager::Find_Component_In_Layer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	CComponent* pComponent = pLayer->Find_Component(pComponentTag);

	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CLayer* CComponent_Manager::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

HRESULT CComponent_Manager::Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return E_FAIL;

	return pLayer->Clear_Layer();
}

void CComponent_Manager::Set_CurrentScene(const _tchar* pSceneTag)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pCurrentLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pCurrentLayers[i].clear();
	}

	CLevel_Manager* pLevel_Manager = CLevel_Manager::GetInstance();
	Safe_AddRef(pLevel_Manager);

	list<const _tchar*> SceneLayers = pLevel_Manager->Get_Layers(pSceneTag);

	Safe_Release(pLevel_Manager);

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			for (auto& LayerTag : SceneLayers)
			{
				if (!lstrcmp(Pair.first, LayerTag))
				{
					m_pCurrentLayers[i].emplace(Pair);
					Safe_AddRef(Pair.second);
				}
			}
			
		}
	}

	lstrcpy(m_szCurrentSceneTag, pSceneTag);
}

void CComponent_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pCurrentLayers[i])
		{
			Pair.second->Tick(fTimeDelta);
		}
	}
}

void CComponent_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pCurrentLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
	}
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
	
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pCurrentLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pCurrentLayers[i].clear();
	}

	Safe_Delete_Array(m_pCurrentLayers);

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
