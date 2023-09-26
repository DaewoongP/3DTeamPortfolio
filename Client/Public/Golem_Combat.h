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
END

BEGIN(Client)
class CRandom_Select;
class CWeapon_Golem_Combat;
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
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	CWeapon_Golem_Combat* m_pWeapon = { nullptr };

private:
	virtual HRESULT Make_AI() override;
	virtual HRESULT Make_Notifies() override;
	virtual HRESULT Add_Components() override;
	virtual HRESULT SetUp_ShaderResources() override;

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 사망처리 전용 함수 */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음들 */
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack(_Inout_ CSelector* pSelector);
	HRESULT Make_NormalAttack(_Inout_ CSelector* pSelector);
	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Descendo(_Inout_ CSequence* pSequence);
	HRESULT Make_Levioso_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Air_Hit(_Inout_ CSequence* pSequence);
	HRESULT Make_Random_Idle_Move(_Inout_ CRandom_Select* pRandomSelect);
	HRESULT Make_Death(_Inout_ CSequence* pSequence);

private: /* Notify Func */
	void Enter_Light_Attack();
	void Enter_Heavy_Attack();
	void Enter_Body_Attack();
	void Exit_Attack();

public:
	static CGolem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGolem_Combat* Clone(void* pArg) override;
	virtual void Free() override;
};

END