#include "Dummy_UI_Group.h"
#include "GameInstance.h"
#include "Dummy_UI.h"

CDummy_UI_Group::CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Gruop(pDevice, pContext)
{
}

CDummy_UI_Group::CDummy_UI_Group(const CDummy_UI_Group& rhs)
	: CUI_Gruop(rhs)
{
}

HRESULT CDummy_UI_Group::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummy_UI_Group::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return E_FAIL;

	lstrcpy(m_wszUIGroupName, (_tchar*)pArg);
	
	return S_OK;
}

void CDummy_UI_Group::Tick(_float fTimeDelta)
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

void CDummy_UI_Group::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pParent)
	{
		m_pParent->Late_Tick(fTimeDelta);
	}

	for (auto& iter : m_Childs)
	{
		iter->Late_Tick(fTimeDelta);
	}
}


void CDummy_UI_Group::Delete(CDummy_UI* pDeleteUI)
{
	if (wcscmp(m_pParent->Get_Tag(), pDeleteUI->Get_Tag()) == 0)
	{
		m_pParent = nullptr;
		return;
	}

	_int iIndex = 0;
	for (auto& iter : m_Childs)
	{
		if (wcscmp(iter->Get_Tag(), pDeleteUI->Get_Tag()) == 0)
		{
			m_Childs.erase(m_Childs.begin() + iIndex);
			return;
		}
	}

}

void CDummy_UI_Group::Clear()
{
	if (nullptr != m_pParent)
	{
		m_pParent = nullptr;
	}

	if (m_Childs.size() > 0)
	{
		m_Childs.clear();
	}
}

CDummy_UI_Group* CDummy_UI_Group::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_UI_Group* pInstance = New CDummy_UI_Group(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDummy_UI_Group");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_UI_Group::Clone(void* pArg)
{
	CDummy_UI_Group* pInstance = New CDummy_UI_Group(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDummy_UI_Group");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_UI_Group::Free()
{
	__super::Free();
}
