#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "PhysXConverter.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CPhysX_Manager)

const PxRenderBuffer* CPhysX_Manager::Get_RenderBuffer()
{
	if (nullptr == m_pPhysxScene)
		return nullptr;

	const PxRenderBuffer* RenderBuf = &m_pPhysxScene->getRenderBuffer();

	return RenderBuf;
}

_uint CPhysX_Manager::Get_LastLineBufferIndex()
{
	m_iLastLineBufferIndex = m_pPhysxScene->getRenderBuffer().getNbLines();

	return m_iLastLineBufferIndex;
}

_uint CPhysX_Manager::Get_LastTriangleBufferIndex()
{
	m_iLastTriangleBufferIndex = m_pPhysxScene->getRenderBuffer().getNbTriangles();

	return m_iLastTriangleBufferIndex;
}

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
	// Debug Rendering을 위한 처리
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
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
	m_iLastLineBufferIndex = m_pPhysxScene->getRenderBuffer().getNbLines();
	m_iLastTriangleBufferIndex = m_pPhysxScene->getRenderBuffer().getNbTriangles();
	
	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	// fixed time 처리 필요할수도 있음.
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
}

PxScene* CPhysX_Manager::Create_Scene()
{
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	SceneDesc.filterShader = PxDefaultSimulationFilterShader;
	SceneDesc.userData = nullptr; // 데이터를 여기에 넣어두는것도 가능.

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
	// 순서 주의
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