#pragma once

/* =============================================== */
//	[CRandom_Select]
//	
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Client)

class CRandom_Select final : public CRandomChoose
{
private:
	explicit CRandom_Select(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandom_Select(const CRandom_Select& rhs);
	virtual ~CRandom_Select() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	virtual void Reset_Behavior(HRESULT result) override;
	void Set_Option(const _float& fCoolTime) {
		m_fLimit = fCoolTime;
	}

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

public:
	static CRandom_Select* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandom_Select* Clone(void* pArg) override;
	virtual void Free() override;
};

END