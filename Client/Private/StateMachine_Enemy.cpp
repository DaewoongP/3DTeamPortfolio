#include "StateMachine_Enemy.h"
#include "GameInstance.h"
#include "StateContext_Enemy.h"

CStateMachine_Enemy::CStateMachine_Enemy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CStateMachine_Enemy::CStateMachine_Enemy(const CStateMachine_Enemy& rhs)
	: CComposite(rhs)
{
}

HRESULT CStateMachine_Enemy::Set_StateMachine(const _tchar* _pTag)
{
	if (FAILED(static_cast<CStateContext_Enemy*>(m_pOwner)->Set_StateMachine(_pTag)))
	{
		MSG_BOX("Failed Set StateMachine");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine_Enemy::Set_StateMachineDesc(STATEMACHINEDESC* _pStateMachineDesc)
{
	//비어있는게 있다면?
	if (false == m_StateMachineDesc.IsValid())
	{
		m_StateMachineDesc = *_pStateMachineDesc;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine_Enemy::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed Initialize StateMachine");
		return E_FAIL;
	}

	m_StateMachineDesc = *static_cast<STATEMACHINEDESC*>(pArg);
	Safe_AddRef(m_StateMachineDesc.pOwnerModel);
	Safe_AddRef(m_StateMachineDesc.pTarget);

	return S_OK;
}

void CStateMachine_Enemy::Change_Animation(const wstring& _AnimationTag, _bool _isLumos)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag);
}

void CStateMachine_Enemy::Change_Animation_Part(const wstring& _AnimationTag, _uint iPartIndex)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, (CModel::ANIMTYPE)iPartIndex);
}

CStateMachine_Enemy* CStateMachine_Enemy::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine_Enemy* pInstance = New CStateMachine_Enemy(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStateMachine_Enemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CStateMachine_Enemy::Clone(void* pArg)
{
	CStateMachine_Enemy* pInstance = New CStateMachine_Enemy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStateMachine_Enemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine_Enemy::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		if (true == m_StateMachineDesc.IsValid())
		{
			Safe_Release(m_StateMachineDesc.pOwnerModel);
			Safe_Release(m_StateMachineDesc.pTarget);
		}
	}
}
