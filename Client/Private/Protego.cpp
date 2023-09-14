#include "Protego.h"
#include "GameInstance.h"

CProtego::CProtego(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CProtego::CProtego(const CProtego& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CProtego::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProtego::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}
	return S_OK;
}

void CProtego::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CProtego::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CProtego::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionEnter(CollisionDesc);
	cout << "Player Enter" << endl;
}

void CProtego::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionStay(CollisionDesc);
	cout << "stay" << endl;
}

void CProtego::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionExit(CollisionDesc);
	cout << "Exit" << endl;
}

HRESULT CProtego::Add_Components()
{
	return S_OK;
}

HRESULT CProtego::Add_Effect()
{
	return S_OK;
}

CProtego* CProtego::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProtego* pInstance = new CProtego(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProtego");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProtego::Clone(void* pArg)
{
	CProtego* pInstance = new CProtego(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProtego");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CProtego::Free()
{
	__super::Free();
}