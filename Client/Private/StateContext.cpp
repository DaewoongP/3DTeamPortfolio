#include "..\Default\StateContext.h"
#include "Client_Defines.h"
#include "StateMachine.h"

CStateContext::CStateContext(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CStateContext::CStateContext(const CStateContext& rhs)
	: CComposite(rhs)
{
}

HRESULT CStateContext::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateContext::Initialize(void* pArg)
{
	NULL_CHECK_RETURN_MSG(pArg, E_FAIL, TEXT("Failed Initialize CStateContext"));

	STATECONTEXTDESC* StateContextDesc = (STATECONTEXTDESC*)pArg;

	m_pOwnerLookAngle = StateContextDesc->pOwnerLookAngle;

	m_pOwnerModel = StateContextDesc->pOwnerModel;

	Safe_AddRef(m_pOwnerModel);

	return S_OK;
}

void CStateContext::Tick(_float fTimeDelta)
{
	m_pCurrentStateMachine->Tick(fTimeDelta);
}

void CStateContext::Late_Tick(_float fTimeDelta)
{
	m_pCurrentStateMachine->Late_Tick(fTimeDelta);
}

HRESULT CStateContext::Set_StateMachine(const _tchar* _pTag)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->OnStateExit();

		Safe_Release(m_pCurrentStateMachine);
	}

	m_pCurrentStateMachine = Find_StateMachine(_pTag);
	
	NULL_CHECK_RETURN_MSG(m_pCurrentStateMachine,E_FAIL,TEXT("Failed Set StateMachine"));
	
	Safe_AddRef(m_pCurrentStateMachine);

	m_pCurrentStateMachine->OnStateEnter();
}

CStateMachine* CStateContext::Find_StateMachine(const _tchar* _pTag)
{
	auto iter = find_if(
		m_pStateMachines.begin(), 
		m_pStateMachines.end(), 
		CTag_Finder(_pTag));

	if (iter == m_pStateMachines.end())
		return nullptr;

	return iter->second;
}

void CStateContext::Add_StateMachine(const _tchar* _pTag, CStateMachine* _pState)
{
	NULL_CHECK_RETURN_MSG(_pState, , TEXT("Failed Add StateMachine"));

	_pState->Set_OwnerModel(m_pOwnerModel);
	_pState->Set_OwnerLookAngle(m_pOwnerLookAngle);

	m_pStateMachines.emplace(_pTag, _pState);
}

CStateContext* CStateContext::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateContext* pInstance = New CStateContext(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStateContext");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CStateContext::Clone(void* pArg)
{
	CStateContext* pInstance = New CStateContext(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStateContext");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateContext::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCurrentStateMachine);
		Safe_Release(m_pOwnerModel);
	}
}
