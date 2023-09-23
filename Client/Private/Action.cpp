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

HRESULT CAction::Initialize(void* pArg)
{
	/* 쿨타임 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	/* 한번만 실행할 액션인지 */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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
	if (false == Check_Decorations())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	/* 행동이 하나라도 있으면 행동체크 활성화 */
	if (0 < m_Behaviors.size())
		m_isFinishBehaviors = true;

	if (true == m_isFirst)
	{
		m_isFirst = false;
		m_pModel->Change_Animation(m_wstrAnimationTag, m_eAnimationType);
		for (auto& pBehavior : m_Behaviors)
			pBehavior->Reset_Behavior(BEHAVIOR_END);
	}

	/* 행동 체크 */
	for (auto& pBehavior : m_Behaviors)
	{
		HRESULT hr = pBehavior->Tick(fTimeDelta);
		/* 하나라도 동작중이면 계속 진행 */
		if (BEHAVIOR_RUNNING == hr)
			m_isFinishBehaviors = false;

#ifdef _DEBUG
		pBehavior->Set_ReturnData(hr);
#endif // _DEBUG
	}

	_bool* pIsChangeAnimation = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("isChangeAnimation", pIsChangeAnimation)))
	{
		MSG_BOX("isChangeAnimation is nullptr");
		return E_FAIL;
	}

	_bool bCheck = { false };

	if (true == *pIsChangeAnimation ||								// 애니메이션을 변경하라는 노티파이가 울렸거나
		true == m_pModel->Is_Finish_Animation() ||					// 애니메이션이 끝났거나
		(true == m_isCheckBehavior && true == m_isFinishBehaviors)) // 행동체크를 할건데 모든 행동이 끝났으면
	{
		m_isEndFirstPlay = true;
		bCheck = true;	// 나가
	}

	if (true == bCheck)
		return BEHAVIOR_SUCCESS;

	return BEHAVIOR_RUNNING;
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
		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;
	}
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
