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
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			BEGININSTANCE;
			_float fInterval = pGameInstance->Get_World_TimeAcc() - m_fPreWorldTimeAcc;
			ENDINSTANCE;

			if (m_fLimit > fInterval)
				return false;

			return true;
		});

	/* �ѹ��� ������ �׼����� */
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
		m_isFinishBehaviors = false;
		return BEHAVIOR_FAIL;
	}

	_bool isFirst = { false };
	if (FAILED(m_pBlackBoard->Get_Type("isFirst", isFirst)))
		return E_FAIL;

	/* �ൿ�� �ϳ��� ������ �ൿüũ Ȱ��ȭ */
	if (0 < m_Behaviors.size())
		m_isFinishBehaviors = true;

	if (true == isFirst)
	{
		m_isPlayAction = true;
		m_pModel->Change_Animation(m_wstrAnimationTag);

		if (FAILED(m_pBlackBoard->Set_Type("isFirst", false)))
			return E_FAIL;

		if (0 < m_wstrTimerTag.size())
		{
			BEGININSTANCE;
			pGameInstance->Reset_Timer(m_wstrTimerTag);
			ENDINSTANCE;
		}
	}

	/* �ð� üũ */
	_bool isRunOutOfTime = { false };
	if (0 < m_wstrTimerTag.size())
	{
		BEGININSTANCE;
		isRunOutOfTime = pGameInstance->Check_Timer(m_wstrTimerTag);
		ENDINSTANCE;
	}

	/* �ൿ üũ */
	for (auto& pBehavior : m_Behaviors)
	{
		HRESULT hr = pBehavior->Tick(fTimeDelta);
		/* �ϳ��� �������̸� ��� ���� */
		if (BEHAVIOR_RUNNING == hr)
			m_isFinishBehaviors = false;

#ifdef _DEBUG
		pBehavior->Set_ReturnData(hr);
#endif // _DEBUG
	}

	_bool bCheck = { false };

	if (true == m_pModel->Is_Finish_Animation() ||					// �ִϸ��̼��� �����ų�
		true == isRunOutOfTime ||									// �ð��ʰ��߰ų� 
		(true == m_isCheckBehavior && true == m_isFinishBehaviors)) // �ൿüũ�� �Ұǵ� ��� �ൿ�� ��������
	{
		bCheck = true;	// ����
	}

	if (true == bCheck)
	{
		if (FAILED(m_pBlackBoard->Set_Type("isFirst", true)))
			return E_FAIL;

		m_isFinishBehaviors = false;

		BEGININSTANCE;
		m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
		ENDINSTANCE;

		return BEHAVIOR_SUCCESS;
	}

	return BEHAVIOR_RUNNING;
}

void CAction::Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
	_bool _isCheckBehavior, const _float& _fCoolTime,
	const wstring& _wstrTimerTag, const _float& _fDurationTime,
	_bool _isOneTimeAction, _bool _isLerp)
{
	if (nullptr == _pModel)
	{
		MSG_BOX("[CAction] _pModel is nullptr");
		return;
	}

	if (0 < _wstrTimerTag.size())
	{
		BEGININSTANCE;
		m_wstrTimerTag = _wstrTimerTag;
		pGameInstance->Add_Timer(_wstrTimerTag, false, _fDurationTime);
		ENDINSTANCE;
	}

	m_pModel = _pModel;
	Safe_AddRef(m_pModel);

	m_wstrAnimationTag = _wstrAnimationTag;
	m_fLimit = _fCoolTime;
	m_isOneTimeAction = _isOneTimeAction;
	m_isCheckBehavior = _isCheckBehavior;

	m_pModel->Get_Animation(m_wstrAnimationTag)->Set_LerpAnim(_isLerp);
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
