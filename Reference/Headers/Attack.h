#pragma once

/* =============================================== */
//	[CAttack]
//	- 객체의 공격력을 나타내는 클래스
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CAttack : public CComponent
{
public:
	typedef struct tagAttackDesc
	{
		_int iDamage;
		_bool isIgnoreDeffence;
	}ATTACKDESC;

private:
	explicit CAttack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAttack(const CAttack& rhs);
	virtual ~CAttack() = default;

public:
	const _int& Get_Damage() const {
		return m_iDamage;
	}
	void Set_Damage(const _int & iDamage) {
		m_iDamage = iDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override { return S_OK; }
	virtual HRESULT Initialize(void* pArg) override;

private:
	_int m_iDamage = { 0 };

public:
	static CAttack* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CAttack* Clone(void* pArg);
	virtual void Free() override;
};

END