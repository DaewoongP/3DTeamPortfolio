#pragma once

/* =============================================== */
//	[CRandom_Attack]
//	
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "RandomChoose.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CAction;
END

BEGIN(Client)

class CRandom_Attack final : public CRandomChoose
{
private:
	explicit CRandom_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRandom_Attack(const CRandom_Attack& rhs);
	virtual ~CRandom_Attack() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	virtual HRESULT Assemble_Behavior(const wstring& BehaviorTag, CBehavior* pBehavior, const _float& fWeight) override;
	virtual void Reset_Behavior(HRESULT result) override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

public:
	static CRandom_Attack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CRandom_Attack* Clone(void* pArg) override;
	virtual void Free() override;
};

END