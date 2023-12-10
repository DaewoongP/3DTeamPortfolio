#include "Sequence_Descendo.h"

#include "Client_GameInstance_Functions.h"

#include "Wait.h"
#include "Action.h"

CSequence_Descendo::CSequence_Descendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_Descendo::CSequence_Descendo(const CSequence_Descendo& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_Descendo::Initialize(void* pArg)
{
	/* 스펠 확인 */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (BUFF_DESCENDO & *pICurrentSpell)
				return true;

			return false;
		});

	Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (*pICurrentSpell & BUFF_DESCENDO)
				*pICurrentSpell ^= BUFF_DESCENDO;

			return true;
		});

	return S_OK;
}

HRESULT CSequence_Descendo::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CModel* pModel = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
			throw TEXT("pModel is nullptr");

		CAction* pAction_Descendo = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo)))
			throw TEXT("Failed Create_Behavior pAction_Descendo");
		CAction* pAction_GetUp = nullptr;
		if (FAILED(Create_Behavior(pAction_GetUp)))
			throw TEXT("Failed Create_Behavior pAction_GetUp");

		/* Set Decorator */

		/* Set Options */
		pAction_Descendo->Set_Options(TEXT("Descendo_Ground"), pModel);
		pAction_GetUp->Set_Options(TEXT("Getup_Front"), pModel);

		/* Assemble Behaviors */
		if (FAILED(Assemble_Behavior(TEXT("Action_Descendo"), pAction_Descendo)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo");
		if (FAILED(Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Descendo] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_Descendo::Reset_Behavior(HRESULT result)
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

CSequence_Descendo* CSequence_Descendo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_Descendo* pInstance = New CSequence_Descendo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_Descendo* CSequence_Descendo::Clone(void* pArg)
{
	CSequence_Descendo* pInstance = New CSequence_Descendo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Descendo::Free()
{
	__super::Free();
}
