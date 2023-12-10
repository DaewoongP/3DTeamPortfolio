#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "Camera_Manager.h"
#include "TexturePool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

list<const _tchar*> CLevel_Manager::Get_CurrentSceneLayers(const _tchar* pSceneTag)
{
	auto iter = find_if(m_Scenes.begin(), m_Scenes.end(), CTag_Finder(pSceneTag));
	if (iter == m_Scenes.end())
		return list<const _tchar*>();

	return iter->second;
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 이전 레벨에서 사용되던 자원을 날린다.
	if (nullptr != m_pCurrentLevel)
	{
		std::lock_guard<std::mutex> lock(mtx);
		pGameInstance->Clear_LevelResources(m_iLevelIndex);
		CTexturePool::GetInstance()->Clear_Textures();
	}

	// 현재레벨과 다음레벨 교체
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iLevelIndex = iLevelIndex;

	pGameInstance->Initialize_Level(m_iLevelIndex);

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	NULL_CHECK_RETURN_MSG(m_pCurrentLevel, , TEXT("Current Level NULL"));
	
	m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	NULL_CHECK_RETURN_MSG(m_pCurrentLevel, E_FAIL, TEXT("Current Level NULL"));

	return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Add_Scene(const _tchar* pSceneTag, const _tchar* pLayerTag)
{
	auto SceneIter = find_if(m_Scenes.begin(), m_Scenes.end(), CTag_Finder(pSceneTag));
	
	// 씬이 없으면
	if (m_Scenes.end() == SceneIter)
	{
		// 레이어 리스트만 그냥 생성해서 emplace
		list<const _tchar*> Layers;
		Layers.push_back(pLayerTag);
		m_Scenes.emplace(pSceneTag, Layers);
	}
	else // 씬이 이미 있으면
	{
		// 레이어 태그가 있는지 검사하고
		for (auto& ListLayerTag : (*SceneIter).second)
		{
			if (!lstrcmp(ListLayerTag, pLayerTag))
			{
				__debugbreak();
				return S_FALSE;
			}
		}
		// 레이어가 없으면
		// 그 씬에 레이어 태그를 푸쉬백
		SceneIter->second.push_back(pLayerTag);
	}
	
	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}

