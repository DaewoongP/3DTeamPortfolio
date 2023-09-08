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
				true == m_isPlayAction)
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

	if (true == m_isFirst)
	{
		m_isPlayAction = true;
		m_pModel->Reset_Animation(0);
		m_isFirst = false;
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

	_bool bCheck = { false };

	if (true == m_pModel->Is_Able_Change_Animation() || 
		true == m_isFinishBehaviors)
	{
		bCheck = true;
	}

	if (true == bCheck)
	{
		m_isFirst = true;
		m_isFinishBehaviors = false;

		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

void CAction::Set_Options(const wstring& _strAnimationTag, CModel* _pModel, const _float& _fCoolTime, _bool _isOneTimeAction, _bool _isLerp)
{
	if (nullptr == _pModel)
	{
		MSG_BOX("[CAction] _pModel is nullptr");
		return;
	}

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
