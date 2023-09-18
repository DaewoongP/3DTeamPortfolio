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

void CRevelio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//cout << "Player Enter" << endl;
}

void CRevelio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
	//cout << "stay" << endl;
}

void CRevelio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
	//cout << "Exit" << endl;
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
	CRevelio* pInstance = New CRevelio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRevelio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRevelio::Clone(void* pArg)
{
	CRevelio* pInstance = New CRevelio(*this);

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
