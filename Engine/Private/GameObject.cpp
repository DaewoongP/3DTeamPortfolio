#include "..\Public\GameObject.h"
#include "Level_Manager.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	: CComposite(rhs)
{
	m_pTransform = static_cast<CTransform*>(rhs.m_pTransform->Clone(nullptr));
}

HRESULT CGameObject::Initialize_Prototype()
{
	m_pTransform = CTransform::Create(m_pDevice, m_pContext);

	if (nullptr == m_pTransform)
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	m_pTransform->Set_Owner(this);

	m_Components.emplace(TEXT("Com_Transform"), m_pTransform);

	Safe_AddRef(m_pTransform);

	return S_OK;
}

void CGameObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CGameObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CGameObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject::Render_Depth()
{
	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
