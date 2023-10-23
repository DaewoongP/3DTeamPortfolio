#include "Event_Dragon_Hp_Down.h"
#include "GameInstance.h"

#include "Player.h"
#include "Card_Fig.h"
#include "ConjuredDragon.h"

CEvent_Dragon_Hp_Down::CEvent_Dragon_Hp_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Dragon_Hp_Down::CEvent_Dragon_Hp_Down(const CEvent_Dragon_Hp_Down& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Dragon_Hp_Down::Initialize(void* pArg)
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

void CEvent_Dragon_Hp_Down::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (false == m_isCallEvent && 0.3f >= m_pDragon->Get_Current_HP_Percent())
	{
		m_pCard_Fig->On_Dragon_Hp_Down_Script();
		m_isCallEvent = true;
	}
}

CEvent_Dragon_Hp_Down* CEvent_Dragon_Hp_Down::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Dragon_Hp_Down* pInstance = New CEvent_Dragon_Hp_Down(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Dragon_Hp_Down");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Dragon_Hp_Down* CEvent_Dragon_Hp_Down::Clone(void* pArg)
{
	CEvent_Dragon_Hp_Down* pInstance = New CEvent_Dragon_Hp_Down(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Dragon_Hp_Down");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Dragon_Hp_Down::Free()
{
	__super::Free();

	Safe_Release(m_pDragon);
	Safe_Release(m_pCard_Fig);
}