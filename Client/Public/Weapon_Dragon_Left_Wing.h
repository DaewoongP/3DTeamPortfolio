#pragma once

/* =============================================== */
//	[CWeapon_Dragon_Left_Wing]
// 
//	정 : 주성환
//	부 :
//
/* =============================================== */

#include "Parts.h"
#include "Enemy.h"
#include "Client_Defines.h"
#include "ConjuredDragon.h"

BEGIN(Engine)
class CRigidBody;
class CModel;
#ifdef _DEBUG
class CRenderer;
#endif // _DEBUG
END

BEGIN(Client)

class CWeapon_Dragon_Left_Wing : public CParts
{
private:
	explicit CWeapon_Dragon_Left_Wing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWeapon_Dragon_Left_Wing(const CWeapon_Dragon_Left_Wing& rhs);
	virtual ~CWeapon_Dragon_Left_Wing() = default;

public:
	void On_Collider_Attack(); 		
	void Off_Collider_Attack();
	HRESULT Set_Bone_Data(CModel* pModel);
	void Enter_Hit_Terrain();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
#ifdef _DEBUG
	virtual void Late_Tick(_float fTimeDelta) override;
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG

private:
	vector<CConjuredDragon::DRAGONBONEDATA> m_Bones;
	CEnemy::COLLISIONREQUESTDESC m_CollisionRequestDesc;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CParticleSystem* m_pEffect_WingAttack_TraceDarkCloud = { nullptr };
	CParticleSystem* m_pEffect_WingAttack_TraceRocks = { nullptr };

#ifdef _DEBUG
	CRenderer* m_pRenderer = { nullptr };
	int test = 0;
#endif // _DEBUG

private:
	HRESULT Add_Components();

public:
	static CWeapon_Dragon_Left_Wing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWeapon_Dragon_Left_Wing* Clone(void* pArg) override;
	virtual void Free() override;
};

END