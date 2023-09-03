#pragma once
#include "PXAllocator.h"
#include "PXErrorCallBack.h"
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
	const PxRenderBuffer* Get_RenderBuffer();
	_uint Get_LastLineBufferIndex();
	_uint Get_LastTriangleBufferIndex();

private:
	explicit CPhysX_Manager() = default;
	virtual ~CPhysX_Manager() = default;
	
public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);

private: /* ���� �޼��� �� cout ó�� */
	CPXErrorCallBack			m_PXErrorCallback;
	CPXAllocator				m_PXAllocator;

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
	
private:
	// ����� �������� �ʿ��� �ε�����
	// ������ �ε����� �����صΰ� ������ ó����.
	_uint						m_iLastLineBufferIndex = { 0 };
	// ����� �������� �ʿ��� �ε�����
	// ������ �ε����� �����صΰ� ������ ó����.
	_uint						m_iLastTriangleBufferIndex = { 0 };

	_uint						m_iNumPlaneLineBuffer = { 0 };
	_uint						m_iNumPlaneTriangleBuffer = { 0 };

private:
	PxScene* Create_Scene();

public:
	virtual void Free() override;
};

END