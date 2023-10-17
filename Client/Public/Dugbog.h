#pragma once
/* =============================================== */
//	[CDugbog]
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
class CParticleSystem;
class CCamera_Shake;
END

BEGIN(Client)

class CDugbog final : public CEnemy
{
private:
	explicit CDugbog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDugbog(const CDugbog& rhs);
	virtual ~CDugbog() = default;

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
	_bool m_isAbleLevioso = { false };
	array<CParticleSystem*, 3> m_DarkAura = { nullptr };

	//ī�޶� ����ũ ��Ƽ���̿� �Լ��� �ֱ� ���� Ŭ����
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

private: /* ���ó�� ���� �Լ� */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* �ൿ ���� */
	HRESULT Make_Idle_Break(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Death(_Inout_ CSequence* pSequence);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Run_Attack(_Inout_ CSelector* pSelector);
	HRESULT Make_Tongue_Attack(_Inout_ CSequence* pSequence);
	HRESULT Make_Taunts(_Inout_ CSelector* pSelector);

	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Turn_Runs(_Inout_ CSequence* pSequence);
	HRESULT Make_Fly_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Levioso_Tongue(_Inout_ CSequence* pSequence);

	/* Fly Combo */
	HRESULT Make_Air_Hit(_Inout_ CSelector* pSelector);
	HRESULT Make_Fly_Descendo(_Inout_ CSequence* pSequence);

private: /* Notify Functions */
	void Enter_Light_Attack();
	void Enter_Heavy_Attack();
	void Enable_Levioso() {
		m_isAbleLevioso = true;
	}
	void Disable_Levioso() {
		m_isAbleLevioso = false;
	}
	void Exit_Attack();

public:
	static CDugbog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END