#include "..\Public\Quest_Save_Fig.h"
#include "GameInstance.h"

CQuest_Save_Fig::CQuest_Save_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CQuest(pDevice, pContext)
{
}

_bool CQuest_Save_Fig::Is_Finished()
{
	if (false == m_isSaved)
		return false;

	return true;
}

HRESULT CQuest_Save_Fig::Initialize()
{
	return S_OK;
}

void CQuest_Save_Fig::Tick(_float fTimeDelta)
{
}

void CQuest_Save_Fig::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest_Save_Fig::Render()
{
	return S_OK;
}

void CQuest_Save_Fig::Clear_Quest()
{
	m_isSaved = true;
}

CQuest_Save_Fig* CQuest_Save_Fig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Save_Fig* pInstance = New CQuest_Save_Fig(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CQuest_Save_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest_Save_Fig::Free()
{
	__super::Free();
}
