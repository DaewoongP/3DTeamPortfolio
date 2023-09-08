#pragma once
#include "PXAllocator.h"
#include "PXErrorCallBack.h"
#include "PXEventCallBack.h"
#include "NVContextCallBack.h"
#include "PXAssertHandler.h"
#include "JobManager.h"
#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
	DECLARE_SINGLETON(CPhysX_Manager)

public:
	// **피직스의 디바이스를 리턴합니다**
	// create와 관련한 형태의 처리는 거의 이 변수가 관리합니다.
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	// **피직스의 씬을 리턴합니다**
	PxScene* Get_PhysxScene() const { return m_pPhysxScene; }
	PxControllerManager* Get_ControllerManager() const { return m_pControllerManager; }
	cloth::Factory* Get_ClothFactory() const { return m_pClothFactory; }

#ifdef _DEBUG
	const PxRenderBuffer* Get_RenderBuffer();
	_uint Get_LastLineBufferIndex();
	_uint Get_LastTriangleBufferIndex();
	void Add_LastLineBufferIndex(_uint iNumLines);
	void Add_LastTriangleBufferIndex(_uint iNumTriangles);
	void Clear_BufferIndex();
#endif // _DEBUG

private:
	explicit CPhysX_Manager() = default;
	virtual ~CPhysX_Manager() = default;
	
public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);

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

#ifdef _DEBUG
	private:
	// 디버그 렌더링에 필요한 인덱스값
	// 마지막 인덱스를 저장해두고 렌더링 처리함.
	_uint						m_iLastLineBufferIndex = { 0 };
	// 디버그 렌더링에 필요한 인덱스값
	// 마지막 인덱스를 저장해두고 렌더링 처리함.
	_uint						m_iLastTriangleBufferIndex = { 0 };
#endif // _DEBUG

private:
	PxScene* Create_Scene();

public:
	virtual void Free() override;
};

END