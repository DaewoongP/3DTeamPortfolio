#include "StateContext.h"
#include "Client_Defines.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "IdleState.h"

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

	m_pIsDirectionPressed = StateContextDesc->pIsDirectionPressed;

	m_pPlayerTransform = StateContextDesc->pPlayerTransform;

	Safe_AddRef(m_pPlayerTransform);

	if (FAILED(Ready_StateMachine()))
	{
		MSG_BOX(TEXT("Failed Ready StateMachine"));

		return E_FAIL;
	}

	return S_OK;
}

void CStateContext::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->Tick(fTimeDelta);
	}
}

void CStateContext::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->Late_Tick(fTimeDelta);
	}
}

HRESULT CStateContext::Set_StateMachine(const _tchar* _pTag)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->OnStateExit();

		Safe_Release(m_pCurrentStateMachine);
	}

	m_pCurrentStateMachine = Find_StateMachine(_pTag);
	

	if (nullptr == m_pCurrentStateMachine)
	{
		MSG_BOX("Failed Set StateMachine");

		return E_FAIL;
	}
	
	Safe_AddRef(m_pCurrentStateMachine);

	m_pCurrentStateMachine->OnStateEnter();

	return S_OK;
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

HRESULT CStateContext::Add_StateMachine(const _tchar* _pTag, CStateMachine* _pState)
{
	NULL_CHECK_RETURN_MSG(_pState, E_FAIL , TEXT("Failed Add StateMachine"));
	
	_pState->Set_Owner(this);
	_pState->Set_OwnerModel(m_pOwnerModel);
	_pState->Set_OwnerLookAngle(m_pOwnerLookAngle);
	_pState->Set_IsDirectionKeyPressed(m_pIsDirectionPressed);
	_pState->Set_PlayerTransform(m_pPlayerTransform);

	m_pStateMachines.emplace(_pTag, _pState);

	return S_OK;
}

HRESULT CStateContext::Ready_StateMachine()
{
	BEGININSTANCE;

	if (FAILED(Add_StateMachine(TEXT("Idle"),
		static_cast<CStateMachine*>
		(pGameInstance->Clone_Component(LEVEL_MAINGAME,
			TEXT("Prototype_Component_State_Idle"))))))
	{
		ENDINSTANCE;

		MSG_BOX("Failed Ready_StateMachine");

		return E_FAIL;
	};

	if (FAILED(Add_StateMachine(TEXT("Move Turn"),
		static_cast<CStateMachine*>
		(pGameInstance->Clone_Component(LEVEL_MAINGAME,
			TEXT("Prototype_Component_State_Move_Turn"))))))
	{
		ENDINSTANCE;

		MSG_BOX("Failed Ready_StateMachine");

		return E_FAIL;
	};

	if (FAILED(Add_StateMachine(TEXT("Move Start"),
		static_cast<CStateMachine*>
		(pGameInstance->Clone_Component(LEVEL_MAINGAME,
			TEXT("Prototype_Component_State_Move_Start"))))))
	{
		ENDINSTANCE;

		MSG_BOX("Failed Ready_StateMachine");

		return E_FAIL;
	};

	if (FAILED(Add_StateMachine(TEXT("Move Loop"),
		static_cast<CStateMachine*>
		(pGameInstance->Clone_Component(LEVEL_MAINGAME,
			TEXT("Prototype_Component_State_Move_Loop"))))))
	{
		ENDINSTANCE;

		MSG_BOX("Failed Ready_StateMachine");

		return E_FAIL;
	};

	Set_StateMachine(TEXT("Idle"));

	ENDINSTANCE;

	return S_OK;
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
		Safe_Release(m_pPlayerTransform);

		for (auto &iter : m_pStateMachines)
		{
			Safe_Release(iter.second);
		}
		m_pStateMachines.clear();
	}
}
