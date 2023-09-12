#include "..\Public\RigidBody.h"
#include "PipeLine.h"
#include "Transform.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include "CharacterController.h"

#ifdef _DEBUG
#include "Shader.h"
#include "Debug_Render_Box.h"
#include "Debug_Render_Sphere.h"
#include "Debug_Render_Capsule.h"
#include "Component_Manager.h"
#endif // _DEBUG


CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComposite(rhs)
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

void CRigidBody::Set_Position(_float3 vPosition)
{
	PxTransform vPose;
	if (false == m_isKinematic ||
		false == reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(vPose))
	{
		vPose = PxTransform(PhysXConverter::ToPxVec3(vPosition), PhysXConverter::ToPxQuat(Get_Rotation()));
		m_pActor->setGlobalPose(vPose);
	}
}

void CRigidBody::Set_Rotation(_float4 vRotation)
{
	PxTransform vPose;
	if (false == m_isKinematic ||
		false == reinterpret_cast<PxRigidDynamic*>(m_pActor)->getKinematicTarget(vPose))
	{
		vPose = PxTransform(PhysXConverter::ToPxVec3(Get_Position()), PhysXConverter::ToPxQuat(vRotation));
		m_pActor->setGlobalPose(vPose);
	}
}

void CRigidBody::Set_Material(_float3 vMaterial)
{
	m_pMaterial->release();
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	m_pMaterial = pPhysX->createMaterial(vMaterial.x, vMaterial.y, vMaterial.z);
	Safe_Release(pPhysX_Manager);
}

void CRigidBody::Set_Constraint(RigidBodyConstraint eConstraintFlag, _bool _isEnable)
{
	if (nullptr == m_pActor)
		return;

	PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

	if (eConstraintFlag & TransX)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !_isEnable);
	}
	if (eConstraintFlag & TransY)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !_isEnable);
	}
	if (eConstraintFlag & TransZ)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !_isEnable);
	}

	if (eConstraintFlag & RotX)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !_isEnable);
	}
	if (eConstraintFlag & RotY)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !_isEnable);
	}
	if (eConstraintFlag & RotZ)
	{
		pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !_isEnable);
	}
}

void CRigidBody::Set_Kinematic(_bool isKinematic)
{
	// static 객체는 kinematic 옵션 없음.
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

void CRigidBody::Set_AngularDamping(_float _fAngualrDamping) const
{
	if (nullptr != m_pActor &&
		nullptr != m_pActor->is<PxRigidBody>())
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->setAngularDamping(_fAngualrDamping);
	}
}

HRESULT CRigidBody::Initialize_Prototype()
{
#ifdef _DEBUG
	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Box"), 
		CDebug_Render_Box::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Box");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Sphere"),
		CDebug_Render_Sphere::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Sphere");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}
	
	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Capsule"),
		CDebug_Render_Capsule::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Capsule");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	Safe_Release(pComponent_Manager);
#endif // _DEBUG

	return S_OK;
}

HRESULT CRigidBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Failed Clone RigidBody : Argument is NULL");
		return E_FAIL;
	}

	RIGIDBODYDESC* pRigidDesc = reinterpret_cast<RIGIDBODYDESC*>(pArg);
	
	if (FAILED(Create_Actor(pRigidDesc)))
		return E_FAIL;
	
#ifdef _DEBUG
	m_vColor = pRigidDesc->vDebugColor;

	if (FAILED(Add_Components(pRigidDesc->pGeometry)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRigidBody::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pDebug_Render->Render()))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRigidBody::Create_Actor(RIGIDBODYDESC* pRigidBodyDesc)
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	// 매니저를 통해 피직스 객체 가져옴
	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	// 액터를 바인딩할 씬 가져옴
	m_pScene = pPhysX_Manager->Get_PhysxScene();

	// 초기 포지션 세팅
	PxTransform InitTransform(PhysXConverter::ToPxVec3(pRigidBodyDesc->vInitPosition));
	
	if (true == pRigidBodyDesc->isStatic)
	{
		m_pActor = pPhysX->createRigidStatic(InitTransform);
	}
	else
	{
		m_pActor = pPhysX->createRigidDynamic(InitTransform);
	}

	if (nullptr == m_pActor)
	{
		MSG_BOX("Failed Create RigidBody Actor");
		Safe_Release(pPhysX_Manager);
		return E_FAIL;
	}
	
	// 유저데이터에 이 컴포넌트 세팅해서 나중에 충돌처리 함수 부르기 위해 처리.
	m_pActor->userData = pRigidBodyDesc->pOwnerObject;
	
	// 저항 처리
	m_pMaterial = pPhysX->createMaterial(pRigidBodyDesc->fStaticFriction, 
		pRigidBodyDesc->fDynamicFriction, 
		pRigidBodyDesc->fRestitution);

	PxShapeFlags ePxFlag;
	// 트리거 설정
	// Query : 레이캐스트 처리
	if (true == pRigidBodyDesc->isTrigger)
	{
		ePxFlag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE;
	}
	else
	{
		ePxFlag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;
	}

	m_pGeometry = pRigidBodyDesc->pGeometry;
	PxShape* pShape = pPhysX->createShape(*pRigidBodyDesc->pGeometry,
		*m_pMaterial, false, ePxFlag);

	// 충돌처리에 필요한 유저 데이터값 바인딩
	// 나중에 충돌 타입 정해서 처리할거임.
	// 32비트 데이터라 총 8개로 플래그값 설정 가능 (word3 까지 포함하면 총 32개 옵션 설정가능.)
	// 구조체에 옵션값으로 설정하게 해줘야함.
	PxFilterData FilterData;
	FilterData.word0 = 0x1111; // 이데이터는 일단 고정.
	pShape->setSimulationFilterData(FilterData);

	// OffsetPosition 처리 (피직스는 오른손 좌표계를 사용해서 왼손좌표계로 바꿔주기위해 회전 처리합니다.)
	// 이거 확인정확하게 안해봐서 앞뒤가 정확한지는 모르겠어요
	// 근데 앞뒤 달라도 상관없을거 같긴함.
	_float4 vQuaternion = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.f), XMConvertToRadians(90.f));
	PxTransform relativePose(PxQuat(PhysXConverter::ToPxQuat(vQuaternion)));
	pShape->setLocalPose(relativePose);

	// 액터와 씬 처리.
	// AttachShape로 콜라이더 여러개 바인딩 가능.
	// 씬도 일단 한개만 처리하게 해둬서 신경 안써도 될듯.
	m_pActor->attachShape(*pShape);
	m_pScene->addActor(*m_pActor);
	
	if (false == pRigidBodyDesc->isStatic)
	{
		PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

		Set_Constraint(pRigidBodyDesc->Constraint, false);
	}

	Safe_Release(pPhysX_Manager);

	return S_OK;
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

