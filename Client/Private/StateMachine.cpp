#include "StateMachine.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CStateMachine::CStateMachine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
	: CComposite(rhs)
{
}

HRESULT CStateMachine::Set_StateMachine(const _tchar* _pTag)
{
	if (dynamic_cast<CStateContext*>(m_pOwner)->Set_StateMachine(_pTag))
	{
		MSG_BOX("Failed Set StateMachine");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine::Set_StateMachineDesc(STATEMACHINEDESC* _pStateMachineDesc)
{
	//����ִ°� �ִٸ�?
	if (false == m_StateMachineDesc.IsValid())
	{
		m_StateMachineDesc = *_pStateMachineDesc;

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed Initialize StateMachine");

		return E_FAIL;
	}

	//ä�����ֳ�?
	if (true == m_StateMachineDesc.IsValid())
	{
		//ä���������� �ȵǴµ�, �̻��ϴϱ� �޼����� ������
		if (nullptr != m_StateMachineDesc.pOwnerModel)
		{
			//�� ģ���� ������ �����ְ�
			MSG_BOX("this value is be a valid value : pOwnerModel");
			Safe_Release(m_StateMachineDesc.pOwnerModel);
		}
		if (nullptr != m_StateMachineDesc.pPlayerTransform)
		{
			//�� ģ���� ������ �����ְ�
			MSG_BOX("this value is be a valid value : pPlayerTransform");
			Safe_Release(m_StateMachineDesc.pPlayerTransform);
		}
	}

	STATEMACHINEDESC pSTATEMACHINEDESC = *static_cast<STATEMACHINEDESC*>(pArg);

	m_StateMachineDesc = *static_cast<STATEMACHINEDESC*>(pArg);

	

	Safe_AddRef(m_StateMachineDesc.pOwnerModel);
	Safe_AddRef(m_StateMachineDesc.pPlayerTransform);

	return S_OK;
}

void CStateMachine::OnStateEnter(void* _pArg)
{
	
}

CStateMachine* CStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStateMachine* pInstance = New CStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CStateMachine::Clone(void* pArg)
{
	CStateMachine* pInstance = New CStateMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CStateMachine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStateMachine::Free()
{
	CComposite::Free();

	if (true == m_isCloned)
	{
		if (true == m_StateMachineDesc.IsValid())
		{
			Safe_Release(m_StateMachineDesc.pOwnerModel);
			Safe_Release(m_StateMachineDesc.pPlayerTransform);
		}
	}
}
