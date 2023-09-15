#include "..\Public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

list<const _tchar*> CLevel_Manager::Get_Layers(const _tchar* pSceneTag)
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
		pGameInstance->Clear_LevelResources(m_iLevelIndex);

	Safe_Release(pGameInstance);

	// 현재레벨과 다음레벨 교체
	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;

	m_iLevelIndex = iLevelIndex;

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
	auto iter = find_if(m_Scenes.begin(), m_Scenes.end(), CTag_Finder(pSceneTag));

	// 설정한 씬태그에 레이어가 존재 하지 않으면 생성해서 대입
	if (m_Scenes.end() == iter)
	{
		list<const _tchar*> Layers;
		Layers.push_back(pLayerTag);

		m_Scenes.emplace(pSceneTag, Layers);
	}
	else // 존재하면 그 리스트에 대입
	{
		(*iter).second.push_back(pLayerTag);
	}

	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}

