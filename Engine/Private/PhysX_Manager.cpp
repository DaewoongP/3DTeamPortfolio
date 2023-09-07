#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "PhysXConverter.h"
#include "Input_Device.h"

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
	// 파운데이션 생성
	m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_PXAllocator, m_PXErrorCallBack);

	if (nullptr == m_pFoundation)
	{
		MSG_BOX("PxCreateFoundation failed!");
		return E_FAIL;
	}
	// 피직스객체 생성
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(1.f, 1.f), true, nullptr);
	if (nullptr == m_pPhysics)
	{
		MSG_BOX("PxCreatePhysics failed!");
		return E_FAIL;
	}
	// 씬생성에 필요한 디스패쳐 생성
	m_pDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(2);

	// 시공간을 생성할 하나의 씬을 생성
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

	// 충돌처리 이벤트 활성화
	m_pPXEventCallBack = CPXEventCallBack::Create();
	m_pPhysxScene->setSimulationEventCallback(m_pPXEventCallBack);
	
	// 컨트롤러 생성에 필요한 매니저 클래스 생성.
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

	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	// 피직스의 처리를 위한 함수들.
	// 1/60으로 고정해두는 형태가 필요함.
	// fTimeDelta를 사용할 경우 프레임에 따라 처리가 달라질 수 있음.
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
	// 삭제 순서는 거의 이걸로 고정합니다.
	
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