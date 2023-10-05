#pragma once
#include "PXAllocator.h"
#include "PXErrorCallBack.h"
#include "PXEventCallBack.h"
#include "NVContextCallBack.h"
#include "PXAssertHandler.h"
#include "JobManager.h"

#include "PhysXConverter.h"

#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
	DECLARE_SINGLETON(CPhysX_Manager)

public:
	enum RayCastQueryFlag { RAY_ONLY_DYNAMIC, RAY_ONLY_STATIC, RAY_ALL, RAY_END };

public:
	// **피직스의 디바이스를 리턴합니다**
	// create와 관련한 형태의 처리는 거의 이 변수가 관리합니다.
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	// **피직스의 씬을 리턴합니다**
	PxScene* Get_PhysxScene() const { return m_pPhysxScene; }
	PxControllerManager* Get_ControllerManager() const { return m_pControllerManager; }
	cloth::Factory* Get_ClothFactory() const { return m_pClothFactory; }
	void Set_Simulation(_bool isSimulation) { m_isSimulation = isSimulation; }

private:
	explicit CPhysX_Manager() = default;
	virtual ~CPhysX_Manager() = default;
	
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);

public:
	// 1. vOrigin : 레이 시작지점 2. vDir : 방향 3. pCollisionObject : 충돌한 오브젝트 4. fMaxDist : 최대거리 5. pHitPosition : (out)레이가 충돌한 위치 6. pDist : (out)충돌한 거리 
	// 7. iMaxHits : 레이를 맞을 수 있는 최대 개수 8. RaycastFlag : dynamic / static / all 중에 레이와 충돌할 객체 타입
	// 반환 : 충돌 했을 시 true
	_bool RayCast(_float3 vOrigin, _float3 vDir, _Inout_ class CGameObject** ppCollisionObject = nullptr, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, RayCastQueryFlag RaycastFlag = RAY_ALL);
	// 1. pContext : Dx11 DeviceContext 2. hWnd : 클라이언트 핸들 3. pCollisionObject : 충돌한 오브젝트 4. fMaxDist : 최대거리 5. pHitPosition : (out)레이가 충돌한 위치 6. pDist : (out)충돌한 거리 
	// 7. iMaxHits : 레이를 맞을 수 있는 최대 개수 8. RaycastFlag : dynamic / static / all 중에 레이와 충돌할 객체 타입
	// 반환 : 충돌 했을 시 true
	_bool Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _Inout_ class CGameObject** ppCollisionObject = nullptr, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, RayCastQueryFlag RaycastFlag = RAY_ALL);

private: /* 에러 메세지 등 cout 처리 */
	CPXErrorCallBack			m_PXErrorCallBack;
	CPXAllocator				m_PXAllocator;
	CPXEventCallBack*			m_pPXEventCallBack;

private: /* 피직스 기본 변수들 */
	// 인스턴스 느낌.
	PxFoundation*				m_pFoundation = { nullptr };
	// 피직스의 기본 디바이스 같은 변수
	// ID3D11Device와 느낌 비슷.
	PxPhysics*					m_pPhysics = { nullptr };
	// 씬 생성에 필요한 변수
	PxDefaultCpuDispatcher*		m_pDefaultCpuDispatcher = { nullptr };
	// 피직스의 시공간을 처리하는 하나의 씬
	PxScene*					m_pPhysxScene = { nullptr };
	// 모든 컨트롤러를 제어하는 매니저
	PxControllerManager*		m_pControllerManager = { nullptr };

private: /* NvCloth */
	cloth::Factory*				m_pClothFactory = { nullptr };
	CNVContextCallBack*			m_pContextManagerCallBack = { nullptr };
	CPXAssertHandler*			m_pAssertHandler = { nullptr };

private:
	_bool						m_isSimulation = { false };

private:
	PxScene* Create_Scene();

public:
	virtual void Free() override;
};

END