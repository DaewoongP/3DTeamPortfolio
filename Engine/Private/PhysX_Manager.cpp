#include "..\Public\PhysX_Manager.h"
#include "Component_Manager.h"
#include "Calculator.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CPhysX_Manager)

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
	/*m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 0.5f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	m_pPhysxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.f);*/
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
	if (false == m_isSimulation)
		return;
	// 피직스의 처리를 위한 함수들.
	// 1/60으로 고정해두는 형태가 필요함.
	// fTimeDelta를 사용할 경우 프레임에 따라 처리가 달라질 수 있음.
	m_pPhysxScene->simulate(1 / 60.f);
	m_pPhysxScene->fetchResults(true);
}

_bool CPhysX_Manager::RayCast(_float3 vOrigin, _float3 vDir, _float fMaxDist, _Inout_ _float3* pHitPosition, _Inout_ _float* pDist, const _tchar* pObjectTag)
{// 트리거는 레이 캐스팅 안됩니다!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 막아놨어요 필요없어서 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	vDir.Normalize();

	PxRaycastHit rayhit;
	PxRaycastBuffer buf;

	_bool isCollision = m_pPhysxScene->raycast(PhysXConverter::ToPxVec3(vOrigin), PhysXConverter::ToPxVec3(vDir),
		fMaxDist, buf);

	if (true == isCollision)
	{
		if (nullptr != buf.block.shape)
		{
			CRigidBody::COLLISIONDATADESC* CollisionData = static_cast<CRigidBody::COLLISIONDATADESC*>(buf.block.shape->userData);

			if (nullptr != pObjectTag &&
				nullptr != CollisionData &&
				nullptr != CollisionData->pOwnerObject &&
				!lstrcmp(pObjectTag, CollisionData->pOwnerObject->Get_Tag()))
			{
				_float fValue = 0.03f;
				_bool isObjectColled = RayCast(vOrigin + vDir * fValue, vDir, fMaxDist, pHitPosition, pDist, pObjectTag);
				*pDist += vDir.Length() * fValue;
				return isObjectColled;
			}
		}

		if (nullptr != pHitPosition)
		{
			*pHitPosition = PhysXConverter::ToXMFLOAT3(buf.block.position);
		}

		if (nullptr != pDist)
		{
			*pDist = buf.block.distance;
		}
	}

	return isCollision;
}

_bool CPhysX_Manager::Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _float fMaxDist, _Inout_ _float3* pHitPosition, _Inout_ _float* pDist, const _tchar* pCollisionTag)
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
	
	return RayCast(vRayPos.xyz(), vRayDir.xyz(), fMaxDist, pHitPosition, pDist, pCollisionTag);
}

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
	
	// 여기서 터졌다면 플레이어가 삭제가 최종적으로 되지않았을 확률이 높습니다
	// 플레이어의 메쉬파츠가 삭제안됨 -> cloth 관련 포인터 삭제안됨 -> 팩토리 삭제 불가로 터짐
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