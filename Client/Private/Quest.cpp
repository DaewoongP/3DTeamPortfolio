#include "Quest.h"
#include "GameInstance.h"

CQuest::CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) 
	: CGameObject(pDevice, pContext)
{
}

CQuest::CQuest(const CQuest& rhs) 
	: CGameObject(rhs)
{
}

void CQuest::Free()
{
	__super::Free();
}
