#pragma once
/* =============================================== */
//	[CEnemy]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CHealth;
class CRenderer;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
class CUI_Group_Enemy_HP;
class CWeapon_Armored_Troll;
END

BEGIN(Client)

class CEnemy abstract : public CGameObject
{
public:
	enum ATTACKTYPE { ATTACK_NONE, ATTACK_LIGHT, ATTACK_HEAVY, ATTACKTYPE_END };
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

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

protected:
	CUI_Group_Enemy_HP* m_pUI_HP = { nullptr };
	CHealth* m_pHealth = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };

protected:
	const CGameObject* m_pTarget = { nullptr };
	const CGameObject* m_pPlayer = { nullptr };
	_uint m_iCurrentSpell = { 0 };
	unordered_map<BUFF_TYPE, function<void(void*)>> m_CurrentTickSpells;
	unordered_map<BUFF_TYPE, MAIGBUFFTICKDESC*>		m_MagicTickDesc;

	_bool m_isSpawn = { false };
	_bool m_isParring = { false };
	_bool m_isHitCombo = { false };
	_bool m_isHitAttack = { false };
	_bool m_isRangeInEnemy = { false };
	_bool m_isChangeAnimation = { false };

	_float3 m_vCurrentPosition = {};

	COLLISIONREQUESTDESC m_CollisionRequestDesc;

	// 범위 안에 들어온 오브젝트(적군) 리스트
	list<pair<wstring, const CGameObject*>> m_RangeInEnemies;

protected:
	virtual HRESULT Make_AI();
	virtual HRESULT Make_Notifies() = 0;
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Add_Components_Level(_uint iCurrentLevelIndex) = 0;
	virtual HRESULT SetUp_ShaderResources();
	virtual HRESULT SetUp_ShadowShaderResources();

protected:// 가까운 적을 타겟으로 세팅
	void Set_Current_Target();
	HRESULT Remove_GameObject(const wstring& wstrObjectTag);
	_bool IsEnemy(const wstring& wstrObjectTag);
	_bool IsDebuff(BUFF_TYPE eType);
	_bool isCombo(BUFF_TYPE eType);

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