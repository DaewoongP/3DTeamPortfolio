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

BEGIN(Engine)
class CModel;
class CTransform;
END

BEGIN(Client)
class CAction;
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
	void Set_LoopTime(const _float& fTime) {
		m_fLoopTime = fTime;
	}

private:
	virtual HRESULT Assemble_Childs() override;

private:
	_float m_fLimit = { 0.f };
	_float m_fPreWorldTimeAcc = { 0.f };

	_float m_fLoopTime = { 0.f };

private:
	virtual void Reset_Behavior(HRESULT result) override;

public:
	static CSequence_Levitated* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSequence_Levitated* Clone(void* pArg) override;
	virtual void Free() override;
};

END