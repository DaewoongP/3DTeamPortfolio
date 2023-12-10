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
#include "ConjuredDragon.h"

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
	void On_Collider_Attack(); 
	void Off_Collider_Attack(); 
	HRESULT Set_Bone_Data(CModel* pModel);
	void Enter_Hit_Terrain();

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
#ifdef _DEBUG
	virtual void Late_Tick(_float fTimeDelta) override;
#endif // _DEBUG

private:
	_uint m_iLevel = { 0 };
	vector<CConjuredDragon::DRAGONBONEDATA> m_Bones;
	CEnemy::COLLISIONREQUESTDESC m_CollisionRequestDesc;

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CParticleSystem* m_pEffect_WingAttack_TraceDarkCloud = { nullptr };
	CParticleSystem* m_pEffect_WingAttack_TraceRocks = { nullptr };
	CParticleSystem* m_pEffect_WingAttack_TraceDust = { nullptr };
#ifdef _DEBUG
	CRenderer* m_pRenderer = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components();

public:
	static CWeapon_Dragon_Right_Wing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CWeapon_Dragon_Right_Wing* Clone(void* pArg) override;
	virtual void Free() override;
};

END