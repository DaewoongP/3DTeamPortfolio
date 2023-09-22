#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

#include "Trail.h"
#include "ParticleSystem.h"

BEGIN(Engine)
class CParticleSystem;
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

private:
	//Lightning Streak
	CTrail* m_pTrail[3] = { nullptr };

private:
	// For. Lightining Trails
	_float3 m_vSplineLerpPostion[3][10] = {};
	_float3	m_vTargetPosition = {};
	_float3 m_vLighiningStartPosition = {};
	_float	m_fLerpAcc = { 0.f };

	_uint	m_iLevel = { 0 };
private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CFinisher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel = 0);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END