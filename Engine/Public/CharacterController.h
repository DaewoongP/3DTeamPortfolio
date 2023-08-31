#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacterController final : public CComponent
{
private:
	explicit CCharacterController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CCharacterController(const CCharacterController& rhs);
	virtual ~CCharacterController() = default;

public:

public:
	static CCharacterController* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END