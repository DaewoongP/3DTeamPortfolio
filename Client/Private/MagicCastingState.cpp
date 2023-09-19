#include "MagicCastingState.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"

CMagicCastingState::CMagicCastingState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CMagicCastingState::CMagicCastingState(const CMagicCastingState& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CMagicCastingState::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagicCastingState::Initialize(void* pArg)
{
	return S_OK;
}

void CMagicCastingState::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CMagicCastingState::Late_Tick(_float fTimeDelta)
{
}

void CMagicCastingState::OnStateEnter()
{
	switch (*m_pIActionSwitch)
	{
	case CStateContext::ACTION_CASUAL:
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"));
	}
	break;
	case CStateContext::ACTION_CMBT:
	{
		m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"));
	}
	break;
	default:
		break;
	}

	++m_iBasicSpellCombo;

	m_isReadySpell = false;

	m_iBasicSpellCombo = BASICSPELL_START;

#ifdef _DEBUG
	cout << "Hard Land Enter" << endl;
#endif // _DEBUG
}

void CMagicCastingState::OnStateTick()
{
	Go_Idle();

	BasicSpell_Tick();

	Initialize_BasicSpell_Combo();
}

void CMagicCastingState::OnStateExit()
{
#ifdef _DEBUG
	cout << "Hard Land Exit" << endl;
#endif // _DEBUG
}

void CMagicCastingState::Bind_Notify()
{
	/*void (Client::CMagicCastingState::*Notify_Pointer)();

	Notify_Pointer = &BasicSpell_Ready;*/

	function<void()> Notify_Pointer = [&] {(*this).BasicSpell_Ready(); };

	//캐주얼 평타
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	//전투 평타
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	m_pOwnerModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"), TEXT("BasicSpell_Ready"), Notify_Pointer);
	
	
}

void CMagicCastingState::Initialize_BasicSpell_Combo()
{
	if (BASICSPELL_END <= m_iBasicSpellCombo)
	{
		m_iBasicSpellCombo = BASICSPELL_START;
	}
}

void CMagicCastingState::BasicSpell_Ready()
{
	//인덱스 증가 
	++m_iBasicSpellCombo;
	//준비 변수 초기화
	m_isReadySpell = true;
}

void CMagicCastingState::BasicSpell_Tick()
{
	BEGININSTANCE;
	switch (m_iBasicSpellCombo)
	{
	case Client::CMagicCastingState::BASICSPELL_START:
	{
		if (true == m_isReadySpell && 
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"));
			}
			break;
			default:
				break;
			}
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_SECOND:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"));
			}
			break;
			default:
				break;
			}
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_BACKSTEP:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"));
			}
			break;
			default:
				break;
			}
			m_isReadySpell = false;
		}
	}
		break;
	case Client::CMagicCastingState::BASICSPELL_LAST:
	{
		if (true == m_isReadySpell &&
			pGameInstance->Get_DIMouseState(
				CInput_Device::DIMK_LBUTTON,
				CInput_Device::KEY_DOWN))
		{
			switch (*m_pIActionSwitch)
			{
			case CStateContext::ACTION_CASUAL:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"));
			}
			break;
			case CStateContext::ACTION_CMBT:
			{
				m_pOwnerModel->Change_Animation(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"));
			}
			break;
			default:
				break;
			}
			m_isReadySpell = false;
		}
	}
		break;
	default:
		break;
	}
	ENDINSTANCE;
}

void CMagicCastingState::Action_Casual_Tick()
{
}

void CMagicCastingState::Action_Cmbt_Tick()
{
}

void CMagicCastingState::Go_Idle()
{
	if (m_pOwnerModel->Is_Finish_Animation())
	{
		Set_StateMachine(TEXT("Idle"));
	}
}

CMagicCastingState* CMagicCastingState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagicCastingState* pInstance = New CMagicCastingState(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CMagicCastingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CMagicCastingState::Clone(void* pArg)
{
	CMagicCastingState* pInstance = New CMagicCastingState(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CMagicCastingState");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagicCastingState::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{

	}
}
