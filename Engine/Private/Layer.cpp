#include "..\Public\Layer.h"
#include "..\Public\GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObjects(const _tchar* pGameObjectTag, CGameObject* pGameObject)
{
	NULL_CHECK_RETURN(pGameObject, E_FAIL);

	if (nullptr != Find_GameObject(pGameObjectTag))
	{
		MSG_BOX("GameObject Tag is Already Used");
		return E_FAIL;
	}
	
	m_GameObjects.emplace(pGameObjectTag, pGameObject);

	return S_OK;
}

CGameObject* CLayer::Find_GameObject(const _tchar* pGameObjectTag)
{
	auto	iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), CTag_Finder(pGameObjectTag));

	if (iter == m_GameObjects.end())
		return nullptr;

	return iter->second;
}

HRESULT CLayer::Clear_Layer()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject.second->Tick(fTimeDelta);
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		iter->second->Late_Tick(fTimeDelta);

		if (CGameObject::OBJ_DEAD == iter->second->Get_ObjEvent())
		{
			Safe_Release(iter->second);
			iter = m_GameObjects.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();
}

