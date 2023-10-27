#include "Event_Enter_Sanctum.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Player.h"
#include "Trigger.h"
#include "Card_Fig.h"
#include "LightStand.h"
#include "Sanctum_Door.h"

CEvent_Enter_Sanctum::CEvent_Enter_Sanctum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Enter_Sanctum::CEvent_Enter_Sanctum(const CEvent_Enter_Sanctum& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Enter_Sanctum::Initialize(void* pArg)
{
	/* Set Monsters */
	BEGININSTANCE;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	if (nullptr == pPlayer)
	{
		ENDINSTANCE;
		return E_FAIL;
	}
	m_pCard_Fig = pPlayer->m_pCard_Fig;
	Safe_AddRef(m_pCard_Fig);

	auto pMonsterLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_Monster"));

	for (auto Pair : *pMonsterLayer)
	{
		wstring wstrObjTag = Pair.first;

		if (wstring::npos != wstrObjTag.find(TEXT("Torch")))
		{
			m_pMonsters.emplace(wstrObjTag, static_cast<CEnemy*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	/* Set Light Stands And Sanctum Doors */
	auto pBackGroundLayer = pGameInstance->Find_Components_In_Layer(LEVEL_SANCTUM, TEXT("Layer_BackGround"));

	for (auto Pair : *pBackGroundLayer)
	{
		wstring wsObjTag = Pair.first;

		if (wstring::npos != wsObjTag.find(TEXT("LightStand")))
		{
			m_pLightStands.push_back(static_cast<CLightStand*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
		else if (wstring::npos != wsObjTag.find(TEXT("Sanctum_Door")))
		{
			m_pDoors.push_back(static_cast<CSanctum_Door*>(Pair.second));
			Safe_AddRef(Pair.second);
		}
	}

	ENDINSTANCE;

	return S_OK;
}

void CEvent_Enter_Sanctum::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(false == m_isSpawn)
		Check_Event_On_Torch_To_Spawn();

	else if (false == m_isDoorOpen)
		Check_Monsters_Are_Death();
}

void CEvent_Enter_Sanctum::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Enter_Sanctum::Check_Event_On_Torch_To_Spawn()
{
	for (auto iter = m_pLightStands.begin(); iter != m_pLightStands.end();)
	{
		if (true == (*iter)->Get_FireOn())
		{
			Safe_Release(*iter);
			iter = m_pLightStands.erase(iter);
		}
		else
			++iter;
	}

	if (0 == m_pLightStands.size())
	{
		for (auto& Pair : m_pMonsters)
			Pair.second->Spawn();
		m_isSpawn = true;
	}
}

void CEvent_Enter_Sanctum::Check_Monsters_Are_Death()
{
	for (auto iter = m_pMonsters.begin(); iter != m_pMonsters.end();)
	{
		if (true == iter->second->isDead())
		{
			Safe_Release(iter->second);
			iter = m_pMonsters.erase(iter);
		}
		else
			++iter;
	}

	if (0 == m_pMonsters.size())
	{
		for (auto iter = m_pDoors.begin(); iter != m_pDoors.end();)
		{
			(*iter)->Open_Door();
			Safe_Release(*iter);
			iter = m_pDoors.erase(iter);
		}
		m_pCard_Fig->On_Enter_Sanctum_Script();
		m_isDoorOpen = true;
	}
}

CEvent_Enter_Sanctum* CEvent_Enter_Sanctum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Enter_Sanctum* pInstance = New CEvent_Enter_Sanctum(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Enter_Sanctum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Enter_Sanctum* CEvent_Enter_Sanctum::Clone(void* pArg)
{
	CEvent_Enter_Sanctum* pInstance = New CEvent_Enter_Sanctum(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Enter_Sanctum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Enter_Sanctum::Free()
{
	__super::Free();

	Safe_Release(m_pCard_Fig);

	for (auto& Pair : m_pMonsters)
		Safe_Release(Pair.second);

	for (auto& pLightStand : m_pLightStands)
		Safe_Release(pLightStand);

	for(auto& pDoor : m_pDoors)
		Safe_Release(pDoor);
}