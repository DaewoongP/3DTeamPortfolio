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
	// **�������� ����̽��� �����մϴ�**
	// create�� ������ ������ ó���� ���� �� ������ �����մϴ�.
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	// **�������� ���� �����մϴ�**
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

#ifdef _DEBUG
	private:
	// ����� �������� �ʿ��� �ε�����
	// ������ �ε����� �����صΰ� ������ ó����.
	_uint						m_iLastLineBufferIndex = { 0 };
	// ����� �������� �ʿ��� �ε�����
	// ������ �ε����� �����صΰ� ������ ó����.
	_uint						m_iLastTriangleBufferIndex = { 0 };
#endif // _DEBUG

private:
	PxScene* Create_Scene();

public:
	virtual void Free() override;
};

END