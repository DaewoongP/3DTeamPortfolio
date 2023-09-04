#include "Action.h"

#include "GameInstance.h"
#include "BlackBoard.h"
#include "Model.h"

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
	/* 쿨타임 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_float* pTimeAcc = { nullptr };
			pBlackBoard->Get_Type("fTimeAcc", pTimeAcc);
			if (nullptr == pTimeAcc)
			{
				MSG_BOX("is Not fTimeAcc");
				return false;
			}

			_float fInterval = *pTimeAcc - m_fPreTimeAcc;
			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			if (true == m_isOneTimeAction &&
				false == m_isFirstAction)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CAction::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
	{
		m_isFirst = true;
		m_isFinishBehaviors = false;
		return BEHAVIOR_FAIL;
	}

	/* 행동이 하나라도 있으면 행동체크 활성화 */
	if (0 != m_Behaviors.size())
		m_isFinishBehaviors = true;

	m_fTimeAcc += fTimeDelta;

	if (true == m_isFirst)
	{
		//m_pModel->Change_Animation(m_strAnimationTag, m_isLerp);
		m_isFirst = false;
	}

	/* 행동 체크 */
	for (auto& pBehavior : m_Behaviors)
	{
		HRESULT hr = pBehavior->Tick(fTimeDelta);
		/* 하나라도 동작중이면 멈추지마 */
		if (BEHAVIOR_RUNNING == hr)
			m_isFinishBehaviors = false;
	}

	_bool bCheck = { false };

	/*if (false == m_pModel->isLoopAnimation())
	{
		if (true == m_pModel->isAbleChangeAnimation() ||
			true == m_pModel->isFinishedAnimation())
			bCheck = true;
	}

	else if (true == m_isFinishBehaviors)
	{
		bCheck = true;
	}*/

	if (true == bCheck)
	{
		if (true == m_isOneTimeAction)
			m_isFirstAction = false;

		m_isFirst = true;
		m_isFinishBehaviors = false;

		_float* pTimeAcc = { nullptr };
		m_pBlackBoard->Get_Type("fTimeAcc", pTimeAcc);
		if (nullptr == pTimeAcc)
			return E_FAIL;

		m_fTimeAcc = 0.f;
		m_fPreTimeAcc = *pTimeAcc;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

void CAction::Set_Options(const wstring& _strAnimationTag, CModel* _pModel, const _float& _fCoolTime, _bool _isOneTimeAction, _bool _isLerp)
{
	m_pModel = _pModel;
	Safe_AddRef(m_pModel);

	m_wstrAnimationTag = _strAnimationTag;
	m_fLimit = _fCoolTime;
	m_isOneTimeAction = _isOneTimeAction;
	m_isLerp = _isLerp;
}

CAction* CAction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction* pInstance = new CAction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAction* CAction::Clone(void* pArg)
{
	CAction* pInstance = new CAction(*this);

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
