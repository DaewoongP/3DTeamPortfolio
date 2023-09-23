#include "Sequence_Levitated.h"

#include "GameInstance.h"

#include "Wait.h"
#include "Action.h"
#include "Random_Select.h"
#include "Check_Distance.h"

CSequence_Levitated::CSequence_Levitated(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_Levitated::CSequence_Levitated(const CSequence_Levitated& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_Levitated::Initialize(void* pArg)
{
	/* 쿨타임 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	/* 스펠 확인 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (BUFF_LEVIOSO & *pICurrentSpell)
				return true;

			return false;
		});

	return S_OK;
}

HRESULT CSequence_Levitated::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	if (BEHAVIOR_SUCCESS == hr)
	{
		_uint* pICurrentSpell = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
			return E_FAIL;

		if (*pICurrentSpell & BUFF_LEVIOSO)
			*pICurrentSpell ^= BUFF_LEVIOSO;
	}

	return hr;
}

HRESULT CSequence_Levitated::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CModel* pModel = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
			throw TEXT("pModel is nullptr");
		CTransform* pTransform = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pTransform", pTransform)))
			throw TEXT("pTransform is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Levitated_Enter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Enter)
			throw TEXT("pAction_Levitated_Enter is nullptr");
		CRandom_Select* pRandom_Levitated_Loop = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
		if (nullptr == pRandom_Levitated_Loop)
			throw TEXT("pRandom_Levitated_Loop is nullptr");
		CAction* pAction_Levitated_Land = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Land)
			throw TEXT("pAction_Levitated_Land is nullptr");

		CAction* pAction_Levitated_Loop1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Loop1)
			throw TEXT("pAction_Levitated_Loop1 is nullptr");
		CAction* pAction_Levitated_Loop2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Loop2)
			throw TEXT("pAction_Levitated_Loop2 is nullptr");
		CAction* pAction_Levitated_Loop3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Loop3)
			throw TEXT("pAction_Levitated_Loop3 is nullptr");
		CAction* pAction_Levitated_Loop4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Loop4)
			throw TEXT("pAction_Levitated_Loop4 is nullptr");
		CAction* pAction_Levitated_Loop5 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Loop5)
			throw TEXT("pAction_Levitated_Loop5 is nullptr");

		CWait* pTsk_Wait_Loop1 = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait_Loop1)
			throw TEXT("pTsk_Wait_Loop1 is nullptr");
		CWait* pTsk_Wait_Loop2 = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait_Loop2)
			throw TEXT("pTsk_Wait_Loop2 is nullptr");
		CWait* pTsk_Wait_Loop3 = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait_Loop3)
			throw TEXT("pTsk_Wait_Loop3 is nullptr");
		CWait* pTsk_Wait_Loop4 = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait_Loop4)
			throw TEXT("pTsk_Wait_Loop4 is nullptr");
		CWait* pTsk_Wait_Loop5 = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait_Loop5)
			throw TEXT("pTsk_Wait_Loop5 is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Levitated_Enter->Set_Options(TEXT("Levitated_Enter"), pModel);
		pAction_Levitated_Loop1->Set_Options(TEXT("Levitated_Loop_1"), pModel, true);
		pAction_Levitated_Loop2->Set_Options(TEXT("Levitated_Loop_2"), pModel, true);
		pAction_Levitated_Loop3->Set_Options(TEXT("Levitated_Loop_3"), pModel, true);
		pAction_Levitated_Loop4->Set_Options(TEXT("Levitated_Loop_4"), pModel, true);
		pAction_Levitated_Loop5->Set_Options(TEXT("Levitated_Loop_5"), pModel, true);
		pAction_Levitated_Land->Set_Options(TEXT("Levitated_Land"), pModel);
		pTsk_Wait_Loop1->Set_Timer(4.f);
		pTsk_Wait_Loop2->Set_Timer(4.f);
		pTsk_Wait_Loop3->Set_Timer(4.f);
		pTsk_Wait_Loop4->Set_Timer(4.f);
		pTsk_Wait_Loop5->Set_Timer(4.f);

		/* Assemble Behaviors */
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitated_Enter"), pAction_Levitated_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Enter");
		if (FAILED(Assemble_Behavior(TEXT("Random_Levitated_Loop"), pRandom_Levitated_Loop)))
			throw TEXT("Failed Assemble_Behavior Random_Levitated_Loop");
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitated_Land"), pAction_Levitated_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Land");

		if (FAILED(pRandom_Levitated_Loop->Assemble_Behavior(TEXT("Action_Levitated_Loop1"), pAction_Levitated_Loop1, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Loop1");
		if (FAILED(pRandom_Levitated_Loop->Assemble_Behavior(TEXT("Action_Levitated_Loop2"), pAction_Levitated_Loop2, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Loop2");
		if (FAILED(pRandom_Levitated_Loop->Assemble_Behavior(TEXT("Action_Levitated_Loop3"), pAction_Levitated_Loop3, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Loop3");
		if (FAILED(pRandom_Levitated_Loop->Assemble_Behavior(TEXT("Action_Levitated_Loop4"), pAction_Levitated_Loop4, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Loop4");
		if (FAILED(pRandom_Levitated_Loop->Assemble_Behavior(TEXT("Action_Levitated_Loop5"), pAction_Levitated_Loop5, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Loop5");

		if (FAILED(pAction_Levitated_Loop1->Assemble_Behavior(TEXT("Tsk_Wait_Loop1"), pTsk_Wait_Loop1)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
		if (FAILED(pAction_Levitated_Loop2->Assemble_Behavior(TEXT("Tsk_Wait_Loop2"), pTsk_Wait_Loop2)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
		if (FAILED(pAction_Levitated_Loop3->Assemble_Behavior(TEXT("Tsk_Wait_Loop3"), pTsk_Wait_Loop3)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
		if (FAILED(pAction_Levitated_Loop4->Assemble_Behavior(TEXT("Tsk_Wait_Loop4"), pTsk_Wait_Loop4)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
		if (FAILED(pAction_Levitated_Loop5->Assemble_Behavior(TEXT("Tsk_Wait_Loop5"), pTsk_Wait_Loop5)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Levitated] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_Levitated::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);

	if (BEHAVIOR_RUNNING == m_ReturnData &&	// 현재 공중에 뜸 상태가 진행중이었는데
		BEHAVIOR_RUNNING != result)			// 상위 노드에서 상태가 바뀐경우
	{
		_uint* pICurrentSpell = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
			return;

		if (*pICurrentSpell & BUFF_LEVIOSO)
			*pICurrentSpell ^= BUFF_LEVIOSO;
	}

	BEGININSTANCE;
	m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
	ENDINSTANCE;
}

CSequence_Levitated* CSequence_Levitated::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_Levitated* pInstance = new CSequence_Levitated(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_Levitated* CSequence_Levitated::Clone(void* pArg)
{
	CSequence_Levitated* pInstance = new CSequence_Levitated(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Levitated::Free()
{
	__super::Free();
}
