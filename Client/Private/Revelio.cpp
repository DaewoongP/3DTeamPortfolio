#include "Revelio.h"
#include "GameInstance.h"

CRevelio::CRevelio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CRevelio::CRevelio(const CRevelio& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CRevelio::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRevelio::Initialize(void* pArg)
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

void CRevelio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CRevelio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CRevelio::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionEnter(CollisionDesc);
	cout << "Player Enter" << endl;
}

void CRevelio::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionStay(CollisionDesc);
	cout << "stay" << endl;
}

void CRevelio::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionExit(CollisionDesc);
	cout << "Exit" << endl;
}

HRESULT CRevelio::Add_Components()
{
	return S_OK;
}

HRESULT CRevelio::Add_Effect()
{
	return S_OK;
}

CRevelio* CRevelio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRevelio* pInstance = new CRevelio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRevelio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRevelio::Clone(void* pArg)
{
	CRevelio* pInstance = new CRevelio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRevelio");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRevelio::Free()
{
	__super::Free();
}
