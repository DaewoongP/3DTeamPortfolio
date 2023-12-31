#include "..\Public\Composite.h"
#include "Component_Manager.h"
#include "GameObject.h"

CComposite::CComposite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CComposite::CComposite(const CComposite& rhs)
	: CComponent(rhs)
	, m_Components(rhs.m_Components)
{
	for (auto& Pair : m_Components)
		Safe_AddRef(Pair.second);
}

HRESULT CComposite::Initialize_Level(_uint iCurrentLevelIndex)
{
	for (auto& Pair : m_Components)
		Pair.second->Initialize_Level(iCurrentLevelIndex);

	return S_OK;
}

void CComposite::Tick(_float fTimeDelta)
{
	for (auto& Pair : m_Components)
		Pair.second->Tick(fTimeDelta);
}

void CComposite::Late_Tick(_float fTimeDelta)
{
	for (auto& Pair : m_Components)
		Pair.second->Late_Tick(fTimeDelta);
}

HRESULT CComposite::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, _Inout_ CComponent** ppOut, void* pArg)
{
	CComponent_Manager* pComponentManager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponentManager);

	CComponent* pComponent = pComponentManager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);

	if (nullptr == pComponent)
		return E_FAIL;

	pComponent->Set_Owner(this);
	pComponent->Set_Tag(pComponentTag);
	m_Components.emplace(pComponent->Get_Tag(), pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pComponentManager);

	return S_OK;
}

CComponent* CComposite::Find_Component(const _tchar* pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (m_Components.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CComposite::Delete_Component(const _tchar* pComponentTag)
{
	for (auto iter = m_Components.begin(); iter != m_Components.end(); )
	{
		if (0 == lstrcmp(iter->second->Get_Tag(), pComponentTag))
		{
			CComponent* pComponent = iter->second;
			m_Components.erase(iter);
			Safe_Release(pComponent);
			return S_OK;
		}
		else
			++iter;
	}
	
	return E_FAIL;
}

void CComposite::Free()
{
	__super::Free();
	
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();
}
