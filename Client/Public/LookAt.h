#pragma once

/* =============================================== */
//	[CLookAt]
//	: Ÿ���� ���� Look�� �����Ѵ�.
//	�� : �ּ�ȯ
//	�� :
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
		1. ������ Ʈ������ ������
		2. ȸ�� �ӵ�(���, Default 1.f)
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