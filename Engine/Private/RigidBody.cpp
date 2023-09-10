#include "..\Public\RigidBody.h"
#include "PipeLine.h"
#include "Transform.h"
#include "PhysX_Manager.h"
#include "PhysXConverter.h"
#include "CharacterController.h"

#ifdef _DEBUG
#include "Shader.h"
#include "VIBuffer_Line.h"
#include "VIBuffer_Triangle.h"
#endif // _DEBUG


CRigidBody::CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComposite(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	: CComposite(rhs)
#ifdef _DEBUG
	, m_vColor(_float4(1.f, 0.f, 0.f, 1.f))
#endif // _DEBUG
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

HRESULT CRigidBody::Initialize(void* pArg)
{
	if (FAILED(Create_Actor()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Add_Components()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

void CRigidBody::Late_Tick(_float fTimeDelta)
{
	/*m_pActor->addForce(m_pActor->getLinearVelocity() * -m_fAirDrag);
	m_pActor->addTorque(m_pActor->getAngularVelocity() * -m_fAirDrag);*/

#ifdef _DEBUG
	Make_Buffers();
#endif // _DEBUG
}

#ifdef _DEBUG
HRESULT CRigidBody::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin("Debug")))
		return E_FAIL;

	if (FAILED(m_pLine->Render()))
		return E_FAIL;

	if (FAILED(m_pTriangle->Render()))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRigidBody::Create_Actor()
{
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	// 매니저를 통해 피직스 객체 가져옴
	PxPhysics* pPhysX = pPhysX_Manager->Get_Physics();
	// 액터를 바인딩할 씬 가져옴
	m_pScene = pPhysX_Manager->Get_PhysxScene();

	// 초기 포지션 세팅
	PxVec3 vLocal = PxVec3(5.f, 10.f, 5.f);
	PxTransform localTm(vLocal);
	m_pActor = pPhysX->createRigidDynamic(localTm);
	// 유저데이터에 이 컴포넌트 세팅해서 나중에 충돌처리 함수 부르기 위해 처리.
	m_pActor->userData = this;
	// 저항 처리
	m_pMaterial = pPhysX->createMaterial(0.5f, 0.5f, 0.5f);

#ifdef _DEBUG // 렌더링
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);
	_uint iPrevLines = m_pScene->getRenderBuffer().getNbLines();
	_uint iPrevTriangles = m_pScene->getRenderBuffer().getNbTriangles();

	m_iStartLineBufferIndex = pPhysX_Manager->Get_LastLineBufferIndex();
	m_iStartTriangleBufferIndex = pPhysX_Manager->Get_LastTriangleBufferIndex();
#endif // _DEBUG

#ifdef _DEBUG // 렌더링 선택
	// shape 생성 (1번 매개변수에 Capsule, Box, Sphere등 선택가능합니다 (Px~Geometry 쓰면됨.))
	PxShape* boxshape = pPhysX->createShape(PxCapsuleGeometry(1.f, 1.f), *m_pMaterial, false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);
#else
	PxShape* boxshape = pPhysX->createShape(PxCapsuleGeometry(1.f, 1.f), *m_pMaterial, false, PxShapeFlag::eSIMULATION_SHAPE);
#endif // _DEBUG

	// 충돌처리에 필요한 유저 데이터값 바인딩
	PxFilterData data;
	data.word0 = 0x1111; // 이데이터는 일단 고정.
	boxshape->setSimulationFilterData(data);

	// OffsetPosition 처리 (피직스는 오른손 좌표계를 사용해서 왼손좌표계로 바꿔주기위해 회전 처리합니다.)
	// 이거 확인정확하게 안해봐서 앞뒤가 정확한지는 모르겠어요
	// 근데 앞뒤 달라도 상관없을거 같긴함.
	_float4 vQuaternion = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.f), XMConvertToRadians(90.f));
	PxTransform relativePose(PxQuat(PhysXConverter::ToPxQuat(vQuaternion)));
	boxshape->setLocalPose(relativePose);

	// 액터와 씬 처리.
	// AttachShape로 콜라이더 여러개 바인딩 가능.
	// 씬도 일단 한개만 처리하게 해둬서 신경 안써도 될듯.
	m_pActor->attachShape(*boxshape);
	m_pScene->addActor(*m_pActor);
	

	PxRigidDynamic* pRigidBody = m_pActor->is<PxRigidDynamic>();

	// 회전을 " 하기 싫은 " 부분만 true로 처리해주면 된다.
	pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
	pRigidBody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

