#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CParticleSystem;
END

BEGIN(Client)
class CProjectile_Black final : public CMagicBall
{
private:
	explicit CProjectile_Black(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProjectile_Black(const CProjectile_Black& rhs);
	virtual ~CProjectile_Black() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint m_iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc) override;

private:
	virtual void Ready_Begin() override;
	virtual void Ready_DrawMagic() override;
	virtual void Ready_CastMagic() override;
	virtual void Ready_Dying() override;

	virtual void Tick_Begin(_float fTimeDelta) override;
	virtual void Tick_DrawMagic(_float fTimeDelta) override;
	virtual void Tick_CastMagic(_float fTimeDelta) override;
	virtual void Tick_Dying(_float fTimeDelta) override;

private:
	HRESULT Add_Components();

private:
	CMeshEffect*		m_pMeshEffect_Outer_Ball = { nullptr };
	CMeshEffect*		m_pMeshEffect_Inner_Ball = { nullptr };
	CMeshEffect*		m_pMeshEffect_StartBoom = { nullptr };
	CMeshEffect*		m_pMeshEffect_EndBoom = { nullptr };
	CParticleSystem*	m_pParticle_Projectile_White_BulletTrace = { nullptr };

public:
	static CProjectile_Black* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END