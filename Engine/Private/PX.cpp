#include "..\Public\PX.h"

CPX::CPX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CPX::CPX(const CPX& rhs)
	: CComponent(rhs)
{
}

HRESULT CPX::Initialize_Prototype()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PXAllocator, m_PXErrorCallback);

	if (nullptr == m_pFoundation)
	{
		MSG_BOX("PxCreateFoundation failed!");
		return E_FAIL;
	}

	/*_bool bRecordMemoryAllocations = true;

	m_pPvd = PxCreatePvd(*m_pFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);*/

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation,
		PxTolerancesScale(), true, nullptr);
	if (nullptr == m_pPhysics)
	{
		MSG_BOX("PxCreatePhysics failed!");
		return E_FAIL;
	}
	/*PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_pDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	m_pScene = m_pPhysics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();

	m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, physx::PxPlane(0, 1, 0, 50), *m_pMaterial);
	m_pScene->addActor(*groundPlane);

	_float halfExtent = 0.5f;
	physx::PxShape* shape = m_pPhysics->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_pMaterial);
	physx::PxU32 size = 30;
	physx::PxTransform t(physx::PxVec3(0));
	for (physx::PxU32 i = 0; i < size; i++) {
		for (physx::PxU32 j = 0; j < size - i; j++) {
			physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
			physx::PxRigidDynamic* body = m_pPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			physx::PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			m_pScene->addActor(*body);

			body->getLinearVelocity();
		}
	}
	shape->release();*/

	return S_OK;
}

HRESULT CPX::Initialize(void* _pArg)
{
	return S_OK;
}

CPX* CPX::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPX* pInstance = new CPX(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPX::Clone(void* _pArg)
{
	CPX* pInstance = new CPX(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned CPX");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPX::Free()
{
	__super::Free();

	if (!m_isCloned)
	{
		m_pFoundation->release();
		m_pPhysics->release();
		m_pPvd->release();
	}
}
