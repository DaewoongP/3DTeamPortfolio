#include "..\Public\Event_Cliffside_Next_Level.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#include "Cliff_Gate.h"
#include "Trigger.h"

CEvent_Cliffside_Next_Level::CEvent_Cliffside_Next_Level(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Cliffside_Next_Level::CEvent_Cliffside_Next_Level(const CEvent_Cliffside_Next_Level& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Cliffside_Next_Level::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	BEGININSTANCE;
	auto pMapObjectLayer = pGameInstance->Find_Components_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_BackGround"));
	ENDINSTANCE;

	for (auto Pair : *pMapObjectLayer)
	{
		wstring wstrObjTag = Pair.first;
		// Cliff_Gate를 찾는다.
		if (wstring::npos != wstrObjTag.find(TEXT("Cliff_Gate")))
		{
			/* 여기 두번 불려서 누수남 */
			m_pCliff_Gate = static_cast<CCliff_Gate*>(Pair.second);
			Safe_AddRef(m_pCliff_Gate);
		}
	}

	return S_OK;
}

void CEvent_Cliffside_Next_Level::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Check_Event(fTimeDelta);
}

void CEvent_Cliffside_Next_Level::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CEvent_Cliffside_Next_Level::Add_Components()
{
	/* For.Trigger_Cliff_Next_Level */
	CTrigger::TRIGGERDESC TriggerDesc;
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_Cliff_Next_Level_Gate");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(1.f, 2.5f, 1.f);
	TriggerDesc.vTriggerWorldPos = _float3(72.5f, 8.75f, 90.5f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_Cliff_Next_Level"), reinterpret_cast<CComponent**>(&m_pNext_Level), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Cliffside_Next_Level Failed Add_Components : Trigger_Cliff_Next_Level");
		return E_FAIL;
	}

	return S_OK;
}

void CEvent_Cliffside_Next_Level::Check_Event(_float fTimeDelta)
{
	if (true == m_isCheck)
		return;

	if (true == m_pNext_Level->Is_Collision())
	{
		if (nullptr == m_pCliff_Gate)
		{
			MSG_BOX("Failed to Find Cliff_Gate");
			return;
		}

		_bool isGateOpen = m_pCliff_Gate->Get_GateOpen();

		if(true == isGateOpen)
			m_isCheck = true;
	}
}

CEvent_Cliffside_Next_Level* CEvent_Cliffside_Next_Level::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Cliffside_Next_Level* pInstance = New CEvent_Cliffside_Next_Level(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Cliffside_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEvent_Cliffside_Next_Level::Clone(void* pArg)
{
	CEvent_Cliffside_Next_Level* pInstance = New CEvent_Cliffside_Next_Level(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Cliffside_Next_Level");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Cliffside_Next_Level::Free()
{
	__super::Free();

	Safe_Release(m_pNext_Level);
	Safe_Release(m_pCliff_Gate);
}