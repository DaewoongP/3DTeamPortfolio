#include "Event_Cliffside.h"
#include "GameInstance.h"

#include "Enemy.h"
#include "Trigger.h"

CEvent_Cliffside::CEvent_Cliffside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEvent_Cliffside::CEvent_Cliffside(const CEvent_Cliffside& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEvent_Cliffside::Initialize(void* pArg)
{
	FAILED_CHECK_RETURN(Add_Components(), E_FAIL);

	return S_OK;
}

void CEvent_Cliffside::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Check_Event_Play_CutScene_0_0();
}

void CEvent_Cliffside::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CEvent_Cliffside::Check_Event_Play_CutScene_0_0()
{
	if (true == m_isCutScene_0_0)
		return;

	if (true == m_pCutScene_0_0->Is_Collision())
	{
		BEGININSTANCE;

		pGameInstance->Add_CutScene(TEXT("CutScene_0_0"));

		ENDINSTANCE;

		if (m_pCutScene_0_0->isDead())
			m_isCutScene_0_0 = true;
	}
}

HRESULT CEvent_Cliffside::Add_Components()
{
	CTrigger::TRIGGERDESC TriggerDesc;

	/* For.Trigger_CutScene_0_0 */
	TriggerDesc.isCollisionToDead = true;
	strcpy_s(TriggerDesc.szCollisionTag, "Trigger_CutSceneTest");
	lstrcpy(TriggerDesc.szOtherTag, TEXT("Player_Default"));
	TriggerDesc.vTriggerSize = _float3(5.f, 5.f, 5.f);
	TriggerDesc.vTriggerWorldPos = _float3(30.0f, 7.0f, 38.0f);

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Trigger"),
		TEXT("Trigger_CutScene_0_0"), reinterpret_cast<CComponent**>(&m_pCutScene_0_0), &TriggerDesc)))
	{
		MSG_BOX("CEvent_Cliffside Failed Add_Components : Trigger_CutScene_0_0");
		return E_FAIL;
	}

	BEGININSTANCE;

	pGameInstance->Read_CutSceneCamera(TEXT("CutScene_0_0"), TEXT("../../Resources/GameData/CutScene/Map0_0.cut"));

	ENDINSTANCE;


	return S_OK;
}

CEvent_Cliffside* CEvent_Cliffside::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEvent_Cliffside* pInstance = New CEvent_Cliffside(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CEvent_Cliffside");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEvent_Cliffside* CEvent_Cliffside::Clone(void* pArg)
{
	CEvent_Cliffside* pInstance = New CEvent_Cliffside(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEvent_Cliffside");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Cliffside::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCutScene_0_0);
	}
}