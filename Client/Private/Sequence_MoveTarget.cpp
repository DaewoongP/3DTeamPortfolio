#include "Sequence_MoveTarget.h"

#include "Client_GameInstance_Functions.h"
#include "BlackBoard.h"

#include "Turn.h"
#include "Action.h"
#include "Check_Degree.h"
#include "Check_Distance.h"

CSequence_MoveTarget::CSequence_MoveTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSequence(pDevice, pContext)
{
}

CSequence_MoveTarget::CSequence_MoveTarget(const CSequence_MoveTarget& rhs)
	: CSequence(rhs)
{
}

HRESULT CSequence_MoveTarget::Initialize(void* pArg)
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

	m_pAction = nullptr;
	if (FAILED(Create_Behavior(m_pAction)))
	{
		MSG_BOX("[CSequence_MoveTarget] Failed Initialize : Failed Create_Behavior m_pAction");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSequence_MoveTarget::Tick(const _float& fTimeDelta)
{
	return __super::Tick(fTimeDelta);
}

HRESULT CSequence_MoveTarget::Assemble_Childs()
{
	BEGININSTANCE;

	try /* Failed Check Make_Move */
	{
		CTransform* pTransform = { nullptr };
		if (FAILED(m_pBlackBoard->Get_Type("pTransform", pTransform)))
			throw TEXT("pTransform is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CCheck_Distance* pTsk_Check_Distance = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Distance)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Distance");
		CTurn* pTsk_Turn = nullptr;
		if (FAILED(Create_Behavior(pTsk_Turn)))
			throw TEXT("Failed Create_Behavior pTsk_Turn");

		/* Set Options */
		pTsk_Check_Degree->Set_Transform(pTransform);
		pTsk_Check_Distance->Set_Transform(pTransform);
		pTsk_Turn->Set_Transform(pTransform);

		/* Assemble Behaviors */
		if (FAILED(Assemble_Behavior(TEXT("Action_Move"), m_pAction)))
			throw TEXT("Failed Assemble_Behavior Action_Move");
		Safe_AddRef(m_pAction);

		if (FAILED(m_pAction->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(m_pAction->Assemble_Behavior(TEXT("Tsk_Check_Distance"), pTsk_Check_Distance)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Distance");
		if (FAILED(m_pAction->Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Failed Assemble_Behavior Tsk_Turn");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CSequence_MoveTarget] Failed Make_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CSequence_MoveTarget::Set_Action_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
	_bool _isCheckBehavior, const _float& _fCoolTime,
	_bool _isOneTimeAction, _bool _isLerp)
{
	if (nullptr == m_pAction)
		return;

	m_pAction->Set_Options(_wstrAnimationTag, _pModel,
		_isCheckBehavior, _fCoolTime,
		_isOneTimeAction, _isLerp);
}

CSequence_MoveTarget* CSequence_MoveTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSequence_MoveTarget* pInstance = New CSequence_MoveTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSequence_MoveTarget* CSequence_MoveTarget::Clone(void* pArg)
{
	CSequence_MoveTarget* pInstance = New CSequence_MoveTarget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSequence_MoveTarget::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pAction);
	}
}
