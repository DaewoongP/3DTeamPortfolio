#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Client)
class CProjectile_White final : public CMagicBall
{
private:
	explicit CProjectile_White(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProjectile_White(const CProjectile_White& rhs);
	virtual ~CProjectile_White() = default;

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
	CMeshEffect* m_pMeshEffect_Projectile_Black = { nullptr };
	CMeshEffect* m_pMeshEffect_SplineUp[6] = { nullptr };

	_float4x4 m_InitMatrix_Projectile_Black;
	_float4x4 m_InitMatrix_SplineUp[6];

public:
	static CProjectile_White* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END