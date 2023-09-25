#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CParticleSystem;
class CTrail;
END

BEGIN(Client)
class CConfringo final : public CMagicBall
{
private:
	explicit CConfringo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConfringo(const CConfringo& rhs);
	virtual ~CConfringo() = default;

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
	// 불 트레일 
	CTrail* m_pMainTrail = { nullptr };
	// 불 트레일 파티클
	CParticleSystem* m_pTrailDustEffect = { nullptr };
	
	//종료 이펙트
	CParticleSystem* m_pExplosiveEffect[2] = { nullptr };
	//종료 큰 불똥
	CParticleSystem* m_pExplosiveBigPartEffect = { nullptr };
	//종료 작은 불똥
	CParticleSystem* m_pExplosiveSmallPartEffect = { nullptr };

	// 완드 불
	CParticleSystem* m_pWandTouchEffect = { nullptr };
	// 완드 먼지
	CParticleSystem* m_pWandDustEffect = { nullptr };
	// 완드 트윙클
	CParticleSystem* m_pWandTwinklEffect = { nullptr };
	// 완드 트레일
	CTrail*			 m_pWandTrail = { nullptr };
private:
	//For. Trail
	_float3 m_vStartPostion = {};
	_float3	m_vTargetPosition = {};
	_float	m_fLerpAcc = { 0.f };

	//For. Spline
	_float3 m_vSplineLerp[2] = {};
	_float  m_fTimeScalePerDitance = { 0.f };
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
	static CConfringo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel = 0);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END