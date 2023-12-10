#pragma once

/* =============================================== */
//	[CHealth]
//	- 객체의 체력을 나타내는 클래스
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CHealth final : public CComponent
{
public:
	typedef struct tagHealthDesc
	{
		_int iMaxHP;
	}HEALTHDESC;

private:
	explicit CHealth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHealth(const CHealth& rhs);
	virtual ~CHealth() = default;

public:
	/* 0.f ~ 1.f 사이의 퍼센트를 반환 */
	_float Get_Current_HP_Percent() const {
		return _float(m_iHP) / _float(m_iMaxHP);
	}
	_float Get_Current_HP() const {
		return _float(m_iHP);
	}
	const _int& Get_MaxHP() const {
		return m_iMaxHP;
	}

	void Set_HP(const _int& iHP) {
		m_iHP = iHP;
	}
	void Set_Max_Hp(const _int & iMaxHP) {
		m_iMaxHP = iMaxHP;
	}
	_bool isDead() const {
		return m_iHP <= 0 ? true : false;
	}
	const _int& Get_HP() const { return m_iHP; }

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Damaged(const _int & iValue) {
		Clamp(m_iHP -= iValue, 0, m_iMaxHP);
	}
	void Heal(const _int & iValue) {
		Clamp(m_iHP += iValue, 0, m_iMaxHP);
	}

	// 예를들어, MaxHP가 100이고, 인자로 0.2f를 입력하면 20의 체력이 회복된다.
	void Heal(_float fPercent) {
		Clamp(fPercent, 0.f, 1.f); // 범위를 [0, 1]로 제한.
		_int iHealAmount = static_cast<_int>(m_iMaxHP * fPercent);
		Heal(iHealAmount);
	}

private:
	_int m_iMaxHP = { 0 };
	_int m_iHP = { 0 };

public:
	static CHealth* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CHealth* Clone(void* pArg);
	virtual void Free() override;
};

END