#include "Sequence_AirHit.h"

#include "GameInstance.h"
#include "BlackBoard.h"

#include "Turn.h"
#include "Action.h"
#include "Random_AirHit.h"
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
	BEGININSTANCE;
	m_pRandom_AirHit = dynamic_cast<CRandom_AirHit*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_AirHit")));
	ENDINSTANCE;
	if (nullptr == m_pRandom_AirHit)
	{
		MSG_BOX("[CSequence_AirHit] Failed Initialize : m_pRandom_Select is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_AirHit::Tick(const _float& fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

HRESULT CSequence_AirHit::Assemble_Behavior(const _float& fWeight)
{
	BEGININSTANCE;
	CAction* pAction_AirHit = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
	ENDINSTANCE;
	if (nullptr == pAction_AirHit)
	{
		MSG_BOX("[CSequence_AirHit] Failed Assemble_Behavior\n : pAction_AirHit is nullptr");
		return E_FAIL;
	}
	_uint iIndex = m_Behaviors.size();
	wstring wstrBehaviorTag = TEXT("Action_AirHit_") + to_wstring(iIndex);

	return m_pRandom_AirHit->Assemble_Behavior(wstrBehaviorTag, pAction_AirHit, fWeight);
}

HRESULT CSequence_AirHit::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		/* Make Child Behaviors */
		CSelector* pSelector_AirHit = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_AirHit)
			throw TEXT("pSelector_AirHit is nullptr");

		/* 막타 맞고 날라가는 액션 */
		CSequence* pSequence_Knockback = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Knockback)
			throw TEXT("pSequence_Knockback is nullptr");

		/* Set Decorator*/
		m_pRandom_AirHit->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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

		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("RandomSelect_AirHit"), m_pRandom_AirHit)))
			throw TEXT("Failed Assemble_Behavior RandomSelect_AirHit");
		Safe_AddRef(m_pRandom_AirHit);
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
		Safe_Release(m_pRandom_AirHit);
	}
}
