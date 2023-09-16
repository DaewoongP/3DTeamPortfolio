#pragma once

/* =============================================== */
//	[CSequence]
//	- CBehavior Ŭ���� ����� ���� ����.
// 
//	�� : �ּ�ȯ
//	�� :
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
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	virtual void Reset_Behavior(HRESULT result) {
		(*m_iterCurBehavior)->Reset_Behavior(result);
		m_iterCurBehavior = m_Behaviors.begin();
	}

protected:
	virtual HRESULT Assemble_Childs() override { return S_OK; }

public:
	static CSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence* Clone(void* pArg);
	virtual void Free() override;
};

END