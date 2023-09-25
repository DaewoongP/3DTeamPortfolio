#pragma once

/* =============================================== */
//	[CSequence_AirHit]
//	: 객체의 공중피격관련 행동을 담당하는 클래스
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)
class CRandom_AirHit;
END

BEGIN(Client)

class CSequence_AirHit final : public CSequence
{
private:
	explicit CSequence_AirHit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_AirHit(const CSequence_AirHit& rhs);
	virtual ~CSequence_AirHit() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	/* 기본 약공 시 랜덤으로 돌려줄 공중 피격 CAction 추가 */
	HRESULT Assemble_Behavior(const _float& fWeight);

private:
	CRandom_AirHit* m_pRandom_AirHit = { nullptr };

private:
	virtual HRESULT Assemble_Childs() override;

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_AirHit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_AirHit* Clone(void* pArg) override;
	virtual void Free() override;
};

END