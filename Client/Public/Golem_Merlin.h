#pragma once
/* =============================================== */
//	[CGolem_Merlin]
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
class CAction;
class CWeapon_Golem_Merlin;
END

BEGIN(Client)

class CGolem_Merlin final : public CGameObject
{
private:
	explicit CGolem_Merlin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CGolem_Merlin(const CGolem_Merlin& rhs);
	virtual ~CGolem_Merlin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	CWeapon_Golem_Merlin* m_pWeapon = { nullptr };

private:
	_uint m_iCurrentSpell = { 0 };
	_uint m_iPreviousSpell = { 0 };

	_bool m_isParring = { false };
	_bool m_isSpawn = { false };

private:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 행동 묶음들 */

public:
	static CGolem_Merlin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END