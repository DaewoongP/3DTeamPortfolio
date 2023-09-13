#pragma once

/* =============================================== */
//	[CTurn]
//	: 타겟을 향해 회전 한다.
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

class CTurn final : public CBehavior
{
private:
	explicit CTurn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTurn(const CTurn& rhs);
	virtual ~CTurn() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	void Set_Degree(const _float& _fDegree) {
		m_fDegree = _fDegree;
	}
	void Set_Transform(CTransform* pTransform) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
	}

private:
	_float m_fDegree = { 0.f };
	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CTurn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTurn* Clone(void* pArg) override;
	virtual void Free() override;
};

END