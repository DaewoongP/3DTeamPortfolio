#include "Quest_Potion.h"
#include "GameInstance.h"

CQuest_Potion::CQuest_Potion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Potion::Is_Finished()
{
	if (false == m_isSaved)
		return false;

	return true;
}

HRESULT CQuest_Potion::Initialize()
{
	return S_OK;
}

void CQuest_Potion::Tick(_float fTimeDelta)
{
}

void CQuest_Potion::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest_Potion::Render()
{
	return S_OK;
}

void CQuest_Potion::Clear_Quest()
{
}

CQuest_Potion* CQuest_Potion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Potion* pInstance = New CQuest_Potion(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Potion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Potion::Free()
{
	__super::Free();
}
