#pragma once
#include "PXAllocator.h"
#include "PXErrorCallBack.h"
#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
	DECLARE_SINGLETON(CPhysX_Manager)

public:
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	PxScene* Get_PhysxScene() const { return m_pPhysxScene; }
	PxControllerManager* Get_ControllerManager() const { return m_pControllerManager; }
	_float3 Get_PxPos() { 
		PxTransform Pose = Actor->getGlobalPose();
		return _float3(Pose.p.x, Pose.p.y, Pose.p.z); }
	
private:
	explicit CPhysX_Manager() = default;
	virtual ~CPhysX_Manager() = default;
	
public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	PxScene* Create_Scene();
	
private:
	CPXErrorCallBack			m_PXErrorCallback;
	CPXAllocator				m_PXAllocator;

private:
	PxFoundation*				m_pFoundation = { nullptr };
	PxPhysics*					m_pPhysics = { nullptr };
	PxDefaultCpuDispatcher*		m_pDefaultCpuDispatcher = { nullptr };
	
private:
	PxScene*					m_pPhysxScene = { nullptr };
	PxControllerManager*		m_pControllerManager = { nullptr };

	PxRigidDynamic* Actor;

public:
	virtual void Free() override;
};

END