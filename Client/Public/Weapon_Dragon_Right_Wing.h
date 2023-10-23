#pragma once

/* =============================================== */
//	[CWeapon_Dragon_Right_Wing]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
#ifdef _DEBUG
class CRenderer;
#endif // _DEBUG
END

BEGIN(Client)

class CWeapon_Dragon_Right_Wing : public CParts
{
private:
	explicit CWeapon_Dragon_Right_Wing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Dragon_Right_Wing(const CWeapon_Dragon_Right_Wing& rhs);
	virtual ~CWeapon_Dragon_Right_Wing() = default;

public:
	void On_Collider_Attack() {
		m_pRigidBody->Enable_Collision("Attack", this, &m_CollisionRequestDesc);
	}
	void Off_Collider_Attack() {
		m_pRigidBody->Disable_Collision("Attack");
	}

public:
	virtual HRESULT Initialize(void* pArg) override;
#ifdef _DEBUG
	virtual void Late_Tick(_float fTimeDelta) override;
#endif // _DEBUG

private:
	CEnemy::COLLISIONREQUESTDESC m_CollisionRequestDesc;

private:
	CRigidBody* m_pRigidBody = { nullptr };
#ifdef _DEBUG
	CRenderer* m_pRenderer = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components();

public:
	static CWeapon_Dragon_Right_Wing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Dragon_Right_Wing* Clone(void* pArg) override;
	virtual void Free() override;
};

END