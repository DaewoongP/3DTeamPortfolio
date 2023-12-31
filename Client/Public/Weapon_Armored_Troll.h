#pragma once

/* =============================================== */
//	[CWeapon_Armored_Troll]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CWeapon_Armored_Troll : public CParts
{
private:
	explicit CWeapon_Armored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Armored_Troll(const CWeapon_Armored_Troll& rhs);
	virtual ~CWeapon_Armored_Troll() = default;

public:
	void Set_Render(_bool tf) { m_isRender = true; }

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
	void Off_Collider_Attack() {
		m_pRigidBody->Disable_Collision("Attack");
	}
	void On_Dissolve() { m_isDissolve = true; }

private:
	_bool m_isDissolve = { false };
	_bool m_isRender = { false };
	_float m_fDissolveAmount = { 0.f };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };
	CShader* m_pShadowShaderCom = { nullptr };
	CTexture* m_pDissolveTexture = { nullptr };
	CTexture* m_pEmissiveTexture = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Set_Shader_Resources();
	HRESULT SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix);

private:
	void Play_Random_Hit_Sounds();

public:
	static CWeapon_Armored_Troll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Armored_Troll* Clone(void* pArg) override;
	virtual void Free() override;
};

END