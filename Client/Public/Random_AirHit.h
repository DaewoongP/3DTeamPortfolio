#pragma once

/* =============================================== */
//	[CRandom_AirHit]
//	
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRandom_AirHit final : public CRandomChoose
{
private:
	explicit CRandom_AirHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandom_AirHit(const CRandom_AirHit& rhs);
	virtual ~CRandom_AirHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CRandom_AirHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandom_AirHit* Clone(void* pArg) override;
	virtual void Free() override;
};

END