#ifdef _DEBUG
	// 다음 렌더링을 위한 갱신 처리
	m_pScene->simulate(1 / 60.f);
	m_pScene->fetchResults(true);
	_uint iNewLines = m_pScene->getRenderBuffer().getNbLines();
	_uint iNewTriangles = m_pScene->getRenderBuffer().getNbTriangles();

	m_iNumLineBuffer = iNewLines - iPrevLines;
	m_iNumTriangleBuffer = iNewTriangles - iPrevTriangles;
	pPhysX_Manager->Add_LastLineBufferIndex(iNewLines - iPrevLines);
	pPhysX_Manager->Add_LastTriangleBufferIndex(iNewTriangles - iPrevTriangles);
#endif // _DEBUG

	Safe_Release(pPhysX_Manager);

	return S_OK;
}

HRESULT CRigidBody::SetUp_Actor(_float3 _vInitPos, PxGeometry _ShapeType, _bool _isTrigger, RigidBodyConstraint eConstraintFlag, _float3 _vResistance, PxFilterData FilterData)
{
	return E_NOTIMPL;
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
HRESULT CRigidBody::Add_Components()
{
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
	m_Components.emplace(TEXT("Com_Shader"), m_pShader);
	Safe_AddRef(m_pShader);
	
	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();

	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	LineDesc.iNum = m_iNumLineBuffer;
	const PxDebugLine* pLines = pBuffer->getLines();

	vector<_float3> Lines;
	for (_uint i = m_iStartLineBufferIndex;
		i < m_iStartLineBufferIndex + m_iNumLineBuffer; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pLines[i].pos1));
	}
	LineDesc.pLines = Lines.data();

	if (0 < LineDesc.iNum &&
		nullptr != LineDesc.pLines)
	{
		/* For.Com_Line */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Line"),
			TEXT("Com_Line"), reinterpret_cast<CComponent**>(&m_pLine), &LineDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Line)");
			return E_FAIL;
		}
	}

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	TriangleDesc.iNum = m_iNumTriangleBuffer;
	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();

	vector<_float3> Triangles;
	for (_uint i = m_iStartTriangleBufferIndex;
		i < m_iStartTriangleBufferIndex + m_iNumTriangleBuffer; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.pTriangles = Triangles.data();

	if (0 < TriangleDesc.iNum &&
		nullptr != TriangleDesc.pTriangles)
	{
		/* For.Com_Triangle */
		if (FAILED(CComposite::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Triangle"),
			TEXT("Com_Triangle"), reinterpret_cast<CComponent**>(&m_pTriangle), &TriangleDesc)))
		{
			MSG_BOX("Failed CRigidBody Add_Component : (Com_Triangle)");
			return E_FAIL;
		}
	}

	Safe_Release(pPhysX_Manager);

	return S_OK;
}

HRESULT CRigidBody::SetUp_ShaderResources()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 WorldMatrix = XMMatrixIdentity();

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

void CRigidBody::Make_Buffers()
{
	CVIBuffer_Line::LINEDESC LineDesc;
	ZEROMEM(&LineDesc);

	CPhysX_Manager* pPhysX_Manager = CPhysX_Manager::GetInstance();
	Safe_AddRef(pPhysX_Manager);

	const PxRenderBuffer* pBuffer = pPhysX_Manager->Get_RenderBuffer();

	Safe_Release(pPhysX_Manager);

	const PxDebugLine* pDebugLines = pBuffer->getLines();

	vector<_float3> Lines;
	for (_uint i = m_iStartLineBufferIndex;
		i < m_iStartLineBufferIndex + m_iNumLineBuffer; ++i)
	{
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos0));
		Lines.push_back(PhysXConverter::ToXMFLOAT3(pDebugLines[i].pos1));
	}

	LineDesc.iNum = m_iNumLineBuffer;
	LineDesc.pLines = Lines.data();

	m_pLine->Tick(LineDesc);

	CVIBuffer_Triangle::TRIANGLEDESC TriangleDesc;
	ZEROMEM(&TriangleDesc);

	const PxDebugTriangle* pDebugTriangles = pBuffer->getTriangles();

	vector<_float3> Triangles;
	for (_uint i = m_iStartTriangleBufferIndex;
		i < m_iStartTriangleBufferIndex + m_iNumTriangleBuffer; ++i)
	{
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos0));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos1));
		Triangles.push_back(PhysXConverter::ToXMFLOAT3(pDebugTriangles[i].pos2));
	}
	TriangleDesc.iNum = m_iNumTriangleBuffer;
	TriangleDesc.pTriangles = Triangles.data();

	m_pTriangle->Tick(TriangleDesc);
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
	Safe_Release(m_pTriangle);
	Safe_Release(m_pShader);
	Safe_Release(m_pLine);
#endif // _DEBUG
}
