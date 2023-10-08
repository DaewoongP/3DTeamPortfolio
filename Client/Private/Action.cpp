#include "Action.h"

#include "GameInstance.h"
#include "BlackBoard.h"

CAction::CAction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CAction::CAction(const CAction& rhs)
	: CBehavior(rhs)
{
}

HRESULT CAction::Initialize(void* pArg)
{
	/* ��Ÿ�� */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;
			
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	/* �ѹ��� ������ �׼����� */
	Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_isOneTimeAction &&
				true == m_isEndFirstPlay)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CAction::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorators())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		Check_End_Decorators();
		Check_Fail_Decorators();
		return BEHAVIOR_FAIL;
	}

	/* �ൿ�� �ϳ��� ������ �ൿüũ Ȱ��ȭ */
	if (0 < m_Behaviors.size())
		m_isFinishBehaviors = true;

	if (true == m_isFirst)
	{
		m_isFirst = false;
		m_pModel->Change_Animation(m_wstrAnimationTag, m_eAnimationType);
		for (auto& pBehavior : m_Behaviors)
			pBehavior->Reset_Behavior(BEHAVIOR_END);
	}

	/* �ൿ üũ */
	for (auto& pBehavior : m_Behaviors)
	{
		HRESULT hr = pBehavior->Tick(fTimeDelta);
		/* �ϳ��� �������̸� ��� ���� */
		if (BEHAVIOR_RUNNING == hr)
			m_isFinishBehaviors = false;
	}

	if (true == Is_Success_Action())
	{
		Check_End_Decorators();
		Check_Success_Decorators();
		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

void CAction::Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
	_bool _isCheckBehavior, const _float& _fCoolTime,
	_bool _isOneTimeAction, _bool _isLerp,
	CModel::ANIMTYPE eType)
{
	if (nullptr == _pModel)
	{
		MSG_BOX("[CAction] _pModel is nullptr");
		return;
	}

	m_pModel = _pModel;
	Safe_AddRef(m_pModel);

	m_wstrAnimationTag = _wstrAnimationTag;
	m_fLimit = _fCoolTime;
	m_isOneTimeAction = _isOneTimeAction;
	m_isCheckBehavior = _isCheckBehavior;

	m_pModel->Get_Animation(m_wstrAnimationTag)->Set_LerpAnim(_isLerp);
	m_eAnimationType = eType;
}

HRESULT CAction::Add_Exit_Condition(function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func)
		return E_FAIL;

	CDecorator* pDecoration = CDecorator::Create(Func);
	if (nullptr == pDecoration)
		return E_FAIL;

	m_ConditionFunctions.push_back(pDecoration);

	return S_OK;
}

void CAction::Reset_Behavior(HRESULT result)
{
	m_isFirst = true;
	m_isFinishBehaviors = false;

	_bool* pIsChangeAnimation = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("isChangeAnimation", pIsChangeAnimation)))
	{
		MSG_BOX("isChangeAnimation is nullptr");
		return;
	}
	*pIsChangeAnimation = false;

	if (BEHAVIOR_SUCCESS == result)
	{
		for (auto& pBehavior : m_Behaviors)
			pBehavior->Reset_Behavior(result);

		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;
	}
}

_bool CAction::Is_Success_Action()
{
	_bool* pIsChangeAnimation = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("isChangeAnimation", pIsChangeAnimation)))
	{
		MSG_BOX("isChangeAnimation is nullptr");
		return false;
	}

	_bool isSuccess = { false };
	// �ִϸ��̼��� �����϶�� ��Ƽ���̰� �︰ ���
	if (true == *pIsChangeAnimation)
		isSuccess = true;

	// �ִϸ��̼��� ���� ���
	if (true == m_pModel->Is_Finish_Animation())
		isSuccess = true;

	// ���� ������ Ż�����ǿ� ������ ���
	if (true == Check_Exit_Conditions())
		isSuccess = true;

	// �ൿüũ�� �Ұǵ� ��� �ൿ�� ���� ���
	if (true == m_isCheckBehavior && 
		true == m_isFinishBehaviors)
		isSuccess = true;

	if(true == isSuccess)
	{
		m_isEndFirstPlay = true;
		return true;
	}

	return isSuccess;
}

_bool CAction::Check_Exit_Conditions()
{
	if (0 == m_ConditionFunctions.size())
		return false;

	for (auto& Deco : m_ConditionFunctions)
	{
		if (false == Deco->Is_Execute(m_pBlackBoard))
			return false;
	}

	return true;
}

CAction* CAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction* pInstance = New CAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAction* CAction::Clone(void* pArg)
{
	CAction* pInstance = New CAction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModel);
	}
}
