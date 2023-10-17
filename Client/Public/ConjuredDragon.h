#pragma once
/* =============================================== */
//	[CConjuredDragon]
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
END

BEGIN(Client)
class CPulse;
class CAction;
class CBreath;
class CMagicSlot;
class CEnergyBall;
class CWeapon_Dragon_Head;
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
	/* 무적 게이지. 100 까지 차면 무적이 됨 */
	_float m_fInvincibleGauge = { 0.f };
	_bool m_isInvincible = { false };
	_bool m_isBreakInvincible = { false };

	void Update_Invincible(const _float& fTimeDelta);

private: /* 구체 패턴 관련 데이터 */
	CEnergyBall* m_pEnergyBall = { nullptr };
	_float m_fSpawnBallTimeAcc = { 0.f };

	void EnergyBall_PhaseOne(const _float& fTimeDelta);
	_bool Break_Invincible(const _float& fTimeDelta);

	/* 마지막 불사상태의 구체 패턴 */
	_uint m_iDeathCount = { 0 };
	void EnergyBall_PhaseFinal(const _float& fTimeDelta);
	_bool UnSeal(const _float& fTimeDelta);

private: /* 브레스 관련 데이터 */
	CBreath* m_pBreath = { nullptr };
	const _float4x4* m_pHeadMatrix = { nullptr };
	_float3 m_vTargetPosition;

	void Update_Breath(const _float& fTimeDelta);

private: /* 펄스 패턴 관련 데이터 */
	CPulse* m_pPulse = { nullptr };

private: /* 사망처리 전용 데이터 */
	_float m_fDeadTimeAcc = { 0.f };
	/* 사망 시 돌아가는 함수 */
	void DeathBehavior(const _float& fTimeDelta);

private:
	_bool m_isSettingBalance = { false };
	_bool m_isMoveLeft = { false };
	/* 무중력 상태에서 특정 방향으로 이동하지 못하게 만드는 함수 */
	void Check_Air_Balance(const _float& fTimeDelta);

private: /* 페이즈 관련 함수 */
	_bool m_isPhaseOne = { true };
	_bool m_isPhaseTwo = { false };
	_bool m_isPhaseFinal = { false };
	_bool m_isFinish = { false };

	void Check_Phase();

private:
	CMagicSlot* m_pMagicSlot = { nullptr };
	CWeapon_Dragon_Head* m_pWeapon = { nullptr };

private:
	_float3 vOffsetPos = { _float3() };
	CParticleSystem* m_pEffect_BlackSmokeIdle = { nullptr };
	CParticleSystem* m_pEffect_Pulse_Charge = { nullptr };
	CParticleSystem* m_pEffect_Pulse_CircleEmit = { nullptr };
	CParticleSystem* m_pEffect_Pulse_Rock = { nullptr };
	CParticleSystem* m_pEffect_Pulse_SplashWater= { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();
	HRESULT Add_Effects();

private: /* 행동 묶음들 */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);
	HRESULT Make_Final(_Inout_ CSelector* pSelector);

	HRESULT Make_Next_Phase(_Inout_ CSequence* pSequence);
	HRESULT Make_Start_Phase_Two(_Inout_ CSequence* pSequence);
	HRESULT Make_Ground_Pattern(_Inout_ CSelector* pSelector);
	HRESULT Make_Air_Pattern(_Inout_ CSelector* pSelector);
	HRESULT Make_Enter_Final(_Inout_ CSequence* pSequence);

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
	void Shot_Fireball_Black();
	void Shot_Fireball_White();
	void On_Breath();
	void Off_Breath();
	void Action_Pulse();
	void Pulse_Charge();
public:
	static CConjuredDragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CConjuredDragon* Clone(void* pArg) override;
	virtual void Free() override;
};

END