#pragma once
/* =============================================== */
//	[CGolem_Combat]
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
class CRigidBody;
class CRootBehavior;
END

BEGIN(Client)
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
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth() override;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CRootBehavior* m_pRootBehavior = { nullptr };

private:
	CWeapon_Golem_Combat* m_pWeapon = { nullptr };

private:
	HRESULT Make_AI();
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

#ifdef _DEBUG
	_int m_iIndex = { 0 };
	void Tick_ImGui();
#endif // _DEBUG

private: /* 행동 묶음들 */
	HRESULT Make_Descendo(_Inout_ CSequence* pSequence);
	HRESULT Make_Move(_Inout_ CSequence* pSequence);

public:
	static CGolem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END