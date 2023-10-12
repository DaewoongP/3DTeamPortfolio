#include "UseItemState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CUseItemState::CUseItemState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CUseItemState::CUseItemState(const CUseItemState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CUseItemState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUseItemState::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

void CUseItemState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CUseItemState::Late_Tick(_float fTimeDelta)
{
}

void CUseItemState::OnStateEnter(void* _pArg)
{
	USEITEMDESC* pUseItemDesc = static_cast<USEITEMDESC*>(_pArg);

	pUseItemDesc->eItem_Id;

	switch (pUseItemDesc->eItem_Id)
	{
	case Client::ITEM_ID_WIGGENWELD_POTION:
	case Client::ITEM_ID_EDURUS_POTION:
	case Client::ITEM_ID_FOCUS_POTION:
	case Client::ITEM_ID_MAXIMA_POTION:
	case Client::ITEM_ID_INVISIBILITY_POTION:
	case Client::ITEM_ID_THUNDERBEW_POTION:
	case Client::ITEM_ID_FELIX_FELICIS_POTION:
	{
		m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("Drink_Potion"), pUseItemDesc->funcPotion);

		Change_Animation(TEXT("Drink_Potion_Throw"));
	}
	break;
	case Client::ITEM_ID_CHINESE_CHOMPING_CABBAGE:
	{
		m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Throw_Plant"), TEXT("Throw_Plant"), pUseItemDesc->funcPotion);

		Change_Animation(TEXT("Epuip_Plant"));
	}
	break;
	default:
		break;
	}


	*m_StateMachineDesc.pisFinishAnimation = false;
}

void CUseItemState::OnStateTick()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		//식물
		if (false == lstrcmp(TEXT("Epuip_Plant"), m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName()))
		{
			Change_Animation(TEXT("Throw_Plant"));
		}
		//맨드레이크 시퀀스
		else if (false == lstrcmp(TEXT("Mandrake_Enter"), m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName()))
		{
			Change_Animation(TEXT("Mandrake_Idle"));
		}
		else if (false == lstrcmp(TEXT("Mandrake_Idle"), m_StateMachineDesc.pOwnerModel->Get_Animation()->Get_AnimationName()))
		{
			Change_Animation(TEXT("Mandrake_Exit"));
		}
		else
		{
			Go_Idle();
		}

		*m_StateMachineDesc.pisFinishAnimation = false;
	}
}

void CUseItemState::OnStateExit()
{
	* m_StateMachineDesc.pisFinishAnimation = false;
}

void CUseItemState::Bind_Notify()
{
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Epuip_Plant"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Throw_Plant"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Mandrake_Enter"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Mandrake_Idle"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
	m_StateMachineDesc.pOwnerModel->Bind_Notify(TEXT("Mandrake_Exit"), TEXT("End_Animation"), m_StateMachineDesc.pfuncFinishAnimation);
}

void CUseItemState::Go_Idle()
{
	if (true == *m_StateMachineDesc.pisFinishAnimation)
	{
		*m_StateMachineDesc.pisFinishAnimation = false;
		Set_StateMachine(TEXT("Idle"));
	}
}

CUseItemState* CUseItemState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUseItemState* pInstance = New CUseItemState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CUseItemState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CUseItemState::Clone(void* pArg)
{
	CUseItemState* pInstance = New CUseItemState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CUseItemState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUseItemState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
