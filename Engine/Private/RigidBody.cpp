#include "..\Public\RigidBody.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"

CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComponent(rhs)
{
}

PxRigidBody* CRigidBody::Get_RigidBodyActor() const
{
	return reinterpret_cast<PxRigidBody*>(m_pActor);
}

_float3 CRigidBody::Get_Position() const
{
	PxTransform vPose;
	if (false == m_isKinematic ||
		false == reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(vPose))
	{
		vPose = m_pActor->getGlobalPose();
	}
	
	return _float3(vPose.p.x, vPose.p.y, vPose.p.z);
}

_float4 CRigidBody::Get_Rotation() const
{
	PxTransform vPose;
	if (false == m_isKinematic ||
		false == reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(vPose))
	{
		vPose = m_pActor->getGlobalPose();
	}
	
	return _float4(vPose.q.x, vPose.q.y, vPose.q.z, vPose.q.w);
}

void CRigidBody::Set_Constraint(RigidBodyConstraint eConstraintFlag, _bool _isEnable)
{
	PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

	//pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !_isEnable);
}

void CRigidBody::Set_Kinematic(_bool isKinematic)
{
	// static ��ü�� kinematic �ɼ� ����.
	if (true == m_isStatic)
		return;

	m_isKinematic = isKinematic;
	reinterpret_cast<PxRigidDynamic*>(m_pActor)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, isKinematic);
}

void CRigidBody::Set_Density(_float _fDensity) const
{
	if (nullptr != m_pActor && 
		nullptr != m_pActor->is<PxRigidBody>())
	{
		PxRigidBodyExt::updateMassAndInertia(*m_pActor->is<PxRigidBody>(), _fDensity);
	}
}

void CRigidBody::Set_CollisionGroup(COLLISIONDESC::COLTYPE eColType)
{
	m_CollisionGroups.word0 = eColType;
	/*for (auto& Collier : m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
		shape->GetShape()->setQueryFilterData(m_CollisionGroups);
	}*/
}

void CRigidBody::Set_CollisionIgnoreGroups(COLLISIONDESC::COLTYPE eIgnoreColType)
{
	m_CollisionGroups.word1 = eIgnoreColType;
	/*for each (auto shape in m_Colliders)
	{
		shape->GetShape()->setSimulationFilterData(m_CollisionGroups);
	}*/
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	m_CollisionGroups = PxFilterData(COLLISIONDESC::COLTYPE_PLAYER, 0, 0, 0);

	if (FAILED(Create_Actor()))
		return E_FAIL;


	return S_OK;
}

void CRigidBody::Tick(_float fTimeDelta)
{
}

HRESULT CRigidBody::Create_Actor()
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);
	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	PxScene* pPhysxScene = pPhysX_Manager->Get_PhysxScene();

	Safe_Release(pPhysX_Manager);

	PxShape* shape = pPhysX->createShape(PxCapsuleGeometry(1.f, 1.f), *pPhysX->createMaterial(0.5f, 0.5f, 0.5f), false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);

	PxVec3 tr = PxVec3(-2.f, 20.f, 5.f);
	PxTransform localTm(tr);
	m_pActor = pPhysX->createRigidDynamic(localTm);
	shape->setLocalPose(PxTransformFromSegment(PxVec3(0.f, 1.f, 0.f), PxVec3(0.f, -1.f, 0.f)));
	m_pActor->attachShape(*shape);
	
	PxShape* boxshape = pPhysX->createShape(PxBoxGeometry(1.f, 1.f, 1.f), *pPhysX->createMaterial(1.f, 0.5f, 0.f), false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);

	m_pActor->attachShape(*boxshape);
	m_pActor->setMass(50.f);
	pPhysxScene->addActor(*m_pActor);
	shape->release();
	
	PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

	// ȸ���� " �ϰ� ���� " �κи� true�� ó�����ָ� �ȴ�.
	pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	//pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	return S_OK;
}

void CRigidBody::Add_Collider(CColliderCom* collider)
{
}

void CRigidBody::Put_To_Sleep() const
{
	if (m_pActor != nullptr &&
		false == m_isStatic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->putToSleep();
	}
}

void CRigidBody::Add_Force(const _float3& _vForce, PxForceMode::Enum _eMode, _bool _bAutowake) const
{
	if (m_pActor != nullptr &&
		false == m_isStatic &&
		false == m_isKinematic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->addForce(PhysXConverter::ToPxVec3(_vForce), _eMode, _bAutowake);
	}
}

void CRigidBody::Add_Torque(const PxVec3& _vTorque, PxForceMode::Enum _eMode, _bool _bAutowake) const
{
	if (m_pActor != nullptr &&
		false == m_isStatic &&
		false == m_isKinematic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->addTorque(_vTorque, _eMode, _bAutowake);
	}
}

void CRigidBody::Clear_Force(PxForceMode::Enum _eMode) const
{
	if (m_pActor != nullptr &&
		false == m_isStatic &&
		false == m_isKinematic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->clearForce(_eMode);
	}		
}

void CRigidBody::Clear_Torque(PxForceMode::Enum _eMode) const
{
	if (m_pActor != nullptr &&
		false == m_isStatic &&
		false == m_isKinematic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->clearTorque(_eMode);
	}
}

void CRigidBody::Translate(_float3 _vPosition) const
{
	if (nullptr == m_pActor ||
		true == m_isStatic)
		return;

	if (false == m_isKinematic)
	{
		m_pActor->setGlobalPose(
			PxTransform(PhysXConverter::ToPxVec3(_vPosition),
				PhysXConverter::ToPxQuat(Get_Rotation())));
	}
	else
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(
			PxTransform(PhysXConverter::ToPxVec3(_vPosition), 
				PhysXConverter::ToPxQuat(Get_Rotation())));
	}
}

void CRigidBody::Rotate(_float4 _vRotation) const
{
	if (nullptr == m_pActor ||
		true == m_isStatic)
		return;

	if (false == m_isKinematic)
	{
		m_pActor->setGlobalPose(
			PxTransform(PhysXConverter::ToPxVec3(Get_Position()),
				PhysXConverter::ToPxQuat(_vRotation)));
	}
	else
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->setKinematicTarget(
			PxTransform(PhysXConverter::ToPxVec3(Get_Position()), 
				PhysXConverter::ToPxQuat(_vRotation)));
	}
}

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = new CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = new CRigidBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();
}