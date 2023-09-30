#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CParticleSystem;
class CTrail;
END

BEGIN(Client)
class CLumos final : public CMagicBall
{
private:
	explicit CLumos(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLumos(const CLumos& rhs);
	virtual ~CLumos() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel)override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc) override;

public:
	void	Lumos_Tick(_float3 vPos, _float fTimeDelta);

private:
	// ±Û·Î¿ì ÀÌÆåÆ®
	CParticleSystem*			m_pWandGlowEffect = { nullptr };
	// ±Û·Î¿ì »¡°­ ÀÌÆåÆ®
	CParticleSystem*			m_pWandGlowRedEffect= { nullptr };
private:
	_float				m_fEndTimer = { 0.3f };

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
	virtual HRESULT Add_Effect();

public:
	static CLumos* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END