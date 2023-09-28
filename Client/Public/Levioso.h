#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Engine)
class CParticleSystem;
class CTrail;
END

BEGIN(Client)
class CWingardium_Effect;
END

BEGIN(Client)
class CLevioso final : public CMagicBall
{
private:
	explicit CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLevioso(const CLevioso& rhs);
	virtual ~CLevioso() = default;

public:
	//윙가를 돌리는 함수임. 안부른지 0.3초 이상 지나면 죽음.
	void TrailAction(_float3 vPosition, _float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset(MAGICBALLINITDESC& InitDesc) override;

private:
	// 트레일 
	CTrail*					  m_pMainTrail = { nullptr };
	// 트레일 따라가는 연기
	CParticleSystem*		  m_pTraceDustEffect = { nullptr };
	// 몹 피격 트레일
	CWingardium_Effect* m_pWingardiumEffect = { nullptr };

	// 피격 푸쉬시
	CParticleSystem*		  m_pHitDustEffect = { nullptr };
	// 피격 파바바바방
	CParticleSystem*		  m_pHitExplosionEffect = { nullptr };

	//완드 
	CTrail*					  m_pWandTrail = { nullptr };
	//완드 글로우
	CParticleSystem*		  m_pWandGlow = { nullptr };

private:
	_float				m_fWingardiumEffectDeadTimer = { 0.3f };

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
	static CLevioso* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END