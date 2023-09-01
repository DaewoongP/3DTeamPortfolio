#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics final : public CComponent
{
private:
	explicit CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPhysics(const CPhysics& rhs);
	virtual ~CPhysics() = default;

public:
	PxScene* Get_PhysxScene() const { return m_pPhysxScene; }
	PxControllerManager* Get_ControllerManager() const { return m_pControllerManager; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif // _DEBUG
		
private:
	PxScene*				m_pPhysxScene = { nullptr };
	PxControllerManager*	m_pControllerManager = { nullptr };

public:
	static CPhysics* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CPhysics* Clone(void* pArg);
	virtual void Free() override;
};

END