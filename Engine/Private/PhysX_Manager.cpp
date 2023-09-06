#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "PhysXConverter.h"
#include "Input_Device.h"

#include <NvClothExt/ClothFabricCooker.h>

IMPLEMENT_SINGLETON(CPhysX_Manager)

#ifdef _DEBUG
const PxRenderBuffer* CPhysX_Manager::Get_RenderBuffer()
{
	if (nullptr == m_pPhysxScene)
		return nullptr;
	
	const PxRenderBuffer* RenderBuf = &m_pPhysxScene->getRenderBuffer();

	return RenderBuf;
}

_uint CPhysX_Manager::Get_LastLineBufferIndex()
{
	return m_iLastLineBufferIndex;
}

_uint CPhysX_Manager::Get_LastTriangleBufferIndex()
{
	return m_iLastTriangleBufferIndex;
}

void CPhysX_Manager::Add_LastLineBufferIndex(_uint iNumLines)
{
	m_iLastLineBufferIndex += iNumLines;
}

void CPhysX_Manager::Add_LastTriangleBufferIndex(_uint iNumTriangles)
{
	m_iLastTriangleBufferIndex += iNumTriangles;
}
#endif // _DEBUG

HRESULT CPhysX_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// �Ŀ�̼� ����
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PXAllocator, m_PXErrorCallBack);

	if (nullptr == m_pFoundation)
	{
		MSG_BOX("PxCreateFoundation failed!");
		return E_FAIL;
	}
	// ��������ü ����
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, nullptr);
	if (nullptr == m_pPhysics)
	{
		MSG_BOX("PxCreatePhysics failed!");
		return E_FAIL;
	}
	// �������� �ʿ��� ������ ����
	m_pDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);

	// �ð����� ������ �ϳ��� ���� ����
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

	// �浹ó�� �̺�Ʈ Ȱ��ȭ
	m_pPXEventCallBack = CPXEventCallBack::Create();
	m_pPhysxScene->setSimulationEventCallback(m_pPXEventCallBack);
	
	// ��Ʈ�ѷ� ������ �ʿ��� �Ŵ��� Ŭ���� ����.
	m_pControllerManager = PxCreateControllerManager(*m_pPhysxScene);
	if (nullptr == m_pControllerManager)
	{
		MSG_BOX("Failed Create ControllerManager");
		return E_FAIL;
	}

	// NvCloth Initialize
	m_pContextManagerCallBack = New CNVContextCallBack(pDevice, pContext);
	if (nullptr == m_pContextManagerCallBack)
	{
		MSG_BOX("Failed Create NvCallBack");
		return E_FAIL;
	}

	cloth::InitializeNvCloth(&m_PXAllocator, &m_PXErrorCallBack, m_pAssertHandler, nullptr);
	m_pClothFactory = NvClothCreateFactoryDX11(m_pContextManagerCallBack);
	if (nullptr == m_pClothFactory)
	{
		MSG_BOX("Failed Create ClothFactory");
		return E_FAIL;
	}


	//----------------------------------------------------------------------------------------
	//nv::cloth::ClothMeshDesc meshDesc;

	////Fill meshDesc with data
	//meshDesc.setToDefault();
	//meshDesc.points.data = vertexArray;
	//meshDesc.points.stride = sizeof(vertexArray[0]);
	//meshDesc.points.count = vertexCount;
	////etc. for quads, triangles and invMasses

	//physx::PxVec3 gravity(0.0f, -9.8f, 0.0f);
	//nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
	//nv::cloth::Fabric* fabric = NvClothCookFabricFromMesh(m_pClothFactory, meshDesc, gravity, &phaseTypeInfo);

	//physx::PxVec4* particlePositions = ...;


	//nv::cloth::Cloth* cloth = m_pClothFactory->createCloth(nv::cloth::Range<physx::PxVec4>(particlePositions, particlePositions + particleCount), *fabric);
	//// particlePositions can be freed here.'



	//nv::cloth::PhaseConfig* phases = new nv::cloth::PhaseConfig[fabric->getNumPhases()];
	//for (int i = 0; i < fabric->getNumPhases(); i++)
	//{
	//	phases[i].mPhaseIndex = i; // Set index to the corresponding set (constraint group)

	//	//Give phases different configs depending on type
	//	switch (phaseTypeInfo[i])
	//	{
	//	case nv::cloth::ClothFabricPhaseType::eINVALID:
	//		//ERROR
	//		break;
	//	case nv::cloth::ClothFabricPhaseType::eVERTICAL:
	//		break;
	//	case nv::cloth::ClothFabricPhaseType::eHORIZONTAL:
	//		break;
	//	case nv::cloth::ClothFabricPhaseType::eBENDING:
	//		break;
	//	case nv::cloth::ClothFabricPhaseType::eSHEARING:
	//		break;
	//	}

	//	//For this example we give very phase the same config
	//	phases[i].mStiffness = 1.0f;
	//	phases[i].mStiffnessMultiplier = 1.0f;
	//	phases[i].mCompressionLimit = 1.0f;
	//	phases[i].mStretchLimit = 1.0f;
	//}

	//cloth->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(phases, phases + fabric->getNumPhases()));
	//delete[] phases;

	//nv::cloth::Solver* solver = factory->createSolver();

	//solver->addCloth(cloth);
	//float deltaTime = 1.0f / 60.0f;
	//solver->beginSimulation(deltaTime);
	//for (int i = 0; i < solver->getSimulationChunkCount(); i++)
	//{
	//	solver->simulateChunk(i);
	//}

	//// Render
	//nv::cloth::MappedRange<physx::PxVec4> particles = mCloth->getCurrentParticles();
	//for (int i = 0; i < particles.size(); i++)
	//{
	//	//do something with particles[i]
	//	//the xyz components are the current positions
	//	//the w component is the invMass.
	//}


	//solver->endSimulation();
	//solver->removeCloth(cloth);
	//NV_CLOTH_DELETE(solver);

	//NV_CLOTH_DELETE(cloth);
	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	// �������� ó���� ���� �Լ���.
	// 1/60���� �����صδ� ���°� �ʿ���.
	// fTimeDelta�� ����� ��� �����ӿ� ���� ó���� �޶��� �� ����.
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
}

#ifdef _DEBUG
void CPhysX_Manager::Clear_BufferIndex()
{
	m_iLastLineBufferIndex = 0;
	m_iLastTriangleBufferIndex = 0;
}
#endif // _DEBUG

PxScene* CPhysX_Manager::Create_Scene()
{
	PxSceneDesc SceneDesc(m_pPhysics->getTolerancesScale());
	SceneDesc.setToDefault(m_pPhysics->getTolerancesScale());
	SceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	SceneDesc.cpuDispatcher = m_pDefaultCpuDispatcher;
	SceneDesc.filterShader = CollisionFilterShader;
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
	// ���� ������ ���� �̰ɷ� �����մϴ�.
	
	NvClothDestroyFactory(m_pClothFactory);
	m_pClothFactory = nullptr;

	delete m_pContextManagerCallBack;
	m_pContextManagerCallBack = nullptr;

	if (nullptr != m_pControllerManager)
	{
		m_pControllerManager->release();
	}

	if (nullptr != m_pPXEventCallBack)
	{
		m_pPXEventCallBack->Release();
		m_pPXEventCallBack = nullptr;
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