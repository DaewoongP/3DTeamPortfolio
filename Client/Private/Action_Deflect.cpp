#include "Action_Deflect.h"

#include "GameInstance.h"
#include "BlackBoard.h"

CAction_Deflect::CAction_Deflect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBehavior(pDevice, pContext)
{
}

CAction_Deflect::CAction_Deflect(const CAction_Deflect& rhs)
	: CBehavior(rhs)
{
}

void CAction_Deflect::Set_Options(const wstring& _wstrAnimationTag, CModel* _pModel,
	_bool _isCheckBehavior, const _float& _fCoolTime,
	_bool _isOneTimeAction, _bool _isLerp)
{
	if (nullptr == _pModel)
	{
		MSG_BOX("[CAction_Deflect] _pModel is nullptr");
		return;
	}

	m_pModel = _pModel;
	Safe_AddRef(m_pModel);

	m_wstrAnimationTag = _wstrAnimationTag;
	m_fLimit = _fCoolTime;
	m_isOneTimeAction = _isOneTimeAction;
	m_isCheckBehavior = _isCheckBehavior;

	m_pModel->Get_Animation(m_wstrAnimationTag)->Set_LerpAnim(_isLerp);
}

HRESULT CAction_Deflect::Initialize(void* pArg)
{
	/* �и� ���� */
	Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
		{
			_bool* pIsParring = { nullptr };
			if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
				return false;

			return *pIsParring;
		});

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
				true == m_isEndFirstPlay)
				return false;

			return true;
		});

	return S_OK;
}

HRESULT CAction_Deflect::Tick(const _float& fTimeDelta)
{
	if (false == Check_Decorations())
	{
		m_ReturnData = BEHAVIOR_FAIL;
		return BEHAVIOR_FAIL;
	}

	/* �ൿ�� �ϳ��� ������ �ൿüũ Ȱ��ȭ */
	if (0 < m_Behaviors.size())
		m_isFinishBehaviors = true;
	
	if (true == m_isFirst)
	{
		m_isPlay = true;
		m_pModel->Change_Animation(m_wstrAnimationTag);
		m_isFirst = false;
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
		(true == m_isCheckBehavior && true == m_isFinishBehaviors)) // �ൿüũ�� �Ұǵ� ��� �ൿ�� ��������
	{
		m_isEndFirstPlay = true;
		bCheck = true;	// ����
	}

	if (true == bCheck)
		return BEHAVIOR_SUCCESS;

	return BEHAVIOR_RUNNING;
}

void CAction_Deflect::Reset_Behavior(HRESULT result)
{
	m_isFirst = true;

	_bool* pIsParring = { nullptr };
	if (FAILED(m_pBlackBoard->Get_Type("isParring", pIsParring)))
		return;

	*pIsParring = false;
	
	m_isPlay = false;
	m_isFinishBehaviors = false;

	BEGININSTANCE;
	m_fPreWorldTimeAcc = pGameInstance->Get_World_TimeAcc();
	ENDINSTANCE;
}

CAction_Deflect* CAction_Deflect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction_Deflect* pInstance = new CAction_Deflect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAction_Deflect* CAction_Deflect::Clone(void* pArg)
{
	CAction_Deflect* pInstance = new CAction_Deflect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CAnimStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAction_Deflect::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pModel);
	}
}