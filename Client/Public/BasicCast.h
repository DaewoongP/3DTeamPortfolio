#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

//스플라인 트레일 파티클
#include "Default_MagicTraill_Effect.h"
//파티클
#include "ParticleSystem.h"

BEGIN(Client)
class CBasicCast final : public CMagicBall
{
private:
	explicit CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBasicCast(const CBasicCast& rhs);
	virtual ~CBasicCast() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint m_iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;

private:
	// 트레일을 만드는 친구.
	CDefault_MagicTraill_Effect* m_pTrailEffect = { nullptr };
	CParticleSystem*			m_pTraceEffect = { nullptr };
	//피격 이펙트
	CParticleSystem* m_pHitEffect = { nullptr };
	//지팡이 움직임 이펙트
	CParticleSystem* m_pWandEffect = { nullptr };
	//지팡이 3타 터지는 이펙트
	CParticleSystem* m_pFinalAttackEffect = { nullptr };
private:
	_float3				m_vTargetPosition = {};
	_bool				m_bDeadTrigger = { false };
	_float				m_fSettingTimer = { 0.2f };
	_uint				m_iLevel = {};
private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CBasicCast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END