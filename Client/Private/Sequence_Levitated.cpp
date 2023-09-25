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
	/* 스펠 확인 */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (BUFF_LEVIOSO & *pICurrentSpell)
				return true;

			return false;
		});

	BEGININSTANCE;
	m_pRandom_Levitated_Loop = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
	ENDINSTANCE;
	if (nullptr == m_pRandom_Levitated_Loop)
	{
		MSG_BOX("[CSequence_AirHit] Failed Initialize : m_pRandom_Select is nullptr");
		return E_FAIL;
	}

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

HRESULT CSequence_Levitated::Assemble_Random_Select_Behavior(const wstring& wstrActionTag, const _float& fWeight, const _float& fLoopTime)
{
	CModel* pModel = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
		return E_FAIL;

	BEGININSTANCE;

	CAction* pAction = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
	if (nullptr == pAction)
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Random_Select_Behavior\n : pAction is nullptr");
		ENDINSTANCE;
		return E_FAIL;
	}
	CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
	if (nullptr == pTsk_Wait)
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Random_Select_Behavior\n : pTsk_Wait is nullptr");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	pAction->Set_Options(wstrActionTag, pModel, true);
	pTsk_Wait->Set_Timer(fLoopTime);
	
	_uint iIndex = m_Behaviors.size();
	if (FAILED(m_pRandom_Levitated_Loop->Assemble_Behavior(wstrActionTag, pAction, fWeight)))
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Random_Select_Behavior\n : Failed Assemble_Behavior");
		return E_FAIL;
	}
	wstring wstrBehaviorTag = TEXT("Tsk_Wait_") + to_wstring(iIndex);
	if (FAILED(pAction->Assemble_Behavior(wstrBehaviorTag, pTsk_Wait)))
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Random_Select_Behavior\n : Failed Assemble_Behavior");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_Levitated::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CModel* pModel = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
			throw TEXT("pModel is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Levitated_Enter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Enter)
			throw TEXT("pAction_Levitated_Enter is nullptr");
		CAction* pAction_Levitated_Land = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Levitated_Land)
			throw TEXT("pAction_Levitated_Land is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Levitated_Enter->Set_Options(TEXT("Levitated_Enter"), pModel);
		pAction_Levitated_Land->Set_Options(TEXT("Levitated_Land"), pModel);

		/* Assemble Behaviors */
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitated_Enter"), pAction_Levitated_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Enter");
		if (FAILED(Assemble_Behavior(TEXT("Random_Levitated_Loop"), m_pRandom_Levitated_Loop)))
			throw TEXT("Failed Assemble_Behavior Random_Levitated_Loop");
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitated_Land"), pAction_Levitated_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Levitated_Land");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Levitated] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

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
		m_ReturnData = result;

		_uint* pICurrentSpell = { nullptr };
		_uint* pIPreviusSpell = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
			return;
		if (FAILED(m_pBlackBoard->Get_Type("iPreviusSpell", pIPreviusSpell)))
			return;

		if (*pICurrentSpell & BUFF_LEVIOSO)
		{
			*pIPreviusSpell = *pICurrentSpell;
			*pICurrentSpell ^= BUFF_LEVIOSO;
		}
	}
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
	if (true == m_isCloned)
	{
		Safe_Release(m_pRandom_Levitated_Loop);
	}
}
