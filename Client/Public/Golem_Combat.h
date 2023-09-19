#pragma once
/* =============================================== */
//	[CGolem_Combat]
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CSequence;
class CSelector;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
class CAction;
class CRandom_Select;
class CWeapon_Golem_Combat;
END

BEGIN(Client)

class CGolem_Combat final : public CGameObject
{
private:
	explicit CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGolem_Combat(const CGolem_Combat& rhs);
	virtual ~CGolem_Combat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	CWeapon_Golem_Combat* m_pWeapon = { nullptr };
	
private:
	const CGameObject* m_pTarget = { nullptr };
	_uint m_iCurrentSpell = { 0 };
	unordered_map<SPELL, function<void(_float3, _float)>> m_CurrentTickSpells;

	_bool m_isSpawn = { false };
	_bool m_isParring = { false };
	_bool m_isRangeInEnemy = { false };
	_bool m_isChangeAnimation = { false };

	// ���� �ȿ� ���� ���� ����Ʈ
	list<pair<wstring, const CGameObject*>> m_RangeInEnemies;

private:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	// ����� ���� Ÿ������ ����
	void Set_Current_Target();
	HRESULT Remove_GameObject(const wstring& wstrObjectTag);

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* �ൿ ������ */
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack(_Inout_ CSelector* pSelector);
	HRESULT Make_NormalAttack(_Inout_ CSelector* pSelector);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);
	HRESULT Make_Descendo(_Inout_ CSequence* pSequence);
	HRESULT Make_Random_Idle_Move(_Inout_ CRandom_Select* pRandomSelect);

public:
	static CGolem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END