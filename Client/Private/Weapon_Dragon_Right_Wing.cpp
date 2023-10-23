#include "Weapon_Dragon_Right_Wing.h"
#include "GameInstance.h"

CWeapon_Dragon_Right_Wing::CWeapon_Dragon_Right_Wing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Dragon_Right_Wing::CWeapon_Dragon_Right_Wing(const CWeapon_Dragon_Right_Wing& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_Dragon_Right_Wing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_CollisionRequestDesc.eType = CEnemy::ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 10;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	return S_OK;
}

#ifdef _DEBUG
void CWeapon_Dragon_Right_Wing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
}
#endif // _DEBUG

HRESULT CWeapon_Dragon_Right_Wing::Add_Components()
{
	/* For.Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(2.f, 3.5f);
	RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::None;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 1.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Attack");
	RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_SHIELD;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("[CWeapon_Dragon_Right_Wing] Failed Initialize : Com_RigidBody");
		return E_FAIL;
	}

#ifdef _DEBUG
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
		throw TEXT("Com_Renderer");
#endif // _DEBUG

	return S_OK;
}

CWeapon_Dragon_Right_Wing* CWeapon_Dragon_Right_Wing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Dragon_Right_Wing* pInstance = New CWeapon_Dragon_Right_Wing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Dragon_Right_Wing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Dragon_Right_Wing* CWeapon_Dragon_Right_Wing::Clone(void* pArg)
{
	CWeapon_Dragon_Right_Wing* pInstance = New CWeapon_Dragon_Right_Wing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Dragon_Right_Wing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Dragon_Right_Wing::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG
}
