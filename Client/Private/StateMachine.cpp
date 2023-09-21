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

HRESULT CStateMachine::Set_StateMachine(const _tchar* _pTag)
{
	if (dynamic_cast<CStateContext*>(m_pOwner)->Set_StateMachine(_pTag))
	{
		MSG_BOX("Failed Set StateMachine");

		return E_FAIL;
	}

	return S_OK;
}

void CStateMachine::Set_OwnerModel(CModel* _pOwnerModel)
{
	NULL_CHECK_RETURN_MSG(_pOwnerModel, , TEXT("Failed Set StateMachine Owner Model"));

	m_pOwnerModel = _pOwnerModel;

	Safe_AddRef(m_pOwnerModel);
}

void CStateMachine::Set_OwnerLookAngle(_float* _pOwnerLookAngle)
{
	NULL_CHECK_RETURN_MSG(_pOwnerLookAngle, , TEXT("Failed Set StateMachine Owner LookAngle"));

	m_pOwnerLookAngle = _pOwnerLookAngle;
}

void CStateMachine::Set_IsDirectionKeyPressed(_bool* _pIsDirectionKeyPressed)
{
	NULL_CHECK_RETURN_MSG(_pIsDirectionKeyPressed, , TEXT("Failed Set StateMachine Owner IsDirectionKeyPressed"));

	m_pIsDirectionKeyPressed = _pIsDirectionKeyPressed;
}

void CStateMachine::Set_PlayerTransform(CTransform* _pPlayerTransform)
{
	NULL_CHECK_RETURN_MSG(_pPlayerTransform, , TEXT("Failed Set StateMachine Owner PlayerTransform"));

	m_pPlayerTransform = _pPlayerTransform;

	Safe_AddRef(m_pPlayerTransform);
}


void CStateMachine::Go_Protego()
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN))
	{
		Set_StateMachine(TEXT("Protego"));
	}

	ENDINSTANCE;
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
		Safe_Release(m_pOwnerModel);
		Safe_Release(m_pPlayerTransform);

	}
}
