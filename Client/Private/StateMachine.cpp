#include "StateMachine.h"
#include "Client_Defines.h"

CStateMachine::CStateMachine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComposite(_pDevice, _pContext)
{
}

CStateMachine::CStateMachine(const CStateMachine& rhs)
	: CComposite(rhs)
{
}

HRESULT CStateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateMachine::Initialize(void* pArg)
{
	return S_OK;
}

void CStateMachine::Tick(_float fTimeDelta)
{
}

void CStateMachine::Late_Tick(_float fTimeDelta)
{
}

void CStateMachine::Set_OwnerModel(CModel* _pOwnerModel)
{
	NULL_CHECK_RETURN_MSG(_pOwnerModel, , TEXT("Failed Set StateMachine Owner Model"));

	m_pOwnerModel = _pOwnerModel;

	Safe_AddRef(m_pOwnerModel);
}

void CStateMachine::Set_OwnerLookAngle(_float* _pOwnerLookAngle)
{
	NULL_CHECK_RETURN_MSG(m_pOwnerLookAngle, , TEXT("Failed Set StateMachine Owner LookAngle"));

	m_pOwnerLookAngle = _pOwnerLookAngle;
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
	if (true == m_isCloned)
	{
		Safe_Release(m_pOwnerModel);
	}
}
