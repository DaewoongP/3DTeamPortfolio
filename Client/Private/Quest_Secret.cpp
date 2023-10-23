#include "Quest_Secret.h"
#include "GameInstance.h"

CQuest_Secret::CQuest_Secret(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Secret::Is_Finished()
{
	if (false == m_isClear)
		return false;

	return true;
}

HRESULT CQuest_Secret::Initialize()
{
	return S_OK;
}

void CQuest_Secret::Tick(_float fTimeDelta)
{
}

void CQuest_Secret::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest_Secret::Render()
{
	return S_OK;
}

void CQuest_Secret::Clear_Quest()
{
	m_isClear = true;
}

CQuest_Secret* CQuest_Secret::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Secret* pInstance = New CQuest_Secret(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Secret");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Secret::Free()
{
	__super::Free();
}
