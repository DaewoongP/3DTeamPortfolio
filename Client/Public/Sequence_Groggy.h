#pragma once

/* =============================================== */
//	[CSequence_Groggy]
//	
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSequence_Groggy final : public CSequence
{
private:
	explicit CSequence_Groggy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Groggy(const CSequence_Groggy& rhs);
	virtual ~CSequence_Groggy() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta);

public:
	void Set_LoopTime(const _float& fTime) {
		m_fLoopTime = fTime;
	}

private:
	virtual HRESULT Assemble_Childs() override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

	_float m_fLoopTime = { 0.f };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_Groggy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Groggy* Clone(void* pArg) override;
	virtual void Free() override;
};

END