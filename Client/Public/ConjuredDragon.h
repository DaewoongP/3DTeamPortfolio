#pragma once
/* =============================================== */
//	[CConjuredDragon]
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CSequence;
class CSelector;
class CRandomChoose;
END

BEGIN(Client)
class CAction;
class CEnergyBall;
END

BEGIN(Client)

class CConjuredDragon final : public CEnemy
{
private:
	explicit CConjuredDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConjuredDragon(const CConjuredDragon& rhs);
	virtual ~CConjuredDragon() = default;

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
	/* ���� ������. 100 ���� ���� ������ �� */
	_float m_fInvincibleGauge = { 0.f };
	_bool m_isInvincible = { false };
	_bool m_isBreakInvincible = { false };
	void Update_Invincible(const _float& fTimeDelta);

private: /* ��ü ���� ���� �Լ� */
	CEnergyBall* m_pEnergyBall;
	_float m_fSpawnBallTimeAcc = { 0.f };
	void Spawn_EnergyBall(const _float& fTimeDelta);
	_bool Break_Invincible(const _float& fTimeDelta);

private: /* ���ó�� ���� �Լ� */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private:
	/* ���߷� ���¿��� Ư�� �������� �̵����� ���ϰ� ����� �Լ� */
	_bool m_isSettingBalance = { false };
	_bool m_isMoveLeft = { false };
	void Check_Air_Balance(const _float& fTimeDelta);

private: /* ������ ���� �Լ� */
	_bool m_isPhaseOne = { true };
	_bool m_isPhaseTwo = { false };
	void Check_Phase();

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private: /* �ൿ ������ */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Next_Phase(_Inout_ CSequence* pSequence);
	HRESULT Make_Start_Phase_Two(_Inout_ CSequence* pSequence);
	HRESULT Make_Ground_Pattern(_Inout_ CSelector* pSelector);
	HRESULT Make_Air_Pattern(_Inout_ CSelector* pSelector);

	/* Ground Patterns */
	HRESULT Make_Ground_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Ground_Attacks(_Inout_ CSequence* pSequence);

	HRESULT Make_Ground_Attacks_Melee(_Inout_ CSequence* pSequence);
	HRESULT Make_Ground_Attacks_Range(_Inout_ CSequence* pSequence);
	HRESULT Make_Ground_Attack_Pulse(_Inout_ CSequence* pSequence);

	HRESULT Make_Ground_Charge(_Inout_ CSequence* pSequence);

	/* Air Patterns */
	HRESULT Make_Air_Break_Invincible(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Change_Invincible(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Attacks(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Attack_Fireball(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Idle_Breaks(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Air_Hover(_Inout_ CAction* pAction);
	
private: /* Notify Func */
	void Shot_Fireball();

public:
	static CConjuredDragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CConjuredDragon* Clone(void* pArg) override;
	virtual void Free() override;
};

END