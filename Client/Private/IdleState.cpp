#include "..\Public\IdleState.h"
#include "Client_Defines.h"

CIdleState::CIdleState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CIdleState::CIdleState(const CIdleState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CIdleState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIdleState::Initialize(void* pArg)
{
	return S_OK;
}

void CIdleState::Tick(_float fTimeDelta)
{
}

void CIdleState::Late_Tick(_float fTimeDelta)
{
}

void CIdleState::OnStateEnter()
{
}

void CIdleState::OnStateTick()
{
}

void CIdleState::OnStateExit()
{
}

CIdleState* CIdleState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIdleState* pInstance = New CIdleState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CIdleState::Clone(void* pArg)
{
	CIdleState* pInstance = New CIdleState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CIdleState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIdleState::Free()
{
	if (true == m_isCloned)
	{

	}
}
