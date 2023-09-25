#include "Sequence_Attack.h"

#include "GameInstance.h"
#include "BlackBoard.h"

#include "Turn.h"
#include "Action.h"
#include "Random_Select.h"
#include "Check_Distance.h"

CSequence_Attack::CSequence_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_Attack::CSequence_Attack(const CSequence_Attack& rhs)
	: CSequence(rhs)
{
}

void CSequence_Attack::Set_Attack_Action_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
	_bool _isCheckBehavior, const _float& _fCoolTime,
	_bool _isOneTimeAction, _bool _isLerp)
{
	if (nullptr == m_pAction)
		return;

	m_pAction->Set_Options(_wstrAnimationTag, _pModel,
		_isCheckBehavior, _fCoolTime,
		_isOneTimeAction, _isLerp);
}

HRESULT CSequence_Attack::Initialize(void* pArg)
{
	/* ÄðÅ¸ÀÓ */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	BEGININSTANCE;
	m_pAction = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
	ENDINSTANCE;
	if (nullptr == m_pAction)
	{
		MSG_BOX("[CSequence_Attack] Failed Initialize : m_pAction is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_Attack::Tick(const _float& fTimeDelta)
{
	if (FAILED(m_pBlackBoard->Set_Type("fAttackRange", m_fAttackRange)))
		return E_FAIL;

	return __super::Tick(fTimeDelta);
}

HRESULT CSequence_Attack::Assemble_Childs()
{
	BEGININSTANCE;

	try
	{
		CTransform* pTransform = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pTransform", pTransform)))
			throw TEXT("pTransform is nullptr");

		CCheck_Distance* pTsk_Check_Distance_Enter = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pTsk_Check_Distance_Enter)
			throw TEXT("pTsk_Check_Distance_Enter is nullptr");
		CCheck_Distance* pTsk_Check_Distance_Exit = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pTsk_Check_Distance_Exit)
			throw TEXT("pTsk_Check_Distance_Exit is nullptr");
		CTurn* pTsk_Turn = dynamic_cast<CTurn*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Turn")));
		if (nullptr == pTsk_Turn)
			throw TEXT("pTsk_Turn is nullptr");

		/* Set Decorations */

		/* Set Options */
		pTsk_Turn->Set_Option(60.f, 3.f);
		pTsk_Turn->Set_Transform(pTransform);
		pTsk_Check_Distance_Enter->Set_Transform(pTransform);
		pTsk_Check_Distance_Exit->Set_Transform(pTransform);

		if (FAILED(Assemble_Behavior(TEXT("Tsk_Check_Distance_Enter"), pTsk_Check_Distance_Enter)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Distance_Enter");
		if (FAILED(Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Tsk_Turn");
		if (FAILED(Assemble_Behavior(TEXT("Action_Attack"), m_pAction)))
			throw TEXT("Action_Attack");
		if (FAILED(Assemble_Behavior(TEXT("Tsk_Check_Distance_Exit"), pTsk_Check_Distance_Exit)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Distance_Exit");
		Safe_AddRef(m_pAction);
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_Attack] Failed Assemble_Childs : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}
	
	ENDINSTANCE;

	return S_OK;
}

void CSequence_Attack::Reset_Behavior(HRESULT result)
{
	m_pAction->Reset_Behavior(result);
	(*m_iterCurBehavior)->Reset_Behavior(result);

	m_isPlayAttack = false;

	BEGININSTANCE;
	m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
	ENDINSTANCE;
}

CSequence_Attack* CSequence_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_Attack* pInstance = New CSequence_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_Attack* CSequence_Attack::Clone(void* pArg)
{
	CSequence_Attack* pInstance = New CSequence_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_Attack::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pAction);
	}
}
