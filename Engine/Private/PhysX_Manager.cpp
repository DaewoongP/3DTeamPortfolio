#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "Calculator.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CPhysX_Manager)

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
	m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(1.f, 1.f), true, nullptr);
	if (nullptr == m_pPhysics)
	{
		MSG_BOX("PxCreatePhysics failed!");
		return E_FAIL;
	}
#include <thread>
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	int numCores = sysInfo.dwNumberOfProcessors;

	std::cout << "���� �ý����� CPU �ھ� ����: " << numCores << std::endl;


	// �������� �ʿ��� ������ ����
	m_pDefaultCpuDispatcher = PxDefaultCpuDispatcherCreate(4);

	// �ð����� ������ �ϳ��� ���� ����
	m_pPhysxScene = Create_Scene();
	if (nullptr == m_pPhysxScene)
	{
		MSG_BOX("Failed Create Scene");
		return E_FAIL;
	}
#ifdef _DEBUG
	// Debug Rendering�� ���� ó��
	/*m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 0.5f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);*/
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

	return S_OK;
}

void CPhysX_Manager::Tick(_float fTimeDelta)
{
	if (false == m_isSimulation)
		return;
	// �������� ó���� ���� �Լ���.
	// 1/60���� �����صδ� ���°� �ʿ���.
	// fTimeDelta�� ����� ��� �����ӿ� ���� ó���� �޶��� �� ����.
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
}

_bool CPhysX_Manager::RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist, _Inout_ _float3* pHitPosition, _Inout_ _float* pDist, _uint iMaxHits, RayCastQueryFlag RaycastFlag)
{
	vDir.Normalize();

	PxRaycastBuffer CallbackBuf;
	PxQueryFilterData FilterData;
	if (RAY_ONLY_DYNAMIC == RaycastFlag)
	{
		FilterData.flags = PxQueryFlag::eDYNAMIC;
	}
	else if (RAY_ONLY_STATIC == RaycastFlag)
	{
		FilterData.flags = PxQueryFlag::eSTATIC;
	}
	else
		FilterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC;

	PxHitFlags eHitFlag = PxHitFlag::ePOSITION | PxHitFlag::eNORMAL | PxHitFlag::eUV;

	_bool isCollision = m_pPhysxScene->raycast(PhysXConverter::ToPxVec3(vOrigin), PhysXConverter::ToPxVec3(vDir),
		fMaxDist, CallbackBuf, eHitFlag, FilterData);

	if (true == isCollision)
	{
		if (nullptr != pHitPosition)
			*pHitPosition = PhysXConverter::ToXMFLOAT3(CallbackBuf.block.position);
		if (nullptr != pDist)
			*pDist = CallbackBuf.block.distance;
	}

	return isCollision;
}

_bool CPhysX_Manager::Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _float fMaxDist, _float3* pHitPosition, _float* pDist, _uint iMaxHits, RayCastQueryFlag RaycastFlag)
{
	CCalculator* pCalculator = CCalculator::GetInstance();
	Safe_AddRef(pCalculator);

	_float4 vRayPos, vRayDir;
	if (FAILED(pCalculator->Get_WorldMouseRay(pContext, hWnd, &vRayPos, &vRayDir)))
	{
		Safe_Release(pCalculator);
		return false;
	}

	Safe_Release(pCalculator);

	return RayCast(vRayPos.xyz(), vRayDir.xyz(), fMaxDist, pHitPosition, pDist, iMaxHits, RaycastFlag);
}

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