void CRigidBody::Add_Torque(const _float3& _vTorque, PxForceMode::Enum _eMode, _bool _bAutowake) const
{
	if (m_pActor != nullptr &&
		false == m_isStatic &&
		false == m_isKinematic)
	{
		reinterpret_cast<PxRigidDynamic*>(m_pActor)->addTorque(PhysXConverter::ToPxVec3(_vTorque), _eMode, _bAutowake);
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

PxRaycastHit CRigidBody::RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist, _uint iMaxHits)
{
	PxRaycastHit HitInfo;
	const PxHitFlags eHitFlags = PxHitFlag::eDEFAULT;
	PxRaycastBuffer buf;
	if (true == m_pScene->raycast(PhysXConverter::ToPxVec3(vOrigin), PhysXConverter::ToPxVec3(vDir),
		fMaxDist, buf))
	{
		int a = 1;
	}

	/*PxU32 hitCount = PxGeometryQuery::raycast(PhysXConverter::ToPxVec3(vOrigin), PhysXConverter::ToPxVec3(vDir),
		*m_pGeometry, m_pActor->getGlobalPose(), fMaxDist, eHitFlags, iMaxHits, &HitInfo);*/
	
	return HitInfo;
}

#ifdef _DEBUG
HRESULT CRigidBody::Add_Components(PxGeometry* pPxValues)
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_Components.emplace(TEXT("Com_Shader"), m_pShader);
	Safe_AddRef(m_pShader);

	if (PxGeometryType::eBOX == pPxValues->getType())
	{
		// Debug Components
		CDebug_Render_Box::BOXDESC BoxDesc;
		BoxDesc.vExtents = PhysXConverter::ToXMFLOAT3(reinterpret_cast<PxBoxGeometry*>(pPxValues)->halfExtents);
		BoxDesc.vOrigin = _float3(0.f, 0.f, 0.f);
		/* For.Com_Debug_Render_Box */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Box"),
			TEXT("Com_Debug_Render_Box"), reinterpret_cast<CComponent**>(&m_pDebug_Render), &BoxDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Box)");
			return E_FAIL;
		}
	}
	else if (PxGeometryType::eSPHERE == pPxValues->getType())
	{
		CDebug_Render_Sphere::SPHEREDESC SphereDesc;
		SphereDesc.fRadius = reinterpret_cast<PxSphereGeometry*>(pPxValues)->radius;
		SphereDesc.vOrigin = _float3(0.f, 0.f, 0.f);
		/* For.Com_Debug_Render_Sphere */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Sphere"),
			TEXT("Com_Debug_Render_Sphere"), reinterpret_cast<CComponent**>(&m_pDebug_Render), &SphereDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Sphere)");
			return E_FAIL;
		}
	}
	else if (PxGeometryType::eCAPSULE == pPxValues->getType())
	{
		CDebug_Render_Capsule::CAPSULEDESC CapsuleDesc;
		CapsuleDesc.fRadius = reinterpret_cast<PxCapsuleGeometry*>(pPxValues)->radius;
		CapsuleDesc.fHalfHeight = reinterpret_cast<PxCapsuleGeometry*>(pPxValues)->halfHeight;
		CapsuleDesc.vOrigin = _float3(0.f, 0.f, 0.f);
		/* For.Com_Debug_Render_Sphere */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Capsule"),
			TEXT("Com_Debug_Render_Capsule"), reinterpret_cast<CComponent**>(&m_pDebug_Render), &CapsuleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Capsule)");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CRigidBody::SetUp_ShaderResources()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	PxTransform Trans = m_pActor->getGlobalPose();
	_float3 vPos = PhysXConverter::ToXMFLOAT3(Trans.p);
	_float4 vQuat(Trans.q.x, Trans.q.y, Trans.q.z, Trans.q.w);

	_float4x4 WorldMatrix = XMMatrixRotationQuaternion(vQuat) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &m_vColor, sizeof(m_vColor))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	return S_OK;
}
#endif // _DEBUG

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

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pDebug_Render);
#endif // _DEBUG
}
