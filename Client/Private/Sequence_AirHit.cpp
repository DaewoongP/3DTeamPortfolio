#include "Sequence_AirHit.h"

#include "Client_GameInstance_Functions.h"
#include "BlackBoard.h"

#include "Turn.h"
#include "Action.h"
#include "RandomChoose.h"
#include "Check_Distance.h"

CSequence_AirHit::CSequence_AirHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_AirHit::CSequence_AirHit(const CSequence_AirHit& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_AirHit::Initialize(void* pArg)
{
	m_pRandomChoose = nullptr;
	if (FAILED(Create_Behavior(m_pRandomChoose)))
	{
		MSG_BOX("[CSequence_AirHit] Failed Initialize : Failed Create_Behavior m_pRandomChoose");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_AirHit::Tick_Behavior(const _float& fTimeDelta)
{
	return __super::Tick_Behavior(fTimeDelta);
}

HRESULT CSequence_AirHit::Assemble_Behavior(const _float& fWeight)
{
	CAction* pAction_AirHit = nullptr;

	if (FAILED(Create_Behavior(pAction_AirHit)))
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Behavior\n : Failed Create_Behavior pAction_AirHit");
		return E_FAIL;
	}
	_uint iIndex = m_Behaviors.size();
	wstring wstrBehaviorTag = TEXT("Action_AirHit_") + to_wstring(iIndex);

	return m_pRandomChoose->Assemble_Behavior(wstrBehaviorTag, pAction_AirHit, fWeight);
}

HRESULT CSequence_AirHit::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		/* Make Child Behaviors */
		CSelector* pSelector_AirHit = nullptr;
		if (FAILED(Create_Behavior(pSelector_AirHit)))
			throw TEXT("Failed Create_Behavior pSelector_AirHit");

		/* 막타 맞고 날라가는 액션 */
		CSequence* pSequence_Knockback = nullptr;
		if (FAILED(Create_Behavior(pSequence_Knockback)))
			throw TEXT("Failed Create_Behavior pSequence_Knockback");

		/* Set Decorator*/
		m_pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_LIGHT & *pICurrentSpell)
					return true;

				return false;
			});
		pSequence_Knockback->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_HEAVY & *pICurrentSpell)
					return true;

				return false;
			});

		/* Assemble Childs */
		if (FAILED(__super::Assemble_Behavior(TEXT("Selector_AirHit"), pSelector_AirHit)))
			throw TEXT("Failed Assemble_Behavior Selector_AirHit");

		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("RandomChoose"), m_pRandomChoose)))
			throw TEXT("Failed Assemble_Behavior RandomChoose");
		Safe_AddRef(m_pRandomChoose);
		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("Sequence_Knockback"), pSequence_Knockback)))
			throw TEXT("Failed Assemble_Behavior Sequence_Knockback");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_AirHit] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_AirHit::Reset_Behavior(HRESULT result)
{
	(*m_iterCurBehavior)->Reset_Behavior(result);
}

CSequence_AirHit* CSequence_AirHit::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_AirHit* pInstance = New CSequence_AirHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_AirHit* CSequence_AirHit::Clone(void* pArg)
{
	CSequence_AirHit* pInstance = New CSequence_AirHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_AirHit::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pRandomChoose);
	}
}
