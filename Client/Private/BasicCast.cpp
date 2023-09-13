#include "BasicCast.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CBasicCast::CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CBasicCast::CBasicCast(const CBasicCast& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CBasicCast::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBasicCast::Initialize(void* pArg)
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

	if (FAILED(Add_Effect()))
	{
		MSG_BOX("Failed Player Add_Effect");

		return E_FAIL;
	}

	//m_vTargetPosition = m_pTarget->Get_Position();
	m_vTargetPosition = _float3(0, 20, 0);
	return S_OK;
}

void CBasicCast::Tick(_float fTimeDelta)
{
	m_fLerpAcc += fTimeDelta;
	BEGININSTANCE
	_float3 movedPos = pGameInstance->GetVectorSlerp(m_vStartPosition, m_vTargetPosition, _float3(0, 1, 0), 0.5f, m_fLerpAcc);
	ENDINSTANCE
	m_pTransform->Set_Position(movedPos);
	m_pMeshEffectTrans->Set_Position(m_pTransform->Get_Position());
	__super::Tick(fTimeDelta);
}

void CBasicCast::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBasicCast::OnCollisionEnter(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionEnter(CollisionDesc);
	cout << "Player Enter" << endl;
}

void CBasicCast::OnCollisionStay(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionStay(CollisionDesc);
	cout << "stay" << endl;
}

void CBasicCast::OnCollisionExit(COLLISIONDESC CollisionDesc)
{
	__super::OnCollisionExit(CollisionDesc);
	cout << "Exit" << endl;
}

HRESULT CBasicCast::Add_Components()
{
	return S_OK;
}

HRESULT CBasicCast::Add_Effect()
{
	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MeshEffect"),
		TEXT("Com_MeshEffect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed CBasicCast Add_Component : (Com_MeshEffect)");
		return E_FAIL;
	}

	m_pMeshEffectTrans = m_pMeshEffect->Get_Transform();
	Safe_AddRef(m_pMeshEffectTrans);
	return S_OK;
}

CBasicCast* CBasicCast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBasicCast* pInstance = new CBasicCast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CBasicCast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBasicCast::Clone(void* pArg)
{
	CBasicCast* pInstance = new CBasicCast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBasicCast");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBasicCast::Free()
{
	__super::Free();
	Safe_Release(m_pMeshEffectTrans);
	Safe_Release(m_pMeshEffect);
}
