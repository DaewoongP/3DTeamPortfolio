#include "..\Public\RigidBody.h"
#include "PipeLine.h"
#include "Transform.h"
#include "String_Manager.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include "CharacterController.h"

#ifdef _DEBUG
#include "Shader.h"
#include "Debug_Render_HeightField.h"
#include "Debug_Render_TriangleMesh.h"
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

void CRigidBody::Set_Constraint(_uint eConstraintFlag, _bool _isEnable)
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

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_HeightField"),
		CDebug_Render_HeightField::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender Height Field");
		Safe_Release(pComponent_Manager);
		return E_FAIL;
	}

	if (FAILED(pComponent_Manager->Add_Prototype(0, TEXT("Prototype_Component_RigidBody_Debug_Render_TriangleMesh"),
		CDebug_Render_TriangleMesh::Create(m_pDevice, m_pContext), true)))
	{
		MSG_BOX("Failed Create Prototype : RigidBody DebugRender TriangleMesh");
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

	RIGIDBODYDESC* pRigidBodyDesc = reinterpret_cast<RIGIDBODYDESC*>(pArg);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	m_pScene = pPhysX_Manager->Get_PhysxScene();

	// 초기 포지션 세팅
	if (nullptr == m_pActor)
	{
		PxTransform InitTransform(PhysXConverter::ToPxVec3(pRigidBodyDesc->vInitPosition),
			PhysXConverter::ToPxQuat(pRigidBodyDesc->vInitRotation));

		// 액터생성 - 실제 월드위치 대입.
		if (true == pRigidBodyDesc->isStatic)
			m_pActor = pPhysX->createRigidStatic(InitTransform);
		else
			m_pActor = pPhysX->createRigidDynamic(InitTransform);
	}

	Safe_Release(pPhysX_Manager);
	
	if (FAILED(Create_Collider(pRigidBodyDesc)))
		return E_FAIL;
	
	m_pScene->addActor(*m_pActor);
		
	return S_OK;
}

#ifdef _DEBUG
HRESULT CRigidBody::Render()
{
	for (_uint i = 0; i < m_Debug_Renders.size(); ++i)
	{
		if (FAILED(SetUp_ShaderResources(i)))
			return E_FAIL;

		if (FAILED(m_Shaders[i]->Begin("Debug")))
			return E_FAIL;

		if (FAILED(m_Debug_Renders[i]->Render()))
			return E_FAIL;
	}
	
	return S_OK;
}
#endif // _DEBUG

