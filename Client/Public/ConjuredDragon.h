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
class CCamera_Shake;
class CParticleSystem;
class CMeshEffect;
END

BEGIN(Client)
class CPulse;
class CAction;
class CBreath;
class CMagicSlot;
class CEnergyBall;
class CBreath_Effect;
class CWeapon_Dragon_Head;
class CWeapon_Dragon_Left_Wing;
class CWeapon_Dragon_Right_Wing;
class CWeapon_Dragon_Tail;
class CImpulseSphere_Effect;
class CBreath_Effect;
END

BEGIN(Client)

class CConjuredDragon final : public CEnemy
{
public:
	enum BONE_TYPE { LEFT_WING, RIGHT_WING, TAIL, BONE_TYPE_END };
	typedef struct tagDragonBoneData
	{
		const _float4x4* pCombinedTransformationMatrix = { nullptr };
		// ���� ��Ʈ�������� �� ����� ��ġ
		_float3 vPosition;
	}DRAGONBONEDATA;

private:
	explicit CConjuredDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConjuredDragon(const CConjuredDragon& rhs);
	virtual ~CConjuredDragon() = default;

public:
	_float Get_Current_HP_Percent() const {
		return m_pHealth->Get_Current_HP_Percent();
	}

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

#ifdef _DEBUG
public:
	void Tick_Imgui(_float fTimeDelta);
	
#endif // _DEBUG

private:
	_uint m_iLevel = { 0 };

private:
	/* ���� ������. 100 ���� ���� ������ �� */
	_float m_fInvincibleGauge = { 0.f };
	_bool m_isInvincible = { false };
	_bool m_isBreakInvincible = { false };
	_float3 m_vEmissiveStrength = { 0.f, 0.f, 0.f };
	_float m_fInvinRatio = { 1.f };

	void Update_Invincible(const _float& fTimeDelta);
	
private:
	void Play_Death_Color();
	void Death_Color(const _float& fTimeDelta);
	_bool m_isDeathColor = { false };
	_float m_fDeathColorRatio = { 0.f };
	_bool m_isIncreaseColor = { true };

private: /* ��ü ���� ���� ������ */
	CEnergyBall* m_pEnergyBall = { nullptr };
	_float m_fSpawnBallTimeAcc = { 0.f };

	void EnergyBall_PhaseOne(const _float& fTimeDelta);
	_bool Break_Invincible(const _float& fTimeDelta);

	/* ������ �һ������ ��ü ���� */
	_uint m_iDeathCount = { 0 };
	void EnergyBall_PhaseFinal(const _float& fTimeDelta);
	_bool UnSeal(const _float& fTimeDelta);

private: /* �극�� ���� ������ */
	CBreath* m_pBreath = { nullptr };
	const _float4x4* m_pHeadMatrix = { nullptr };
	_float3 m_vTargetPosition;

	void Update_Breath(const _float& fTimeDelta);

private: /* �޽� ���� ���� ������ */
	CPulse* m_pPulse = { nullptr };

private: /* ���ó�� ���� ������ */
	_bool m_isEnterDeath = { false };
	_float m_fDeadTimeAcc = { 0.f };
	/* ��� �� ���ư��� �Լ� */
	void DeathBehavior(const _float& fTimeDelta);

private:
	_bool m_isMoveLeft = { false };

private: /* ������ ���� �Լ� */
	_bool m_isPhaseOne = { false };
	_bool m_isPhaseTwo = { false };
	_bool m_isPhaseFinal = { false };
	_bool m_isFinish = { false };

	void Check_Phase();

private:
	CMagicSlot* m_pMagicSlot = { nullptr };
	CWeapon_Dragon_Head* m_pWeapon_Head = { nullptr };
	CWeapon_Dragon_Left_Wing* m_pWeapon_Left_Wing = { nullptr };
	CWeapon_Dragon_Right_Wing* m_pWeapon_Right_Wing = { nullptr };
	CWeapon_Dragon_Tail* m_pWeapon_Tail = { nullptr };

private:
	//ī�޶� ����ũ ��Ƽ���̿� �Լ��� �ֱ� ���� Ŭ����
	CCamera_Shake* m_pEnter_Shake = { nullptr };
	CCamera_Shake* m_pCamera_Shake_Hit_Terrain = { nullptr };
	CCamera_Shake* m_pStep_Shake = { nullptr };
	CCamera_Shake* m_pPulse_Shake = { nullptr };
	CCamera_Shake* m_pDeath_Shake = { nullptr };
	CCamera_Shake* m_pDeath_Shake2 = { nullptr };
	CCamera_Shake* m_pFallDown_Shake = { nullptr };
	

private:
	virtual HRESULT Add_Components_for_Shake();
	virtual HRESULT Make_Notifies_for_Shake();

private: // For. Effect
	_float3 m_vOffsetPos = { _float3() }; 
	_float3 m_vHeadPosition = { _float3() };
	CImpulseSphere_Effect* m_pEffect_ImpulseSphere = { nullptr };
	CBreath_Effect* m_pEffect_Breath = { nullptr };
	CParticleSystem* m_pEffect_BlackSmokeIdle = { nullptr };
	CParticleSystem* m_pEffect_Pulse_Charge = { nullptr };
	CParticleSystem* m_pEffect_Pulse_CircleEmit = { nullptr };
	CParticleSystem* m_pEffect_Pulse_CircleEmit_Distortion = { nullptr };
	CParticleSystem* m_pEffect_Pulse_Rock = { nullptr };
	CParticleSystem* m_pEffect_Pulse_SplashWater= { nullptr };
	CParticleSystem* m_pEffect_Pulse_BoomWispy = { nullptr };
	CParticleSystem* m_pEffect_DragonInvin = { nullptr };
	CParticleSystem* m_pEffect_InvinBreakDust = { nullptr };
	CParticleSystem* m_pEffect_DeathWhiteBall[5] = { nullptr };
	CMeshEffect* m_pEffect_DragonInvinMesh = { nullptr };
	CMeshEffect* m_pEffect_MeshEffect_DeathWhiteBall = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();
	HRESULT Add_Effects();

private: /* �ൿ ������ */
	HRESULT Make_NonSpawn(_Inout_ CRandomChoose* pRandomChoose);
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
	HRESULT Make_Ground_Attack_AOE(_Inout_ CSequence* pSequence);

	HRESULT Make_Ground_Charge(_Inout_ CSequence* pSequence);

	/* Air Patterns */
	HRESULT Make_Air_Break_Invincible(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Change_Invincible(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Attacks(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Attack_Fireball(_Inout_ CSequence* pSequence);
	HRESULT Make_Air_Idle_Breaks(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Air_Hover(_Inout_ CAction* pAction);
	
private: /* Notify Func */
	void Exit_Attack();
	void Enter_Left_Wing_Attack();
	void Enter_Left_Wing_Hit_Terrain();
	void Enter_Right_Wing_Attack();
	void Enter_Right_Wing_Hit_Terrain();
	void Enter_Tail_Attack();
	void Enter_Invin();
	void Exit_Invin();
	void Shot_Fireball_Black();
	void Shot_Fireball_White();
	void Enter_Charge_Attack();
	void On_Breath();
	void Off_Breath();
	void Action_Pulse();
	void Pulse_Charge();
	void Pulse_Stop_Charge();
	void Death_Enter();
	void Death_Exit();
	void Struggle();
	void Shake_FallDown();

public:
	static CConjuredDragon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CConjuredDragon* Clone(void* pArg) override;
	virtual void Free() override;
};

END