#include "..\Public\Layer.h"
#include "Component.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_Component(const _tchar* pComponentTag, CComponent* pComponent)
{
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	if (nullptr != Find_Component(pComponentTag))
	{
		MSG_BOX("Component Tag is Already Used");
		return E_FAIL;
	}

	pComponent->Set_Tag(pComponentTag);

	m_Components.emplace(pComponent->Get_Tag(), pComponent);

	return S_OK;
}

HRESULT CLayer::Clear_Layer()
{
	for (auto& pComponent : m_Components)
		Safe_Release(pComponent.second);

	m_Components.clear();

	return S_OK;
}

HRESULT CLayer::Delete_Component(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
	{
		return E_FAIL;
	}

	for (auto iter = m_Components.begin(); iter != m_Components.end(); )
	{
		if (0 == lstrcmp(iter->first, pComponentTag))
		{
			iter = m_Components.erase(iter);
			Safe_Release(pComponent);
			return S_OK;
		}
		else
			++iter;
	}

	return S_OK;
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pComponent : m_Components)
		pComponent.second->Tick(fTimeDelta);
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto iter = m_Components.begin(); iter != m_Components.end();)
	{
		iter->second->Late_Tick(fTimeDelta);

		if (CComponent::OBJ_DEAD == iter->second->Get_ObjEvent())
		{
			Safe_Release(iter->second);
			iter = m_Components.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

CComponent* CLayer::Find_Component(const _tchar* pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (auto& pComponent : m_Components)
		Safe_Release(pComponent.second);

	m_Components.clear();
}

