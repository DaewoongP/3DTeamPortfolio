#pragma once

/* =============================================== */
//	[CWeapon_Golem_Combat]
// 
//	�� : �ּ�ȯ
//	�� :
//
/* =============================================== */

#include "Parts.h"
#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CRigidBody;
class CTexture;
END

BEGIN(Client)

class CWeapon_Golem_Combat : public CParts
{
private:
	explicit CWeapon_Golem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Golem_Combat(const CWeapon_Golem_Combat& rhs);
	virtual ~CWeapon_Golem_Combat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix) override;

public:
	void On_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		m_pRigidBody->Enable_Collision("Attack", this, pCollisionRequestDesc);
	}
	void Off_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		m_pRigidBody->Disable_Collision("Attack");
	}
	void On_Dissolve() { m_isDissolve = true; }

private:
	_bool m_isDissolve = { false };
	_float m_fDissolveAmount = { 0.f };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CTexture* m_pDissolveTexture = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Set_Shader_Resources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

public:
	static CWeapon_Golem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Golem_Combat* Clone(void* pArg) override;
	virtual void Free() override;
};

END