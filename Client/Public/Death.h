#pragma once

/* =============================================== */
//	[CDeath]
//	: 타겟을 향해 Look을 고정한다.
//	정 : 주성환
//	부 :
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
	/* Death 가 진행되는 동안 돌려줄 함수포인터를 넣으면 됩니다. */
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