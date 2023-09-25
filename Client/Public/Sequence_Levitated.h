#pragma once

/* =============================================== */
//	[CSequence_Levitated]
//	: 객체의 공격관련 행동을 담당하는 클래스
//	정 : 주성환
//	부 : 
//
/* =============================================== */

#include "Sequence.h"
#include "Client_Defines.h"

BEGIN(Client)
class CRandom_Select;
END

BEGIN(Client)

class CSequence_Levitated final : public CSequence
{
private:
	explicit CSequence_Levitated(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSequence_Levitated(const CSequence_Levitated& rhs);
	virtual ~CSequence_Levitated() = default;

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Tick(const _float& fTimeDelta);

public:
	// Random_Select 클래스에 추가할 Action의 가중치 대입
	HRESULT Assemble_Random_Select_Behavior(const wstring& wstrActionTag, const _float& fWeight, const _float& fLoopTime);

private:
	virtual HRESULT Assemble_Childs() override;

private:
	CRandom_Select* m_pRandom_Levitated_Loop = { nullptr };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_Levitated* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Levitated* Clone(void* pArg) override;
	virtual void Free() override;
};

END