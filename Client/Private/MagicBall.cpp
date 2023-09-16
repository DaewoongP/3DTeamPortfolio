#include "MagicBall.h"
#include "GameInstance.h"
#include "Weapon_Player_Wand.h"

CMagicBall::CMagicBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMagicBall::CMagicBall(const CMagicBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMagicBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMagicBall::Initialize(void* pArg)
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

	MAGICBALLINITDESC* initDesc = static_cast<MAGICBALLINITDESC*>(pArg);

	m_eMagicGroup = initDesc->eMagicGroup;
	m_eMagicType = initDesc->eMagicType;
	m_eBuffType = initDesc->eBuffType;
	m_eMagicTag = initDesc->eMagicTag;
	m_fDamage = initDesc->fDamage;
	m_fDistance = initDesc->fDistance;
	m_fInitLiftTime = initDesc->fLiftTime;
	m_pWeapon = initDesc->pWeapon;
	Safe_AddRef(m_pWeapon);
	m_pTarget = initDesc->pTarget;
	Safe_AddRef(m_pTarget);

	m_fLiftTime = m_fInitLiftTime;
	m_pTransform->Set_Position(m_vStartPosition);

	m_vStartPosition = m_pWeapon->Get_Transform()->Get_Position() + m_pWeapon->Get_Wand_Point_Offset();
	
	m_CollisionDesc.eMagicGroup = m_eMagicGroup;
	m_CollisionDesc.eMagicType = m_eMagicType;
	m_CollisionDesc.eBuffType = m_eBuffType;
	m_CollisionDesc.eMagicTag = m_eMagicTag;
	m_CollisionDesc.fDamage = m_fDamage;

	Set_CollisionData(&m_CollisionDesc);
	return S_OK;
}

void CMagicBall::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	//시간이 흐름.
	if (m_fLiftTime > 0)
		m_fLiftTime -= fTimeDelta;	
}

void CMagicBall::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CMagicBall::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	cout << "Player Enter" << endl;
}

void CMagicBall::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	cout << "stay" << endl;
}

void CMagicBall::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	cout << "Exit" << endl;
}

HRESULT CMagicBall::Add_Components()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(1.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.vInitPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.pOwnerObject = this;

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}
	// 리지드바디 액터 설정
	PxRigidBody* Rigid = m_pRigidBody->Get_RigidBodyActor();
	Rigid->setMaxLinearVelocity(1000.f);
	Rigid->setMass(10.f);
	Rigid->setAngularDamping(0.7f);

	return S_OK;
}

void CMagicBall::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pTarget);
		Safe_Release(m_pWeapon);
	}
}
