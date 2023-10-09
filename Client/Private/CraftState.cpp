#include "..\Public\CraftState.h"
#include "GameInstance.h"
#include "StateContext.h"
#include "Player.h"

CCraftState::CCraftState(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CStateMachine(_pDevice, _pContext)
{

}

CCraftState::CCraftState(const CCraftState& rhs)
	: CStateMachine(rhs)
{
}

HRESULT CCraftState::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CCraftState::Tick(_float fTimeDelta)
{
}

void CCraftState::Late_Tick(_float fTimeDelta)
{
}

void CCraftState::OnStateEnter(void* _pArg)
{
}

void CCraftState::OnStateTick()
{
}

void CCraftState::OnStateExit()
{
}

CCraftState* CCraftState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CComposite* CCraftState::Clone(void* pArg)
{
	return nullptr;
}

void CCraftState::Free()
{
}
