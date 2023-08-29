#include "Decorator.h"

HRESULT CDecorator::Bind_Decoration(function<_bool(class CBlackBoard*)> Func)
{
	if (nullptr == Func)
	{
		MSG_BOX("[CDecorator] function is nullptr");
		return E_FAIL;
	}

	m_Function = Func;

	return S_OK;
}

_bool CDecorator::Is_Execute(CBlackBoard* pBlackBoard)
{
	if (nullptr == m_Function)
	{
		MSG_BOX("[CDecorator] m_Function is nullptr");
		return false;
	}

	return m_Function(pBlackBoard);
}

CDecorator* CDecorator::Create(function<_bool(class CBlackBoard*)> Func)
{
	CDecorator* pInstance = new CDecorator();

	if (FAILED(pInstance->Initialize(Func)))
	{
		MSG_BOX("Failed To Create CDecorator");
		Safe_Release(pInstance);
	}

	return pInstance;
}