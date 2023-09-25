#pragma once

/* =============================================== */
//	[CDeath]
//	: Ÿ���� ���� Look�� �����Ѵ�.
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Behavior.h"
#include "Client_Defines.h"

BEGIN(Client)

class CDeath final : public CBehavior
{
private:
	explicit CDeath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDeath(const CDeath& rhs);
	virtual ~CDeath() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta) override;

public:
	/* Death �� ����Ǵ� ���� ������ �Լ������͸� ������ �˴ϴ�. */
	void Set_DeathFunction(function<void(const _float&)> Func) {
		m_DeathFunction = Func;
	}

private:
	function<void(const _float&)> m_DeathFunction = { nullptr };

public:
	static CDeath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CDeath* Clone(void* pArg) override;
	virtual void Free() override;
};

END