HRESULT CRigidBody::Create_Collider(RIGIDBODYDESC* pRigidBodyDesc)
{
	std::lock_guard<std::mutex> lock(mtx);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();

	Safe_Release(pPhysX_Manager);

	if (nullptr == m_pActor)
	{
		MSG_BOX("Failed Create Collider");
		return E_FAIL;
	}
	
	// 유저데이터에 이 컴포넌트 세팅해서 나중에 충돌처리 함수 부르기 위해 처리.
	m_pActor->userData = pRigidBodyDesc->pOwnerObject;
	
	// 저항 처리
	m_pMaterial = pPhysX->createMaterial(pRigidBodyDesc->fStaticFriction, 
		pRigidBodyDesc->fDynamicFriction, pRigidBodyDesc->fRestitution);
	
	PxShapeFlags ePxFlag;
	// 트리거 설정
	if (true == pRigidBodyDesc->isTrigger)
	{
		ePxFlag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eTRIGGER_SHAPE;// | PxShapeFlag::eVISUALIZATION;
	}
	else
	{
		ePxFlag = PxShapeFlag::eSCENE_QUERY_SHAPE | PxShapeFlag::eSIMULATION_SHAPE;// | PxShapeFlag::eVISUALIZATION;
	}

	m_pGeometry = pRigidBodyDesc->pGeometry;
	PxShape* pShape = pPhysX->createShape(*pRigidBodyDesc->pGeometry,
		*m_pMaterial, false, ePxFlag);

	// 충돌처리에 필요한 유저 데이터값 바인딩
	// 나중에 충돌 타입 정해서 처리할거임.
	// 32비트 데이터라 총 8개로 플래그값 설정 가능 (word3 까지 포함하면 총 32개 옵션 설정가능.)
	// 구조체에 옵션값으로 설정하게 해줘야함.
	PxFilterData FilterData;
	if (PxGeometryType::eHEIGHTFIELD == pRigidBodyDesc->pGeometry->getType() ||
		PxGeometryType::eTRIANGLEMESH == pRigidBodyDesc->pGeometry->getType())
		FilterData.word0 = 0;
	else
		FilterData.word0 = 0x1111; // 이데이터는 일단 고정.
	pShape->setSimulationFilterData(FilterData);

	PxTransform OffsetTransform(PhysXConverter::ToPxVec3(pRigidBodyDesc->vOffsetPosition), PhysXConverter::ToPxQuat(pRigidBodyDesc->vOffsetRotation));
	pShape->setLocalPose(OffsetTransform);


	CString_Manager* pString_Manager = CString_Manager::GetInstance();
	Safe_AddRef(pString_Manager);
	pShape->setName(pString_Manager->Make_Char(pRigidBodyDesc->szCollisionTag));
	Safe_Release(pString_Manager);
	
	// 액터와 씬 처리.
	// AttachShape로 콜라이더 여러개 바인딩 가능.
	// 씬도 일단 한개만 처리하게 해둬서 신경 안써도 될듯.
	m_pActor->attachShape(*pShape);
	m_Shapes.emplace(pShape->getName(), pShape);

	if (false == pRigidBodyDesc->isGravity)
	{
		m_pActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	}

#ifdef _DEBUG
	// 렌더링용 쉐이더, 버퍼
	if (FAILED(Add_Components(pRigidBodyDesc->pGeometry, pShape)))
		return E_FAIL;
	m_Colors.push_back(pRigidBodyDesc->vDebugColor);
#endif // _DEBUG

	if (false == pRigidBodyDesc->isStatic)
	{
		PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

		Set_Constraint(pRigidBodyDesc->eConstraintFlag, false);
	}

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

#ifdef _DEBUG
HRESULT CRigidBody::Add_Components(PxGeometry* pPxValues, PxShape* pShape)
{
	// Debug Shader 생성
	m_Shaders.push_back(CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements));

	_float3 vOffsetPos = _float3(pShape->getLocalPose().p.x, pShape->getLocalPose().p.y, pShape->getLocalPose().p.z);
	_float4 vOffsetRot = _float4(pShape->getLocalPose().q.x, pShape->getLocalPose().q.y, pShape->getLocalPose().q.z, pShape->getLocalPose().q.w);

	CComponent* pComponent = { nullptr };

	if (PxGeometryType::eBOX == pPxValues->getType())
	{
		// Debug Components
		CDebug_Render_Box::BOXDESC BoxDesc;
		BoxDesc.vExtents = PhysXConverter::ToXMFLOAT3(reinterpret_cast<PxBoxGeometry*>(pPxValues)->halfExtents);
		BoxDesc.vOffsetPosition = vOffsetPos;
		BoxDesc.vOffsetRotation = vOffsetRot;
		
		/* For.Com_Debug_Render_Box */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Box"),
			TEXT("Com_Debug_Render_Box"), reinterpret_cast<CComponent**>(&pComponent), &BoxDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Box)");
			return E_FAIL;
		}
	}
	else if (PxGeometryType::eSPHERE == pPxValues->getType())
	{
		CDebug_Render_Sphere::SPHEREDESC SphereDesc;
		SphereDesc.fRadius = reinterpret_cast<PxSphereGeometry*>(pPxValues)->radius;
		SphereDesc.vOffsetPosition = vOffsetPos;
		SphereDesc.vOffsetRotation = vOffsetRot;
		/* For.Com_Debug_Render_Sphere */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Sphere"),
			TEXT("Com_Debug_Render_Sphere"), reinterpret_cast<CComponent**>(&pComponent), &SphereDesc)))
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
		CapsuleDesc.vOffsetPosition = vOffsetPos;
		CapsuleDesc.vOffsetRotation = vOffsetRot;
		/* For.Com_Debug_Render_Capsule */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_Capsule"),
			TEXT("Com_Debug_Render_Capsule"), reinterpret_cast<CComponent**>(&pComponent), &CapsuleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_Capsule)");
			return E_FAIL;
		}
	}
	else if (PxGeometryType::eHEIGHTFIELD == pPxValues->getType())
	{
		CDebug_Render_HeightField::HEIGHTFIELDDESC HeightFieldDesc;
		PxHeightField* pHeightField = reinterpret_cast<PxHeightFieldGeometry*>(pPxValues)->heightField;
		HeightFieldDesc.iNumRows = pHeightField->getNbRows() - 1;
		HeightFieldDesc.iNumColumns = pHeightField->getNbColumns() - 1;
		
		vector<_float3> Positions;
		Positions.resize((HeightFieldDesc.iNumRows + 1) * (HeightFieldDesc.iNumColumns + 1));
		for (_uint i = 0; i < HeightFieldDesc.iNumColumns + 1; ++i)
		{
			for (_uint j = 0; j < HeightFieldDesc.iNumRows + 1; ++j)
			{
				_uint iIndex = j + (HeightFieldDesc.iNumRows + 1) * i;

				Positions[iIndex] = _float3((_float)j, pHeightField->getHeight(_float(j), _float(i)), (_float)i);
			}
		}

		HeightFieldDesc.pPositions = Positions.data();
		HeightFieldDesc.vOffsetPosition = vOffsetPos;
		HeightFieldDesc.vOffsetRotation = vOffsetRot;
		/* For.Com_Debug_Render_HeightField */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_HeightField"),
			TEXT("Com_Debug_Render_HeightField"), reinterpret_cast<CComponent**>(&pComponent), &HeightFieldDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_HeightField)");
			return E_FAIL;
		}

		pHeightField->release();
	}
	else if (PxGeometryType::eTRIANGLEMESH == pPxValues->getType())
	{
		CDebug_Render_TriangleMesh::TRIANGLEMESHDESC TriangleMeshDesc;
		PxTriangleMesh* pTriangleMesh = reinterpret_cast<PxTriangleMeshGeometry*>(pPxValues)->triangleMesh;
		const PxVec3* pVertices = pTriangleMesh->getVertices();
		const void* pTriangles = pTriangleMesh->getTriangles();
		_uint iNumVertices = pTriangleMesh->getNbVertices();
		_uint iNumTriangles = pTriangleMesh->getNbTriangles();
		
		TriangleMeshDesc.iNumVertices = iNumVertices;
		TriangleMeshDesc.iNumIndices = iNumTriangles * 3;
		TriangleMeshDesc.vOffsetPosition = vOffsetPos;
		TriangleMeshDesc.vOffsetRotation = vOffsetRot;
		vector<_float3> Vertices;
		vector<_ushort> Indices;
		Vertices.reserve(iNumVertices);
		Indices.resize(TriangleMeshDesc.iNumIndices);
		for (_uint i = 0; i < iNumVertices; ++i)
		{
			Vertices.push_back(PhysXConverter::ToXMFLOAT3(pVertices[i]));
		}

		// 피직스 내부적으로 16비트로 저장함
		memcpy(Indices.data(), pTriangles, sizeof(_ushort) * TriangleMeshDesc.iNumIndices);
		TriangleMeshDesc.pIndices = Indices.data();
		TriangleMeshDesc.pVertices = Vertices.data();
		
		/* For.Com_Debug_Render_TriangleMesh */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_RigidBody_Debug_Render_TriangleMesh"),
			TEXT("Com_Debug_Render_TriangleMesh"), reinterpret_cast<CComponent**>(&pComponent), &TriangleMeshDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Debug_Render_TriangleMesh)");
			return E_FAIL;
		}
	}

	if (nullptr == pComponent)
		return E_FAIL;

	m_Debug_Renders.push_back(pComponent);

	return S_OK;
}

