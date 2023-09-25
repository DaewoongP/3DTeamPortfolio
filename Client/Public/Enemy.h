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
		_float fDamage = { 0.f };
	}COLLISIONREQUESTDESC;

protected:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

protected:
	CModel* m_pModelCom = { nullptr };
	CHealth* m_pHealth = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

protected:
	const CGameObject* m_pTarget = { nullptr };
	_uint m_iCurrentSpell = { 0 };
	_uint m_iPreviusSpell = { 0 };
	unordered_map<BUFF_TYPE, function<void(_float3, _float)>> m_CurrentTickSpells;

	_bool m_isSpawn = { false };
	_bool m_isParring = { false };
	_bool m_isHitCombo = { false };
	_bool m_isHitAttack = { false };
	_bool m_isRangeInEnemy = { false };
	_bool m_isChangeAnimation = { false };

	COLLISIONREQUESTDESC m_CollisionRequestDesc;

	// 범위 안에 들어온 오브젝트(적군) 리스트
	list<pair<wstring, const CGameObject*>> m_RangeInEnemies;

protected:
	virtual HRESULT Make_AI();
	virtual HRESULT Make_Notifies() = 0;
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT SetUp_ShaderResources();

protected:// 가까운 적을 타겟으로 세팅
	void Set_Current_Target();
	HRESULT Remove_GameObject(const wstring& wstrObjectTag);

protected: /* Notify Func */
	void Change_Animation() {
		m_isChangeAnimation = true;
	}

public:
	virtual void Free() override;
};

END