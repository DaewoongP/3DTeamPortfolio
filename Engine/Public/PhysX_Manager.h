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
	// **�������� ����̽��� �����մϴ�**
	// create�� ������ ������ ó���� ���� �� ������ �����մϴ�.
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	// **�������� ���� �����մϴ�**
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
	// 1. vOrigin : ���� �������� 2. vDir : ���� 3. pCollisionObject : �浹�� ������Ʈ 4. fMaxDist : �ִ�Ÿ� 5. pHitPosition : (out)���̰� �浹�� ��ġ 6. pDist : (out)�浹�� �Ÿ� 
	// 7. iMaxHits : ���̸� ���� �� �ִ� �ִ� ���� 8. RaycastFlag : dynamic / static / all �߿� ���̿� �浹�� ��ü Ÿ��
	// ��ȯ : �浹 ���� �� true
	_bool RayCast(_float3 vOrigin, _float3 vDir, _Inout_ class CGameObject** ppCollisionObject = nullptr, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, RayCastQueryFlag RaycastFlag = RAY_ALL);
	// 1. pContext : Dx11 DeviceContext 2. hWnd : Ŭ���̾�Ʈ �ڵ� 3. pCollisionObject : �浹�� ������Ʈ 4. fMaxDist : �ִ�Ÿ� 5. pHitPosition : (out)���̰� �浹�� ��ġ 6. pDist : (out)�浹�� �Ÿ� 
	// 7. iMaxHits : ���̸� ���� �� �ִ� �ִ� ���� 8. RaycastFlag : dynamic / static / all �߿� ���̿� �浹�� ��ü Ÿ��
	// ��ȯ : �浹 ���� �� true
	_bool Mouse_RayCast(HWND hWnd, ID3D11DeviceContext* pContext, _Inout_ class CGameObject** ppCollisionObject = nullptr, _float fMaxDist = PX_MAX_F32, _Inout_ _float3* pHitPosition = nullptr, _Inout_ _float* pDist = nullptr, _uint iMaxHits = 1, RayCastQueryFlag RaycastFlag = RAY_ALL);

private: /* ���� �޼��� �� cout ó�� */
	CPXErrorCallBack			m_PXErrorCallBack;
	CPXAllocator				m_PXAllocator;
	CPXEventCallBack*			m_pPXEventCallBack;

private: /* ������ �⺻ ������ */
	// �ν��Ͻ� ����.
	PxFoundation*				m_pFoundation = { nullptr };
	// �������� �⺻ ����̽� ���� ����
	// ID3D11Device�� ���� ���.
	PxPhysics*					m_pPhysics = { nullptr };
	// �� ������ �ʿ��� ����
	PxDefaultCpuDispatcher*		m_pDefaultCpuDispatcher = { nullptr };
	// �������� �ð����� ó���ϴ� �ϳ��� ��
	PxScene*					m_pPhysxScene = { nullptr };
	// ��� ��Ʈ�ѷ��� �����ϴ� �Ŵ���
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