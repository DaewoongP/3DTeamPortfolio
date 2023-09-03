#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "PhysXConverter.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CPhysX_Manager)

HRESULT CPhysX_Manager::Initialize()
{
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PXAllocator, m_PXErrorCallback);

	if (nullptr == m_pFoundation)
	{
		MSG_BOX("PxCreateFoundation failed!");
		return E_FAIL;
	}

	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, nullptr);
	if (nullptr == m_pPhysics)
	{
		MSG_BOX("PxCreatePhysics failed!");
		return E_FAIL;
	}

	m_pDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);

	m_pPhysxScene = Create_Scene();
	if (nullptr == m_pPhysxScene)
	{
		MSG_BOX("Failed Create Scene");
		return E_FAIL;
	}
#ifdef _DEBUG
	// Debug Rendering�� ���� ó��
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 0.5f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);
#endif // _DEBUG

	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	if (nullptr == m_pControllerManager)
	{
		MSG_BOX("Failed Create ControllerManager");
		return E_FAIL;
	}

	PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysics, PxPlane(0, 1, 0, 0), *m_pPhysics->createMaterial(0.5f, 0.5f, 0.5f));
	m_pPhysxScene->addActor(*groundPlane);

	PxShape* shape = m_pPhysics->createShape(PxCapsuleGeometry(1.f, 1.f), *m_pPhysics->createMaterial(0.f, 0.f, 0.f), false, PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSIMULATION_SHAPE);

	PxVec3 tr = PxVec3(5.f, 50.f, 5.f);
	PxTransform localTm(tr);
	Actor = m_pPhysics->createRigidDynamic(localTm);
	shape->setLocalPose(PxTransformFromSegment(PxVec3(0.f, 1.f, 0.f), PxVec3(0.f, -1.f, 0.f)));

	Actor->attachShape(*shape);

	PxRigidBodyExt::updateMassAndInertia(*Actor, 10.0f);
	m_pPhysxScene->addActor(*Actor);
	shape->release();
	
	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	// fixed time ó�� �ʿ��Ҽ��� ����.
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
	
}

HRESULT CPhysX_Manager::Render()
{
	return S_OK;
}

PxScene* CPhysX_Manager::Create_Scene()
{
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader;
	SceneDesc.userData = nullptr; // �����͸� ���⿡ �־�δ°͵� ����.

	PxScene* pScene = m_pPhysics->createScene(SceneDesc);
	
	if (nullptr == pScene)
	{
		MSG_BOX("Failed Create Scene");
		return nullptr;
	}

	return pScene;
}

void CPhysX_Manager::Free()
{
	// ���� ����
	if (nullptr != m_pControllerManager)
	{
		m_pControllerManager->release();
	}

	if (nullptr != m_pPhysxScene)
	{
		m_pPhysxScene->release();
	}

	if (nullptr != m_pPhysics)
	{
		m_pPhysics->release();
	}
	
	if (nullptr != m_pDefaultCpuDispatcher)
	{
		m_pDefaultCpuDispatcher->release();
	}

	if (nullptr != m_pFoundation)
	{
		m_pFoundation->release();
	}
}