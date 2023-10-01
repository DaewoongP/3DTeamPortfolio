#include "Quest.h"

CQuest::CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CGameObject(pDevice,pContext)
{
}

CQuest::CQuest(const CQuest& rhs) : CGameObject(rhs)
{
}

HRESULT CQuest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Initialize(void* pArg)
{
	return S_OK;
}

void CQuest::Tick(_float fTimeDelta)
{
}

void CQuest::Late_Tick(_float fTimeDelta)
{
}

HRESULT CQuest::Render()
{
	return S_OK;
}


void CQuest::Free()
{
	__super::Free();
}
