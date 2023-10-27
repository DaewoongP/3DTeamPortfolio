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
class CBalloon;
class CMagicBall;
class CMagicSlot;
class CWeapon_DarkWizard_Wand;
class CBroom_Stick_DarkWizard;
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
	CBroom_Stick_DarkWizard* m_pBroom_Stick = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Make_Magics();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Bind_HitMatrices();

private:
	_bool m_isCollisionEnemy = { false };
	_float3 m_vCollisionTargetPosition;
	_float m_fBounceTimeAcc = { 0.f };

	CBalloon* m_pTargetBalloon = { nullptr };
	_float3 m_vMoveTargetPosition;

private: /* Çàµ¿ ¹­À½µé */
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Attacks(_Inout_ CRandomChoose* pRandomChoose);
	HRESULT Make_Move(_Inout_ CSequence* pSequence);

private:
	_bool is_Enemy(const wstring& wstrObjectTag);
	const CGameObject* Find_Enemy(const wstring& wstrObjectTag);
	void Update_Target();
	void Update_BalloonTarget_Near();
	void Update_BalloonTarget_Middle();
	void Update_BalloonTarget_Far();
	void Update_BalloonTarget_Player();
	void Move(const _float& fTimeDelta);
	void Bounce_Off(const _float& fTimeDelta);

private: /* Notify Functions */
	void Cast_Levioso();
	void Cast_Descendo();
	void Cast_Flipendo();
	void Cast_Arrestomomentum();
	void Shot_Magic();

private:
	_int m_iMy_Number = { 0 };
	static _int iNumClass;

public:
	static CDarkWizard_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END