#include "Dummy_UI_Group.h"
#include "GameInstance.h"
#include "Dummy_UI.h"

CDummy_UI_Group::CDummy_UI_Group(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CDummy_UI_Group::CDummy_UI_Group(const CDummy_UI_Group& rhs)
	: CGameObject(rhs)
{
	lstrcpy(m_wszUIGroupName, rhs.m_wszUIGroupName);
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

	return S_OK;
}

void CDummy_UI_Group::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Set_Parent();
}

void CDummy_UI_Group::Late_Tick(_float fTimeDelta)
{

}

void CDummy_UI_Group::Set_Parent()
{
	if (nullptr == m_pParent)
		return;

	for (auto& iter : m_Childs)
	{
		iter->Set_Parent(m_pParent);
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

CDummy_UI_Group* CDummy_UI_Group::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _tchar* pGroupName)
{
	CDummy_UI_Group* pInstance = New CDummy_UI_Group(pDevice, pContext);

	lstrcpy(pInstance->m_wszUIGroupName, pGroupName);

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
