#pragma once
/* =============================================== */
//	[CArmored_Troll]
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
class CParticleSystem;
END

BEGIN(Client)
class CWeapon_Armored_Troll;
END

BEGIN(Client)

class CArmored_Troll final : public CEnemy
{
private:
	explicit CArmored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CArmored_Troll(const CArmored_Troll& rhs);
	virtual ~CArmored_Troll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

private:
	CWeapon_Armored_Troll* m_pWeapon = { nullptr };

	//카메라 쉐이크 노티파이에 함수를 넣기 위한 클래스
	CCamera_Shake* m_pStep_Shake = { nullptr };
	CCamera_Shake* m_pHit_Shake = { nullptr };
	CCamera_Shake* m_pDeath_Shake = { nullptr };

private:
	virtual HRESULT Add_Components_for_Shake();
	virtual HRESULT Make_Notifies_for_Shake();

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private:
	_bool m_isOverheadAction = { false };
	array<CParticleSystem*, 5> m_DarkAura = { nullptr };
	array<const _float4x4*, 5> m_DarkAuraBoneMatrix = { nullptr };

private:
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음들 */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Intro(_Inout_ CSequence* pSequence);
	HRESULT Make_Flipendo(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Degree(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_Far(_Inout_ CSequence* pSequence);
	HRESULT Make_Taunt_Degree(_Inout_ CSequence* pSequence);

	HRESULT Make_Attack_Left_Front(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_45(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_90(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_180(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_Front(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_45(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_90(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_180(_Inout_ CSequence* pSequence);

	HRESULT Make_Pattern_Attack_BackHnd(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_ForHnd(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_Run(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_Charge(_Inout_ CSelector* pSelector);

	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Turn_Run(_Inout_ CSequence* pSequence);

private: /* Notify Functions */
	void Enter_Light_Attack();
	void Enter_Heavy_Attack();
	void Enter_Body_Attack();
	void Exit_Attack();
	void On_Overhead_Event() {
		m_isOverheadAction = true;
	}
	void Off_Overhead_Event() {
		m_isOverheadAction = false;
	}

public:
	static CArmored_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END