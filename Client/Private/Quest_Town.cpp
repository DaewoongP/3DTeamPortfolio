#include "Quest_Town.h"
#include "GameInstance.h"

CQuest_Town::CQuest_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Town::Is_Finished()
{
	if (false == m_isClear)
		return false;

	return true;
}

HRESULT CQuest_Town::Initialize()
{
	return S_OK;
}

void CQuest_Town::Tick(_float fTimeDelta)
{
}

void CQuest_Town::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest_Town::Render()
{
	return S_OK;
}

void CQuest_Town::Clear_Quest()
{
	m_isClear = true;
}

CQuest_Town* CQuest_Town::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Town* pInstance = New CQuest_Town(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Town");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Town::Free()
{
	__super::Free();
}
