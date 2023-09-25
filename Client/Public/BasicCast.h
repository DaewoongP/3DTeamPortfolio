#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

//스플라인 트레일 파티클
#include "Default_MagicTrail_Effect.h"
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
	virtual HRESULT Reset() { return S_OK; }

private:
	// 지팡이 트레일
	CTrail*						 m_WandTrail = { nullptr };
	//지팡이 움직임 이펙트
	CParticleSystem*			m_pWandEffect = { nullptr };

	// 기본공격 트레일
	CDefault_MagicTrail_Effect* m_pTrailEffect = { nullptr };
	//피격 이펙트
	CParticleSystem* m_pHitEffect = { nullptr };
	
	//지팡이 3타 터지는 이펙트
	CParticleSystem* m_pFinalAttackEffect = { nullptr };
private:
	_float3				m_vTargetPosition = {};
	_uint				m_iLevel = {};

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
	static CBasicCast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END