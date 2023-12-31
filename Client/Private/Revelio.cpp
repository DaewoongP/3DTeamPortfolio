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

HRESULT CRevelio::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
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
}

void CRevelio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CRevelio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CRevelio::Add_Components()
{
	return S_OK;
}

CRevelio* CRevelio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CRevelio* pInstance = New CRevelio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
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
