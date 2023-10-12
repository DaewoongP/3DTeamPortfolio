#include "..\Public\Component.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
{
	lstrcpy(m_pPrototypeTag, rhs.m_pPrototypeTag);
	m_isCloned = true;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComponent::Initialize_Level(_uint iCurrentLevelIndex)
{
	return S_OK;
}

void CComponent::Tick(_float fTimeDelta)
{
}

void CComponent::Late_Tick(_float fTimeDelta)
{
}

HRESULT CComponent::Render()
{
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
