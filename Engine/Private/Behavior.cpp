#include "Behavior.h"

#include "Decorator.h"
#include "BlackBoard.h"

CBehavior::CBehavior(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
	, m_iterCurBehavior(m_Behaviors.begin())
{
}

CBehavior::CBehavior(const CBehavior& rhs)
	: CComponent(rhs)
	, m_iterCurBehavior(m_Behaviors.begin())
{
}

HRESULT CBehavior::Add_Decoration(function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func)
		return E_FAIL;

	CDecorator* pDecoration = CDecorator::Create(Func);
	if (nullptr == pDecoration)
		return E_FAIL;

	m_Decorations.push_back(pDecoration);

	return S_OK;
}

HRESULT CBehavior::Assemble_Behavior(const wstring& _BehaviorTag, CBehavior* _pBehavior)
{
	/* 태그 중복값 검사 */
	if (nullptr != Find_Behavior(_BehaviorTag))
	{
		MSG_BOX("[CBehavior] Tag is duplicate Tag");
		return E_FAIL;
	}

	if (nullptr == m_pBlackBoard)
	{
		MSG_BOX("[CBehavior] BlackBoard is nullptr");
		return E_FAIL;
	}

	_pBehavior->m_wstrBehaviorTag = _BehaviorTag;
	_pBehavior->m_pParentBehavior = this; /* 상호참조 때문에 AddRef는 하지 않는다. */
	_pBehavior->m_pOwner = m_pOwner; /* 상호참조 때문에 AddRef는 하지 않는다. */
	_pBehavior->m_pBlackBoard = m_pBlackBoard;
	Safe_AddRef(m_pBlackBoard);

	m_Behaviors.push_back(_pBehavior);

	m_iterCurBehavior = m_Behaviors.begin();

	if (FAILED(_pBehavior->Assemble_Childs()))
	{
		wstring wstrErrorTag = TEXT("[") + m_wstrBehaviorTag + TEXT("] Failed Assemble_Behavior ") + _BehaviorTag;
		MSG_BOX(wstrErrorTag.c_str());
		return E_FAIL;
	}

	return S_OK;
}

_bool CBehavior::Check_Decorations()
{
	for (auto& Deco : m_Decorations)
	{
		if (false == Deco->Is_Execute(m_pBlackBoard))
			return false;
	}

	return true;
}

CBehavior* CBehavior::Find_Behavior(const wstring& BehaviorTag)
{
	const auto& iter = find_if(m_Behaviors.begin(), m_Behaviors.end(), [&](auto pBehavior)
		{
			if (BehaviorTag == pBehavior->m_wstrBehaviorTag)
				return true;
			else
				return false;
		});

	if (iter == m_Behaviors.end())
		return nullptr;

	return *iter;
}

void CBehavior::Free()
{
	for (auto& pBehavior : m_Behaviors)
		Safe_Release(pBehavior);

	for (auto& Func : m_Decorations)
		Safe_Release(Func);

	if (true == m_isCloned)
		Safe_Release(m_pBlackBoard);

	__super::Free();
}

#ifdef _DEBUG

void CBehavior::Find_Running_Behavior(_Inout_ vector<wstring>& BehaviorTags)
{
	if (0 == m_Behaviors.size())
		return;

	for (auto& pBehavior : m_Behaviors)
	{
		if (BEHAVIOR_RUNNING == pBehavior->m_ReturnData)
		{
			BehaviorTags.push_back(pBehavior->m_wstrBehaviorTag);
			pBehavior->Find_Running_Behavior(BehaviorTags);
		}
	}
}

#endif // _DEBUG
