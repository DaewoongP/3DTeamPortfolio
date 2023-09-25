#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CParticleSystem;
class CTrail;
END

BEGIN(Client)
class CFinisher final : public CMagicBall
{
private:
	explicit CFinisher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFinisher(const CFinisher& rhs);
	virtual ~CFinisher() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset() { return S_OK; }

private:
	//Lightning Streak
	CTrail* m_pTrail[3] = { nullptr };

	//Lightning Spark
	CParticleSystem* m_LightningSparkEffect_Green = { nullptr };
	CParticleSystem* m_LightningSparkEffect_Blue = { nullptr };
	CParticleSystem* m_LightningSparkEffect_Red = { nullptr };

	CParticleSystem* m_LineParticle = { nullptr };
	CParticleSystem* m_FlareCenterParticle = { nullptr };
	CParticleSystem* m_FlareSpreadParticle = { nullptr };
	CParticleSystem* m_DustParticle = { nullptr };

	//Wand Effect
	CParticleSystem* m_pWandLightningParticle = { nullptr };
	CTrail*			 m_pWandTrail = { nullptr };

private:
	// For. Lightining Trails
	_float3 m_vSplineLerpPostion[3][10] = {};
	_float3	m_vTargetPosition = {};
	_float3 m_vLighiningStartPosition = {};
	_float	m_fLerpAcc = { 0.f };

	_uint	m_iLevel = { 0 };

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
	static CFinisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel = 0);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END