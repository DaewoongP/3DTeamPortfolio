#pragma once
/* =============================================== */
//	[CPensive]
// 
//	�� : ��ö��
//	�� :
//
/* =============================================== */

#include "Enemy.h"
#include "StateMachine_Enemy.h"

BEGIN(Engine)
class CTexture;
class CParticleSystem;
END

BEGIN(Client)
class CWeapon_Pensive;
class CStateContext_Enemy;
class CMagicSlot;
class CWeapon_Dragon_Head;
class CMagicBall;
class CVault_Torch;
END

BEGIN(Client)

class CPensive final : public CEnemy
{
public:
	enum PENSIVE_ATTACKTYPE { ATTACK_HAMMER, ATTACK_GROUND, ATTACK_SWORD, ATTACK_ORB, ATTACK_SCREAM, ATTACK_END };

private:
	explicit CPensive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPensive(const CPensive& rhs);
	virtual ~CPensive() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	HRESULT SetUp_ShaderResources();

public:
	virtual void Set_Protego_Collision(CTransform* pTransform, ATTACKTYPE eType) const;
	void		 Set_AttackAble_True() { m_isAttackAble = true; }
	void		 Set_AttackAble_False() { m_isAttackAble = false; }
	void		 Set_Turnable_True() { m_isTurnAble = true; m_isEmmisivePowerAdd = true; m_fEmissivePower = 0; }
	void		 Set_Turnable_False() { m_isTurnAble = false; m_isEmmisivePowerAdd = false; }
	void		 Set_EmmisivePower_Add() { m_isEmmisivePowerAdd = true; m_fEmissivePower = 0;}
	void		 Set_EmmisivePower_Minus() { m_isEmmisivePowerAdd = false; }
	void		 Do_Damage(_int iDmg);
	void ResetMagicBall() { 
		m_pMagicBall_Attack = nullptr;
		m_pMagicBall_Protego = nullptr;
		m_pMagicBall_Sword[0] = nullptr;
		m_pMagicBall_Sword[1] = nullptr;
		m_pMagicBall_Sword[2] = nullptr;
	};
	void DieMagicBall();
	void PensiveSwitchOn();
	void PensiveSwitchOff();

private:
	CStateContext_Enemy*	m_pStateContext = { nullptr };
	CMagicSlot*				m_pMagicSlot = { nullptr };
	CWeapon_Dragon_Head*	m_pDragonHead[3] = { nullptr };

	CMagicBall*				m_pMagicBall_Attack = { nullptr };
	CMagicBall*				m_pMagicBall_Protego = { nullptr };
	CMagicBall*				m_pMagicBall_Sword[3] = { nullptr };

	CTexture*				m_pEmissiveTexture_1 = { nullptr };
	CTexture*				m_pEmissiveTexture_2 = { nullptr };

	CParticleSystem*		m_pSkillDistotionParticle = { nullptr };

	vector<CVault_Torch*>			m_pTorch;

private:
	CStateMachine_Enemy::STATEMACHINEDESC m_StateMachineDesc = { CStateMachine_Enemy::STATEMACHINEDESC() };

	_uint m_iPhase = { 1 };
	_uint m_iSwordIndex = { 0 };
	_uint m_iAttackType = { ATTACK_END };
	_bool m_isAttackAble = { false };
	_bool m_isTurnAble = { false };
	_bool m_isEmmisivePowerAdd = { false };
	CMagic::MAGICDESC		m_ProtegoInitDesc[3] = {};
	_float4x4 m_SwordOffsetMatrix[3] = {};
	_float4x4 m_AttackPosition = {};
	_uint	  m_iGroogyStack = { 0 };

	_float	  m_fEmissivePower = { 0.f };

	_float4 m_vLightColor = {};
	CLight* m_pLight = { nullptr };
private:
	void	Attack_Ground();
	void	Attack_Orb();
	void	Attack_Shouting();
	void	Attack_Throw_Sword();
	void	Attack_Mace();

	void	Next_Attack();
	void	Next_SwordAttack();

private:
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Ready_StateMachine(_uint iCurrentLevelIndex);
	HRESULT Make_Notifies();
	HRESULT Add_Magic();

public:
	static CPensive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END