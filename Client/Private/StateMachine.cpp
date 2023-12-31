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
	if (static_cast<CStateContext*>(m_pOwner)->Set_StateMachine(_pTag))
	{
		MSG_BOX("Failed Set StateMachine");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStateMachine::Set_StateMachineDesc(STATEMACHINEDESC* _pStateMachineDesc)
{
	//비어있는게 있다면?
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

	//채워져있냐?
	if (true == m_StateMachineDesc.IsValid())
	{
		//채워져있으면 안되는데, 이상하니까 메세지만 띄워줘봐
		if (nullptr != m_StateMachineDesc.pOwnerModel)
		{
			//이 친구들 있으면 지워주고
			MSG_BOX("this value is be a valid value : pOwnerModel");
			Safe_Release(m_StateMachineDesc.pOwnerModel);
		}
		if (nullptr != m_StateMachineDesc.pPlayerTransform)
		{
			//이 친구들 있으면 지워주고
			MSG_BOX("this value is be a valid value : pPlayerTransform");
			Safe_Release(m_StateMachineDesc.pPlayerTransform);
		}
	}

	STATEMACHINEDESC pSTATEMACHINEDESC = *static_cast<STATEMACHINEDESC*>(pArg);

	m_StateMachineDesc = *static_cast<STATEMACHINEDESC*>(pArg);

	

	Safe_AddRef(m_StateMachineDesc.pOwnerModel);
	Safe_AddRef(m_StateMachineDesc.pPlayerTransform);
	Safe_AddRef(m_StateMachineDesc.pBroom);

	return S_OK;
}

void CStateMachine::OnStateEnter(void* _pArg)
{
	
}

void CStateMachine::Change_Animation(const wstring& _AnimationTag, _bool _isLumos)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag);

	if (true == _isLumos)
	{
		//루모스가 꺼져 있을 경우만 애니메이션을 바꾼다.
		if (false == *m_StateMachineDesc.pLumosOn)
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::OTHERBODY);
		}
	}
	else
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::OTHERBODY);
	}
}

void CStateMachine::Change_Animation_FlyAttack(const wstring& _AnimationTag)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag);

	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation(CModel::OTHERBODY))
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::OTHERBODY);
	}
}

void CStateMachine::Change_Animation_Part(const wstring& _AnimationTag, _uint iPartIndex)
{
	m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, (CModel::ANIMTYPE)iPartIndex);
}

void CStateMachine::Change_Animation_FlyOrNot(const wstring& _AnimationTag, _bool _isLumos)
{
	if (*m_StateMachineDesc.pIsFlying)
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::ANOTHERBODY);
	}
	else 
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag);
	}

	if (true == _isLumos)
	{
		//루모스가 꺼져 있을 경우만 애니메이션을 바꾼다.
		if (false == *m_StateMachineDesc.pLumosOn)
		{
			m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::OTHERBODY);
		}
	}
	else
	{
		m_StateMachineDesc.pOwnerModel->Change_Animation(_AnimationTag, CModel::OTHERBODY);
	}
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
			Safe_Release(m_StateMachineDesc.pBroom);
		}
	}
}
