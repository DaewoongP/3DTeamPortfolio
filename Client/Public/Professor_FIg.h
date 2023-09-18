#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CSelector;
class CSequence;
class CRenderer;
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
class CMagic;
END

BEGIN(Client)

class CProfessor_Fig final : public CGameObject
{
private:
	explicit CProfessor_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProfessor_Fig(const CProfessor_Fig& rhs);
	virtual ~CProfessor_Fig() = default;

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
	CMagic* m_pMagic = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	// 리스트 내에 들어있는 오브젝트 중 가장 가까운 객체 포인터
	const CGameObject* m_pTarget = { nullptr };
	// 범위 안에 들어온 몬스터 리스트
	list<pair<wstring, const CGameObject*>> m_RangeInEnemies;

	_bool m_isChangeAnimation = { false };
	_bool m_isRangeInEnemy = { false };

private:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	// 가까운 적을 타겟으로 세팅
	void Set_Current_Target();
	HRESULT Remove_GameObject(const wstring& wstrObjectTag);

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 행동 묶음들 */
	HRESULT Make_Turns(_Inout_ CSequence* pSequence);
	HRESULT Make_Non_Combat(_Inout_ CSelector* pSelector); // 비전투
	HRESULT Make_Combat(_Inout_ CSelector* pSelector); // 전투

	HRESULT Make_Attack_Combo1(_Inout_ CSequence* pSequence);

private: /* Notify Functions */
	void Change_Animation() {
		m_isChangeAnimation = true;
	}
	void Attack_Light();
	void Attack_Heavy();

public:
	static CProfessor_Fig* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END