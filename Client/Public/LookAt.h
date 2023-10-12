#pragma once

/* =============================================== */
//	[CLookAt]
//	: 타겟을 향해 Look을 고정한다.
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CLookAt final : public CBehavior
{
private:
	explicit CLookAt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLookAt(const CLookAt& rhs);
	virtual ~CLookAt() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick_Behavior(const _float& fTimeDelta) override;

public:
	/* 
		1. 본인의 트랜스폼 포인터
		2. 회전 속도(배속, Default 1.f)
	*/
	void Set_Option(CTransform* pTransform, const _float& fTurnSpeed = 1.f) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
		m_fTurnSpeed = fTurnSpeed;
	}

private:
	_float m_fTurnSpeed = { 0.f };
	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CLookAt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLookAt* Clone(void* pArg) override;
	virtual void Free() override;
};

END