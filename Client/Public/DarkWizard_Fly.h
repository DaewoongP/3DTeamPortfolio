#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

#include "Enemy.h"
#include "Magic.h"

BEGIN(Engine)
class CSequence;
class CSelector;
class CRandomChoose;
class CCamera_Shake;
END

BEGIN(Client)
class CAction;
class CMagicBall;
class CMagicSlot;
class CWeapon_DarkWizard_Wand;
END

BEGIN(Client)

class CDarkWizard_Fly final : public CEnemy
{
private:
	explicit CDarkWizard_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CDarkWizard_Fly(const CDarkWizard_Fly& rhs);
	virtual ~CDarkWizard_Fly() = default;

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
	CMagicSlot* m_pMagicSlot = { nullptr };
	CMagic::MAGICDESC m_MagicDesc;
	CMagicBall* m_CastingMagic = { nullptr };
	CWeapon_DarkWizard_Wand* m_pWeapon = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private: /* 사망처리 전용 함수 */
	_float m_fDeadTimeAcc = { 0.f };
	void DeathBehavior(const _float& fTimeDelta);

private: /* 행동 묶음들 */
	HRESULT Make_Death(_Inout_ CAction* pAction);
	HRESULT Make_Alive(_Inout_ CSelector* pSelector);

private: /* Notify Functions */
	void Attack_Light();
	void Attack_Heavy();
	void Shot_Magic();

private:
	static _uint iNumClass;
	static _float fAttackCoolTime; // 적군 마법사 무리의 공격 쿨타임

public:
	static CDarkWizard_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END