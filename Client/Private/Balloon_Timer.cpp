#include "..\Public\Balloon_Timer.h"
#include "GameInstance.h"

CBalloon_Timer::CBalloon_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon_Timer::CBalloon_Timer(const CBalloon_Timer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon_Timer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CBalloon_Timer::Tick(_float fTimeDelta)
{
}

void CBalloon_Timer::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBalloon_Timer::Render()
{
	return S_OK;
}

HRESULT CBalloon_Timer::Add_Components()
{
	return S_OK;
}

HRESULT CBalloon_Timer::SetUp_ShaderResources()
{
	return S_OK;
}

CBalloon_Timer* CBalloon_Timer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CBalloon_Timer::Clone(void* pArg)
{
	return nullptr;
}

void CBalloon_Timer::Free()
{
}
