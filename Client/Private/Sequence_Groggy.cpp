#include "Sequence_Groggy.h"

#include "Client_GameInstance_Functions.h"

#include "Wait.h"
#include "Action.h"

CSequence_Groggy::CSequence_Groggy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_Groggy::CSequence_Groggy(const CSequence_Groggy& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_Groggy::Initialize(void* pArg)
{
	/* 스펠 확인 */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (BUFF_STUPEFY & *pICurrentSpell)
				return true;

			return false;
		});

	Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (*pICurrentSpell & BUFF_STUPEFY)
				*pICurrentSpell ^= BUFF_STUPEFY;

			return true;
		});

	return S_OK;
}

HRESULT CSequence_Groggy::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	if (BEHAVIOR_SUCCESS == hr)
	{
		_uint* pICurrentSpell = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
			return E_FAIL;

		if(*pICurrentSpell & BUFF_STUPEFY)
			*pICurrentSpell ^= BUFF_STUPEFY;
	}

	return hr;
}

HRESULT CSequence_Groggy::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CModel* pModel = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
			throw TEXT("pModel is nullptr");

		CAction* pAction_Enter = nullptr;
		if (FAILED(Create_Behavior(pAction_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Enter");
		CAction* pAction_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Loop");
		CAction* pAction_End = nullptr;
		if (FAILED(Create_Behavior(pAction_End)))
			throw TEXT("Failed Create_Behavior pAction_End");

		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorations */

		/* Set Options */
		pAction_Enter->Set_Options(TEXT("Groggy_Enter"), pModel);
		pAction_Loop->Set_Options(TEXT("Groggy_Loop"), pModel, true);
		pAction_End->Set_Options(TEXT("Groggy_End"), pModel);
		pTsk_Wait->Set_Timer(m_fLoopTime);

		/* Assemble Childs */
		if (FAILED(Assemble_Behavior(TEXT("Action_Enter"), pAction_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Enter");
		if (FAILED(Assemble_Behavior(TEXT("Action_Loop"), pAction_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Loop");
		if (FAILED(Assemble_Behavior(TEXT("Action_End"), pAction_End)))
			throw TEXT("Failed Assemble_Behavior Action_End");

		if (FAILED(pAction_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Groggy] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_Groggy::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);

	if (BEHAVIOR_RUNNING == m_ReturnData &&	//현재 기절 상태가 진행중이었는데
		BEHAVIOR_RUNNING != result)			//상위 노드에서 상태가 바뀐경우
	{
		_uint* pICurrentSpell = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
			return;

		if (*pICurrentSpell & BUFF_STUPEFY)
			*pICurrentSpell ^= BUFF_STUPEFY;
	}

	BEGININSTANCE;
	m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
	ENDINSTANCE;
}

CSequence_Groggy* CSequence_Groggy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_Groggy* pInstance = New CSequence_Groggy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_Groggy* CSequence_Groggy::Clone(void* pArg)
{
	CSequence_Groggy* pInstance = New CSequence_Groggy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Groggy::Free()
{
	__super::Free();
}
