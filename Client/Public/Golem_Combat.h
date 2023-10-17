#pragma once
/* =============================================== */
//	[CGolem_Combat]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CSequence;
class CSelector;
class CRandomChoose;
class CCamera_Shake;
END

BEGIN(Client)
class CWeapon_Golem_Combat;
class CUI_Damage;
END

BEGIN(Client)

class CGolem_Combat final : public CEnemy
{
private:
	explicit CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGolem_Combat(const CGolem_Combat& rhs);
	virtual ~CGolem_Combat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	CWeapon_Golem_Combat* m_pWeapon = { nullptr };
	
	//카메라 쉐이크 노티파이에 함수를 넣기 위한 클래스
	CCamera_Shake* m_pDescendo_Shake = { nullptr };

private:
	virtual HRESULT Add_Components_for_Shake();
	virtual HRESULT Make_Notifies_for_Shake();

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private: /* 사망처리 전용 함수 */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음들 */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_NormalAttack(_Inout_ CSelector* pSelector);

	HRESULT Make_Fly_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack(_Inout_ CSelector* pSelector);
	HRESULT Make_Random_Idle_Move(_Inout_ CRandomChoose* pRandomChoose);

	/* Fly Combo */
	HRESULT Make_Air_Hit(_Inout_ CSequence* pSequence);
	HRESULT Make_Fly_Descendo(_Inout_ CSequence* pSequence);

private: /* Notify Func */
	void Enter_Light_Attack();
	void Enter_Heavy_Attack();
	void Enter_Body_Attack();
	void Exit_Attack();

private:
	CUI_Damage* m_pUI_Damage = { nullptr };

public:
	static CGolem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGolem_Combat* Clone(void* pArg) override;
	virtual void Free() override;
};

END