HRESULT CRigidBody::SetUp_ShaderResources(_uint iColliderIndex)
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	PxTransform Trans = m_pActor->getGlobalPose();
	_float3 vPos = PhysXConverter::ToXMFLOAT3(Trans.p);
	_float4 vQuat(Trans.q.x, Trans.q.y, Trans.q.z, Trans.q.w);

	_float4x4 WorldMatrix = XMMatrixRotationQuaternion(vQuat) * XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

	if (FAILED(m_Shaders[iColliderIndex]->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_Shaders[iColliderIndex]->Bind_Matrix("g_ViewMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_Shaders[iColliderIndex]->Bind_Matrix("g_ProjMatrix", pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_Shaders[iColliderIndex]->Bind_RawValue("g_vColor", &m_Colors[iColliderIndex], sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	return S_OK;
}
#endif // _DEBUG

CRigidBody* CRigidBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRigidBody* pInstance = New CRigidBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CRigidBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRigidBody::Clone(void* pArg)
{
	CRigidBody* pInstance = New CRigidBody(*this);

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

	if (nullptr != m_pActor)
	{
		m_pActor->userData = nullptr;
		m_pActor->release();
	}
	
#ifdef _DEBUG
	for (auto& pShader : m_Shaders)
		Safe_Release(pShader);
	m_Shaders.clear();
	for (auto& pComponent : m_Debug_Renders)
		Safe_Release(pComponent);
	m_Debug_Renders.clear();
#endif // _DEBUG
}
