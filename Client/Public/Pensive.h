#pragma once
/* =============================================== */
//	[CPensive]
// 
//	¡§ : æ»√∂πŒ
//	∫Œ :
//
/* =============================================== */

#include "Enemy.h"
#include "StateMachine_Enemy.h"


BEGIN(Client)
class CWeapon_Pensive;
class CStateContext_Enemy;
class CMagicSlot;
class CWeapon_Dragon_Head;
class CMagicBall;
END

BEGIN(Client)

class CPensive final : public CEnemy
{
public:
	enum PENSIVE_ATTACKTYPE {ATTACK_HAMMER, ATTACK_GROUND, ATTACK_SWORD, ATTACK_ORB, ATTACK_SCREAM,ATTACK_END};

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
	virtual HRESULT Render() override;

public:
	virtual void Set_Protego_Collision(CTransform* pTransform, ATTACKTYPE eType) const;

private:
	CStateContext_Enemy*	m_pStateContext = { nullptr };
	CMagicSlot*				m_pMagicSlot = { nullptr };
	CWeapon_Dragon_Head*	m_pDragonHead[3] = { nullptr };

	CMagic::MAGICDESC		m_ProtegoInitDesc[3] = {};

	CMagicBall*				m_pMagicBall_Fail = { nullptr };
	CMagicBall*				m_pMagicBall_Protego = { nullptr };

private:
	CStateMachine_Enemy::STATEMACHINEDESC m_StateMachineDesc = { CStateMachine_Enemy::STATEMACHINEDESC() };

	_uint m_iPhase = { 1 };
	_uint m_iAttackType = { ATTACK_END };

private:
	void	Attack_Ground();
	void	Attack_Orb();
	void	Next_Orb();

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