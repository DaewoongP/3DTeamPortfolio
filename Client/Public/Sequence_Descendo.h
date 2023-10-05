#pragma once

/* =============================================== */
//	[CSequence_Descendo]
//	
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSequence_Descendo final : public CSequence
{
private:
	explicit CSequence_Descendo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Descendo(const CSequence_Descendo& rhs);
	virtual ~CSequence_Descendo() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;

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
	static CSequence_Descendo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Descendo* Clone(void* pArg) override;
	virtual void Free() override;
};

END