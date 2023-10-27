#include "..\Public\Balloon_Coin.h"

CBalloon_Coin::CBalloon_Coin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon_Coin::CBalloon_Coin(const CBalloon_Coin& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon_Coin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBalloon_Coin::Tick(_float fTimeDelta)
{
}

void CBalloon_Coin::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBalloon_Coin::Render()
{
	return E_NOTIMPL;
}

HRESULT CBalloon_Coin::Add_Component()
{
	return E_NOTIMPL;
}

CBalloon_Coin* CBalloon_Coin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CBalloon_Coin::Clone(void* pArg)
{
	return nullptr;
}

void CBalloon_Coin::Free()
{
}
