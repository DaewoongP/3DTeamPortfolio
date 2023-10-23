#include "Event_Dragon_Death.h"
#include "GameInstance.h"

#include "Player.h"
#include "Card_Fig.h"
#include "ConjuredDragon.h"

CEvent_Dragon_Death::CEvent_Dragon_Death(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Dragon_Death::CEvent_Dragon_Death(const CEvent_Dragon_Death& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Dragon_Death::Initialize(void* pArg)
{
	BEGININSTANCE;

	LEVELID LevelID = LEVELID(pGameInstance->Get_CurrentLevelIndex());
	if (LEVEL_SANCTUM != LevelID)
		return E_FAIL;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	if (nullptr == pPlayer)
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pCard_Fig = pPlayer->m_pCard_Fig;
	Safe_AddRef(m_pCard_Fig);

	m_pDragon = dynamic_cast<CConjuredDragon*>(pGameInstance->Find_Component_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"), TEXT("GameObject_ConjuredDragon")));
	if (nullptr == m_pDragon)
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	Safe_AddRef(m_pDragon);

	ENDINSTANCE;

	return S_OK;
}

void CEvent_Dragon_Death::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (false == m_isCallEvent && m_pDragon->isDead())
	{
		m_pCard_Fig->On_Dragon_Death();
		m_isCallEvent = true;
	}
}

CEvent_Dragon_Death* CEvent_Dragon_Death::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Dragon_Death* pInstance = New CEvent_Dragon_Death(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Dragon_Death");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Dragon_Death* CEvent_Dragon_Death::Clone(void* pArg)
{
	CEvent_Dragon_Death* pInstance = New CEvent_Dragon_Death(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Dragon_Death");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Dragon_Death::Free()
{
	__super::Free();

	Safe_Release(m_pDragon);
	Safe_Release(m_pCard_Fig);
}