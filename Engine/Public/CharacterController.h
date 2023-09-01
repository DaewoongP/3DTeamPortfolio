#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacterController final : public CComponent
{
public:
	_float3 Get_Position();
	void Set_Position(_float3 _vPosition);
	void Move(_float3 _vVelocity, _float _fTimeDelta, _float _fMinDist = 0.f);

private:
	explicit CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacterController(const CCharacterController& rhs);
	virtual ~CCharacterController() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;

private:
	PxController*			m_pController = { nullptr };

public:
	static CCharacterController* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END