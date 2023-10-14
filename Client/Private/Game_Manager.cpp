#include "..\Public\Game_Manager.h"

IMPLEMENT_SINGLETON(CGame_Manager)

CGameObject* CGame_Manager::Get_GameObject(const _tchar* pStaticObjectTag)
{
	CGameObject* pObject = Find_Object(pStaticObjectTag);
	if (nullptr == pObject)
		return nullptr;

	Safe_AddRef(pObject);

	return pObject;
}

HRESULT CGame_Manager::Set_GameObject(const _tchar* pStaticObjectTag, CGameObject* pGameObject)
{
	if (nullptr != Find_Object(pStaticObjectTag))
		return E_FAIL;

	m_StaticObjects.emplace(pStaticObjectTag, pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

CGameObject* CGame_Manager::Find_Object(const _tchar* pStaticObjectTag)
{
	auto Pair = find_if(m_StaticObjects.begin(), m_StaticObjects.end(), CTag_Finder(pStaticObjectTag));

	if (Pair == m_StaticObjects.end())
	{
		MSG_BOX("Failed Find Object");
		__debugbreak();
		return nullptr;
	}

	return Pair->second;
}

void CGame_Manager::Free()
{
	for (auto& Pair : m_StaticObjects)
		Safe_Release(Pair.second);

	m_StaticObjects.clear();
}
