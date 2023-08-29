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
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override { return S_OK; }
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	static CSequence* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence* Clone(void* pArg);
	virtual void Free() override;
};

END