#pragma once
#include "PXAllocator.h"
#include "PXErrorCallBack.h"
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPX : public CComponent
{
private:
	explicit CPX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPX(const CPX& rhs);
	virtual ~CPX() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg) override;

private:
	CPXErrorCallBack			m_PXErrorCallback;
	CPXAllocator				m_PXAllocator;

private:
	PxFoundation*				m_pFoundation = { nullptr };
	PxPvd*						m_pPvd = { nullptr };
	PxPhysics*					m_pPhysics = { nullptr };
	PxGeometry*					m_pGeoMetry = { nullptr };
	PxMaterial*					m_pMaterial = { nullptr };
	PxDefaultCpuDispatcher*		m_pDispatcher = { nullptr };
	PxScene*					m_pScene = { nullptr };

public:
	static CPX* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END