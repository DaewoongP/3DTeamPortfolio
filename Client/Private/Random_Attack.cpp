#include "Random_Attack.h"

#include "GameInstance.h"
#include "BlackBoard.h"

#include "Turn.h"
#include "Magic.h"
#include "Action.h"
#include "Check_Degree.h"
#include "Check_Distance.h"
#include "Sequence_Attack.h"

CRandom_Attack::CRandom_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CRandomChoose(pDevice, pContext)
{
}

CRandom_Attack::CRandom_Attack(const CRandom_Attack& rhs)
	: CRandomChoose(rhs)
{
}

HRESULT CRandom_Attack::Initialize(void* pArg)
{
	/* ÄðÅ¸ÀÓ */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CRandom_Attack::Tick(const _float& fTimeDelta)
{
	HRESULT hr = __super::Tick(fTimeDelta);

	if (BEHAVIOR_RUNNING != hr)
	{
		Set_Random_Behavior();
	}

	return hr;
}

HRESULT CRandom_Attack::Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight)
{
	if (nullptr == dynamic_cast<CSequence_Attack*>(pBehavior))
	{
		MSG_BOX("[CRandom_Attack] Failed Assemble_Behavior : Behavior is not CSequence_Attack");
		return E_FAIL;
	}
	
	return __super::Assemble_Behavior(BehaviorTag, pBehavior, fWeight);
}

void CRandom_Attack::Reset_Behavior(HRESULT result)
{
	_uint* pICurrentSpell = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
		return;

	(*m_iterCurBehavior)->Reset_Behavior(result);
	if (CMagic::BUFF_NONE != *pICurrentSpell)
	{
		Set_Random_Behavior();
	}
}

CRandom_Attack* CRandom_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRandom_Attack* pInstance = New CRandom_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRandom_Attack* CRandom_Attack::Clone(void* pArg)
{
	CRandom_Attack* pInstance = New CRandom_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandom_Attack::Free()
{
	__super::Free();
}
