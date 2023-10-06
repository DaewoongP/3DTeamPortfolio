#pragma once

/* =============================================== */
//	[CWeapon_Goblin_Assasin]
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
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CWeapon_Goblin_Assasin : public CParts
{
private:
	explicit CWeapon_Goblin_Assasin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Goblin_Assasin(const CWeapon_Goblin_Assasin& rhs);
	virtual ~CWeapon_Goblin_Assasin() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Render() override;

public:
	void On_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		m_pRigidBody->Enable_Collision("Attack", this, pCollisionRequestDesc);
	}
	void Off_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		m_pRigidBody->Disable_Collision("Attack");
	}

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Add_Components_Level(_uint iCurrentLevelIndex);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_Goblin_Assasin* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Goblin_Assasin* Clone(void* pArg) override;
	virtual void Free() override;
};

END