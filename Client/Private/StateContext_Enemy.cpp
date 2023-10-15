#include "StateContext_Enemy.h"
#include "StateMachine.h"
#include "StateMachine_Enemy.h"

CStateContext_Enemy::CStateContext_Enemy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CStateContext_Enemy::CStateContext_Enemy(const CStateContext_Enemy& rhs)
	: CComposite(rhs)
{
}

HRESULT CStateContext_Enemy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateContext_Enemy::Initialize(void* pArg)
{
	return S_OK;
}

void CStateContext_Enemy::Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->Tick(fTimeDelta);
	}
}

void CStateContext_Enemy::Late_Tick(_float fTimeDelta)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->Late_Tick(fTimeDelta);
	}
}

HRESULT CStateContext_Enemy::Set_StateMachine(const _tchar* _pTag, void * _pArg)
{
	if (nullptr != m_pCurrentStateMachine)
	{
		m_pCurrentStateMachine->OnStateExit();
		lstrcpy(m_wszPreStateKey, (*m_iterCurrentStateMachines).first);
		Safe_Release(m_pCurrentStateMachine);
	}

	m_pCurrentStateMachine = Find_StateMachine(_pTag);

	if (nullptr == m_pCurrentStateMachine)
	{
		MSG_BOX("Failed Set StateMachine_Enemy");
		return E_FAIL;
	}
	
	Safe_AddRef(m_pCurrentStateMachine);
	m_pCurrentStateMachine->OnStateEnter(_pArg);
	return S_OK;
}

_bool CStateContext_Enemy::Is_Current_State(const _tchar* _pTag)
{
	auto iter = find_if(
		m_pStateMachines.begin(),
		m_pStateMachines.end(),
		CTag_Finder(_pTag));

	//없다면 빼고
	if (iter == m_pStateMachines.end())
		return false;

	//있다면 현재 스테이트와 같은지
	if (m_pCurrentStateMachine == iter->second)
	{
		return true;
	}

	return false;
}

CStateMachine_Enemy* CStateContext_Enemy::Find_StateMachine(const _tchar* _pTag)
{
	auto iter = find_if(
		m_pStateMachines.begin(), 
		m_pStateMachines.end(), 
		CTag_Finder(_pTag));

	if (iter == m_pStateMachines.end())
		return nullptr;

	m_iterCurrentStateMachines = iter;

	return iter->second;
}

//래밸, 컴포넌트태그, 스테이트키, 프로토태그, 보이드 포인터
HRESULT CStateContext_Enemy::Add_StateMachine(LEVELID _eLevelID, const _tchar* _ComponentTag, const _tchar* _StateTag, const _tchar* _ProtoTag, void* _pArg)
{
	if (nullptr == _pArg)
	{
		MSG_BOX("Failed Add StateMachine_Enemy");
		return E_FAIL;
	}

	CStateMachine_Enemy* pStateMachine = { nullptr };

	if (FAILED(CComposite::Add_Component
	(_eLevelID,_ProtoTag,_ComponentTag, reinterpret_cast<CComponent**>(&pStateMachine),_pArg)))
	{
		MSG_BOX("Failed Add StateMachine_Enemy");
		return E_FAIL;
	}

	pStateMachine->Set_Owner(this);
	pStateMachine->Bind_Notify();
	m_pStateMachines.emplace(_StateTag, pStateMachine);

	return S_OK;
}


CStateContext_Enemy* CStateContext_Enemy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateContext_Enemy* pInstance = New CStateContext_Enemy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStateContext_Enemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CStateContext_Enemy::Clone(void* pArg)
{
	CStateContext_Enemy* pInstance = New CStateContext_Enemy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStateContext_Enemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateContext_Enemy::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pCurrentStateMachine);

		for (auto &iter : m_pStateMachines)
		{
			Safe_Release(iter.second);
		}
		m_pStateMachines.clear();
	}
}
