#pragma once
/* =============================================== */
//	[CArmored_Troll]
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
class CRenderer;
class CSequence;
class CSelector;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
class CRandom_Select;
class CWeapon_Armored_Troll;
END

BEGIN(Client)

class CArmored_Troll final : public CGameObject
{
private:
	explicit CArmored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CArmored_Troll(const CArmored_Troll& rhs);
	virtual ~CArmored_Troll() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	CWeapon_Armored_Troll* m_pWeapon = { nullptr };

private:
	const CGameObject* m_pTarget = { nullptr };
	_uint m_iCurrentSpell = { 0 };
	unordered_map<BUFF_TYPE, function<void(_float3, _float)>> m_CurrentTickSpells;

	_bool m_isSpawn = { false };
	_bool m_isParring = { false };
	_bool m_isRangeInEnemy = { false };
	_bool m_isChangeAnimation = { false };

	// 범위 안에 들어온 몬스터 리스트
	list<pair<wstring, const CGameObject*>> m_RangeInEnemies;

private:
	HRESULT Make_AI();
	HRESULT Make_Notifies();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:// 가까운 적을 타겟으로 세팅
	void Set_Current_Target();
	HRESULT Remove_GameObject(const wstring& wstrObjectTag);

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 행동 묶음들 */
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Turn_Run(_Inout_ CSequence* pSequence);

	HRESULT Make_Attack_Degree(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_BackHnd(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_ForHnd(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_45(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_90(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_135(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Left_180(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_45(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_90(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_135(_Inout_ CSequence* pSequence);
	HRESULT Make_Attack_Right_180(_Inout_ CSequence* pSequence);

	HRESULT Make_Pattern_Attack_Far(_Inout_ CRandom_Select* pRandom_Select);

	HRESULT Make_Pattern_Attack_Run(_Inout_ CSequence* pSequence);
	HRESULT Make_Pattern_Attack_Charge(_Inout_ CSelector* pSelector);

	HRESULT Make_Taunt_Degree(_Inout_ CSequence* pSequence);
	HRESULT Make_Check_Spell(_Inout_ CSelector* pSelector);

private: /* Notify Func */
	void Change_Animation() {
		m_isChangeAnimation = true;
	}

public:
	static CArmored_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END