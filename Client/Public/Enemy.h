#pragma once
/* =============================================== */
//	[CEnemy]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Magic_Sound_Manager.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CHealth;
class CRenderer;
class CSelector;
class CTexture;
class CSequence;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
class CUI_Damage;
class CUI_Group_Enemy_HP;
class CWeapon_Armored_Troll;
END

BEGIN(Client)

class CEnemy abstract : public CGameObject
{
public:
	enum ATTACKTYPE { ATTACK_NONE, ATTACK_LIGHT, ATTACK_HEAVY, 
		ATTACK_BREAK, ATTACK_SUPERBREAK, ATTACKTYPE_END };

	typedef struct tagCollisionRequestDesc
	{
		ATTACKTYPE eType = { ATTACKTYPE_END };
		_uint iDamage = { 0 };
		CTransform* pEnemyTransform = { nullptr };
	}COLLISIONREQUESTDESC;

protected:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	CUI_Group_Enemy_HP* Get_UI_Enemy_HP() const {
		return m_pUI_HP;
	}
	const _float4x4* Get_HitBoneMatrix(const _uint& iIndex) {
		if (0 > iIndex || 3 <= iIndex)
			return nullptr;
		return m_HitMatrices[iIndex];
	}
	void Set_Parring() {
		m_isParring = true;
	}
	void Spawn() {
		m_isSpawn = true;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

public:
	virtual void Set_Protego_Collision(CTransform* pTransform, ATTACKTYPE eType) const {}
	void Ready_Hit_Finisher() {};
	const _bool Is_Spawn() const { return m_isSpawn; }
	void StopAllSound();

protected:
	CUI_Group_Enemy_HP* m_pUI_HP = { nullptr };
	CHealth* m_pHealth = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CTexture* m_pDissolveTexture = { nullptr };
	CUI_Damage* m_pUI_Damage = { nullptr };

protected:
	const CGameObject* m_pTarget = { nullptr };
	const CGameObject* m_pPlayer = { nullptr };
	mutable _uint m_iCurrentSpell = { 0 };
	_uint m_iPreviusSpell = { 0 };
	unordered_map<BUFF_TYPE, function<void(void*)>> m_CurrentTickSpells;
	unordered_map<BUFF_TYPE, MAIGBUFFTICKDESC*>		m_MagicTickDesc;
	CMagic_Sound_Manager::OWNERTYPE m_eOwnerType;

	_bool m_isSpawn = { false };
	_bool m_isParring = { false };
	_bool m_isHitCombo = { false };
	_bool m_isDissolve = { false };
	_bool m_isHitAttack = { false };
	_bool m_isRangeInEnemy = { false };
	_bool m_isChangeAnimation = { false };

	_float m_fDissolveAmount = { 0.f };
	_float3 m_vCurrentPosition = {};
	array<const _float4x4*, 3> m_HitMatrices;

	COLLISIONREQUESTDESC m_CollisionRequestDesc;

	// 범위 안에 들어온 오브젝트(적군)들
	unordered_map<wstring, const CGameObject*> m_RangeInEnemies;

protected:
	virtual HRESULT Add_Components_for_Shake() { return S_OK; }
	virtual HRESULT Make_Notifies_for_Shake() { return S_OK; }

protected:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

	HRESULT Make_Notifies() {}
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex) {}

protected:
	HRESULT Make_Hit_Combo(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Fly_Descendo(_Inout_ CSequence* pSequence);

protected:
	// 가까운 적을 타겟으로 세팅
	void Set_Current_Target();
	void Tick_Spells();
	void Print_Damage_Font(const _int& iDamage);

	HRESULT Remove_GameObject(const wstring& wstrObjectTag);
	_bool IsEnemy(const wstring& wstrObjectTag);
	_bool IsDebuff(const _uint& iType);
	_bool isFlying(const _uint& iType);
	_bool isCombo(const _uint& iType);
	_bool isControlSpell(const _uint& iType);
	_bool isPowerSpell(const _uint& iType);
	_bool isDamageSpell(const _uint& iType);

	void Remove_Fly_Spells();

protected: /* Notify Func */
	void Change_Animation() {
		m_isChangeAnimation = true;
	}
	void On_Gravity();
	void Off_Gravity();

public:
	virtual void Free() override;
};

END