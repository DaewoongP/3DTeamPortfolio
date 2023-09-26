#pragma once

/* =============================================== */
//	[CWeapon_Golem_Combat]
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

class CWeapon_Golem_Combat : public CParts
{
private:
	explicit CWeapon_Golem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Golem_Combat(const CWeapon_Golem_Combat& rhs);
	virtual ~CWeapon_Golem_Combat() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void On_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		Set_CollisionData(pCollisionRequestDesc);
		m_pRigidBody->Enable_Collision("Attack", this);
	}
	void Off_Collider_Attack(CEnemy::COLLISIONREQUESTDESC* pCollisionRequestDesc) {
		Set_CollisionData(pCollisionRequestDesc);
		m_pRigidBody->Disable_Collision("Attack");
	}
private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	HRESULT Add_Components(void* pArg);
	HRESULT Set_Shader_Resources();

public:
	static CWeapon_Golem_Combat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Golem_Combat* Clone(void* pArg) override;
	virtual void Free() override;
};

END