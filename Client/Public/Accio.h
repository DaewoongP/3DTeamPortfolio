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
class CAccio final : public CMagicBall
{
private:
	explicit CAccio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CAccio(const CAccio& rhs);
	virtual ~CAccio() = default;

public:
	//트레일 액션 돌리는데 필요한 함수임.
	typedef struct Accio_TickDesc {
		_float  fTimeDelta = { 0.02f };
		_float3 vPosition = {};
	}ACCIO_TICKDESC;
	//윙가를 돌리는 함수임. 안부른지 0.3초 이상 지나면 죽음.
	void TrailAction(void* pArg);

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
	// 몹 피격 트레일
	CWingardium_Effect* m_pWingardiumEffect = { nullptr };

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

public:
	static CAccio* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END