#pragma once
/* =============================================== */
//	[CGoblin_Assasin]
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
class CWeapon_Goblin_Assasin;
END

BEGIN(Client)

class CGoblin_Assasin final : public CEnemy
{
private:
	explicit CGoblin_Assasin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGoblin_Assasin(const CGoblin_Assasin& rhs);
	virtual ~CGoblin_Assasin() = default;

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
	CWeapon_Goblin_Assasin* m_pWeapon = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private: /* ���ó�� ���� �Լ� */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* �ൿ ������ */
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_NormalAttack(_Inout_ CSelector* pSelector);

	HRESULT Make_Fly_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Descendo(_Inout_ CSequence* pSequence);
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

public:
	static CGoblin_Assasin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGoblin_Assasin* Clone(void* pArg) override;
	virtual void Free() override;
};

END