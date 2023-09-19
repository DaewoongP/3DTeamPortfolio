#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

//기본 파티클
#include "Default_Magic_Effect.h"
//스플라인 트레일 파티클
#include "Default_MagicTraill_Effect.h"

BEGIN(Client)
class CBasicCast final : public CMagicBall
{
private:
	explicit CBasicCast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBasicCast(const CBasicCast& rhs);
	virtual ~CBasicCast() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;

private:
	CDefault_MagicTraill_Effect* m_pTrailEffect = { nullptr };
	CDefault_Magic_Effect*		 m_pEffect = { nullptr };
	CTransform* m_pEffectTrans = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_bool				m_bDeadTrigger = { false };
	_float				m_fSettingTimer = { 0.2f };
private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CBasicCast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END