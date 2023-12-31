#pragma once

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CWait final : public CBehavior
{
private:
	explicit CWait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWait(const CWait& rhs);
	virtual ~CWait() = default;

public:
	void Set_Timer(const _float& fTime) {
		m_fLimit = fTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta) override;

public:
	virtual void Reset_Behavior(HRESULT result) override;

private:
	_float m_fPreWorldTimeAcc = { 0.f };
	_float m_fLimit = { 0.f };

public:
	static CWait* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWait* Clone(void* pArg) override;
	virtual void Free() override;
};

END