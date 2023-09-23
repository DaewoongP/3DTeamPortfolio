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
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	void Set_Transform(CTransform* pTransform) {
		m_pOwnerTransform = pTransform;
		Safe_AddRef(m_pOwnerTransform);
	}

private:
	CTransform* m_pOwnerTransform = { nullptr };

public:
	static CLookAt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLookAt* Clone(void* pArg) override;
	virtual void Free() override;
};

END