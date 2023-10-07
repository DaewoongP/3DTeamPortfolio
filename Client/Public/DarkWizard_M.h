#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Enemy.h"
#include "Magic.h"

BEGIN(Engine)
class CSequence;
class CSelector;
class CRandomChoose;
END

BEGIN(Client)
class CMagicBall;
class CMagicSlot;
class CWeapon_DarkWizard_Wand;
END

BEGIN(Client)

class CDarkWizard_M final : public CEnemy
{
private:
	explicit CDarkWizard_M(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDarkWizard_M(const CDarkWizard_M& rhs);
	virtual ~CDarkWizard_M() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

public:
	void Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const;

private:
	CMagicSlot* m_pMagicSlot = { nullptr };
	CWeapon_DarkWizard_Wand* m_pWeapon = { nullptr };

	CMagic::MAGICDESC m_MagicDesc;
	CMagicBall* m_CastingMagic = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private: /* ���ó�� ���� �Լ� */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* �ൿ ������ */
	HRESULT Make_Death(_Inout_ CSelector* pSelector);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Protego(_Inout_ CSequence* pSequence);
	HRESULT Make_NormalAttack(_Inout_ CSelector* pSelector);

	HRESULT Make_Fly_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Combat(_Inout_ CSelector* pSelector);

	/* Fly Combo */
	HRESULT Make_Air_Hit(_Inout_ CSequence* pSequence);
	HRESULT Make_Fly_Descendo(_Inout_ CSequence* pSequence);

private: /* Notify Functions */
	void Attack_Light();
	void Attack_Heavy();
	void Cast_Levioso();
	void Cast_Protego();
	void Shot_Magic();

public:
	static CDarkWizard_M* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END