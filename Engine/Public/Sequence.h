#pragma once

/* =============================================== */
//	[CSequence]
//	- CBehavior 클래스 헤더의 설명 참고.
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Behavior.h"

BEGIN(Engine)

class ENGINE_DLL CSequence : public CBehavior
{
protected:
	explicit CSequence(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence(const CSequence& rhs);
	virtual ~CSequence() = default;

public:
	void Set_Option(const _float& fCoolTime) {
		m_fLimit = fCoolTime;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta) override;

public:
	virtual void Reset_Behavior(HRESULT result) override;

protected:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

protected:
	virtual HRESULT Assemble_Childs() override { return S_OK; }

public:
	static CSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence* Clone(void* pArg);
	virtual void Free() override;
};

END