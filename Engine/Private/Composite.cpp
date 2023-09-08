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
	m_Components.emplace(pComponentTag, pComponent);

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
	CComponent* pComponent = Find_Component(pComponentTag);
	if (nullptr == pComponent)
	{
		return E_FAIL;
	}
	Safe_Release(pComponent);
	m_Components.erase(pComponentTag);
	return S_OK;
}

void CComposite::Free()
{
	__super::Free();
	
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();
}
