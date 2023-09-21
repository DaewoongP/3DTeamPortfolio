#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

//트레일
#include "Trail.h"
#include "ParticleSystem.h"

BEGIN(Engine)
class CParticleSystem;
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

private:
	CTrail* m_pTrail = { nullptr };

	//죄다 파티클로 바꾸기로 했음.
	CParticleSystem* m_pExplosiveEffect[2] = { nullptr };
	CParticleSystem* m_pWandDustEffect = { nullptr };

private:
	//For. Trail
	_float3 m_vStartPostion = {};
	_float3	m_vTargetPosition = {};
	_float	m_fLerpAcc = { 0.f };

	//For. Spline
	_float3 m_vSplineLerp[2] = {};
	_float  m_fTimeScalePerDitance = { 0.f };
	
	_bool	m_isExplosiveTrigger = { false };
	_uint	m_iLevel = { 0 };
private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CConfringo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel = 0);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END