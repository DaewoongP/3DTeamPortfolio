#include "Broom_Break.h"
#include "GameInstance.h"
#include "Client_Defines.h"
#include "StateContext.h"
#include "Player.h"

CBroom_Break::CBroom_Break(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice,_pContext)
{

}

CBroom_Break::CBroom_Break(const CBroom_Break& rhs)
	:CStateMachine(rhs)
{

}

HRESULT CBroom_Break::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Break::Initialize(void* pArg)
{
	if (FAILED(CStateMachine::Initialize(pArg)))
	{
		__debugbreak();

		return E_FAIL;
	}

	BEGININSTANCE;

	//pGameInstance->Add_Timer(TEXT("Fix_Angle_Magic_Cast"), false, 0.2f);

	//pGameInstance->Add_Timer(TEXT("Fix_Angle_Magic_Cast_Last"), false, 0.4f);

	//pGameInstance->Add_Timer(TEXT("Go_Idle_Key_Delay"), false, 0.2f);

	m_fFixAngleSpeed = 5.0f;

	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_01"));
	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_02"));
	m_vecSpellActionTextList.push_back(TEXT("Spell_Action_03"));

	ENDINSTANCE;

	return S_OK;
}

void CBroom_Break::Tick(_float fTimeDelta)
{
	OnStateTick();
}

void CBroom_Break::Late_Tick(_float fTimeDelta)
{
}

void CBroom_Break::OnStateEnter(void* _pArg)
{
	
}

void CBroom_Break::OnStateTick()
{
	Go_Idle();
}

void CBroom_Break::OnStateExit()
{

	* m_pisReadySpell = true;


	*m_StateMachineDesc.pisFinishAnimation = true;
}

void CBroom_Break::Go_Idle()
{
	BEGININSTANCE;

	if (m_StateMachineDesc.pOwnerModel->Is_Finish_Animation()
		||
		(true == *m_pisReadySpell && 
			true == *m_StateMachineDesc.pisDirectionPressed && 
			true == pGameInstance->Check_Timer(TEXT("Go_Idle_Key_Delay"))))
	{
		Set_StateMachine(TEXT("Idle"));
	}

	ENDINSTANCE;
}

CBroom_Break* CBroom_Break::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Break* pInstance = New CBroom_Break(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBroom_Break");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComposite* CBroom_Break::Clone(void* pArg)
{
	CBroom_Break* pInstance = New CBroom_Break(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBroom_Break");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBroom_Break::Free()
{
	CStateMachine::Free();

	if (true == m_isCloned)
	{
		m_vecSpellActionTextList.clear();
	}
}
