#include "UI_Gruop.h"
#include "GameInstance.h"
#include "UI.h"

CUI_Gruop::CUI_Gruop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI_Gruop::CUI_Gruop(const CUI_Gruop& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI_Gruop::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Gruop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	lstrcpy(m_wszUIGroupName, (_tchar*)pArg);

	return S_OK;
}

void CUI_Gruop::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Set_Parent();

	if (nullptr != m_pParent)
	{
		m_pParent->Tick(fTimeDelta);
	}

	for (auto& iter : m_Childs)
	{
		iter->Tick(fTimeDelta);
	}


}

void CUI_Gruop::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pParent)
	{
		m_pParent->Late_Tick(fTimeDelta);
	}

	for (auto& iter : m_Childs)
	{
		iter->Late_Tick(fTimeDelta);
	}
}

void CUI_Gruop::Set_Parent()
{
	if (nullptr == m_pParent)
		return;

	for (auto& iter : m_Childs)
	{
		iter->Set_Parent(m_pParent);
	}
}


void CUI_Gruop::Free()
{
	__super::Free();
}
