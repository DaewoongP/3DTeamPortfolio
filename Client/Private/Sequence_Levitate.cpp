#include "Sequence_Levitate.h"

#include "Client_GameInstance_Functions.h"

#include "Wait.h"
#include "Action.h"
#include "RigidMove.h"
#include "RigidBody.h"
#include "RandomChoose.h"
#include "Check_Distance.h"

CSequence_Levitate::CSequence_Levitate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_Levitate::CSequence_Levitate(const CSequence_Levitate& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_Levitate::Initialize(void* pArg)
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

	Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			_uint* pICurrentSpell = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
				return false;

			if (*pICurrentSpell & BUFF_LEVIOSO)
				*pICurrentSpell ^= BUFF_LEVIOSO;

			return true;
		});

	m_pRandom_Levitate_Loop = nullptr;
	if (FAILED(Create_Behavior(m_pRandom_Levitate_Loop)))
	{
		MSG_BOX("[CSequence_AirHit] Failed Initialize : Failed Create_Behavior m_pRandom_Select");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_Levitate::Assemble_Random_Select_Behavior(const wstring& wstrActionTag, const _float& fWeight, const _float& fLoopTime)
{
	CModel* pModel = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
		return E_FAIL;

	CAction* pAction = nullptr;
	if (FAILED(Create_Behavior(pAction)))
	{
		MSG_BOX("[CSequence_Levitate] Failed Assemble_Random_Select_Behavior\n : Failed Create_Behavior pAction");
		return E_FAIL;
	}
	CWait* pTsk_Wait = nullptr;
	if (FAILED(Create_Behavior(pTsk_Wait)))
	{
		MSG_BOX("[CSequence_Levitate] Failed Assemble_Random_Select_Behavior\n : Failed Create_Behavior pTsk_Wait");
		return E_FAIL;
	}

	pAction->Set_Options(wstrActionTag, pModel, true);
	pTsk_Wait->Set_Timer(fLoopTime);

	_uint iIndex = m_Behaviors.size();
	if (FAILED(m_pRandom_Levitate_Loop->Assemble_Behavior(wstrActionTag, pAction, fWeight)))
	{
		MSG_BOX("[CSequence_Levitate] Failed Assemble_Random_Select_Behavior\n : Failed Assemble_Behavior");
		return E_FAIL;
	}
	wstring wstrBehaviorTag = TEXT("Tsk_Wait_") + to_wstring(iIndex);
	if (FAILED(pAction->Assemble_Behavior(wstrBehaviorTag, pTsk_Wait)))
	{
		MSG_BOX("[CSequence_Levitate] Failed Assemble_Random_Select_Behavior\n : Failed Assemble_Behavior");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_Levitate::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CModel* pModel = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pModel", pModel)))
			throw TEXT("pModel is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Levitate_Enter = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Enter");
		CAction* pAction_Levitate_Fall = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Fall)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Fall");
		CAction* pAction_Levitate_Land = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Land)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Land");

		CRigidMove* pTsk_Up = nullptr;
		if (FAILED(Create_Behavior(pTsk_Up)))
			throw TEXT("Failed Create_Behavior pTsk_Up");
		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorations */
		pAction_Levitate_Enter->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CRigidBody* pRigidBody = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
					return false;
				return true;
			});
		m_pRandom_Levitate_Loop->Add_Change_Condition(BEHAVIOR_FAIL, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});
		m_pRandom_Levitate_Loop->Add_Change_Condition(BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		CTransform* pTransform = { nullptr };
		CRigidBody* pRigidBody = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pTransform", pTransform)))
			throw TEXT("pTransform is Invalid");
		if (FAILED(m_pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
			throw TEXT("pRigidBody is Invalid");
		
		pAction_Levitate_Enter->Set_Options(TEXT("Levitate_Enter"), pModel);
		pAction_Levitate_Fall->Set_Options(TEXT("Levitate_Fall_Loop"), pModel, true);
		pAction_Levitate_Land->Set_Options(TEXT("Levitate_Land"), pModel);
		_float3 vForce = { 0.f, m_fUpForce, 0.f };
		pTsk_Up->Set_Option(pRigidBody, vForce, m_fUpTime);
		pTsk_Wait->Set_Timer(0.5f);

		/* Assemble Behaviors */
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitate_Enter"), pAction_Levitate_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Enter");
		if (FAILED(Assemble_Behavior(TEXT("Random_Levitate_Loop"), m_pRandom_Levitate_Loop)))
			throw TEXT("Failed Assemble_Behavior Random_Levitate_Loop");
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitate_Fall"), pAction_Levitate_Fall)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Fall");
		if (FAILED(Assemble_Behavior(TEXT("Action_Levitate_Land"), pAction_Levitate_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Land");

		if (FAILED(pAction_Levitate_Enter->Assemble_Behavior(TEXT("Tsk_Up"), pTsk_Up)))
			throw TEXT("Failed Assemble_Behavior Tsk_Up");
		if (FAILED(pAction_Levitate_Fall->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Levitate] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_Levitate::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);
	m_iterCurBehavior = m_Behaviors.begin();

	if (BEHAVIOR_RUNNING == m_ReturnData &&	// 현재 공중에 뜸 상태가 진행중이었는데
		BEHAVIOR_RUNNING != result)			// 상위 노드에서 상태가 바뀐경우
	{
		m_ReturnData = result;
	}
}

CSequence_Levitate* CSequence_Levitate::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_Levitate* pInstance = new CSequence_Levitate(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_Levitate* CSequence_Levitate::Clone(void* pArg)
{
	CSequence_Levitate* pInstance = new CSequence_Levitate(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Levitate::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pRandom_Levitate_Loop);
	}
}
