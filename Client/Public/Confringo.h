#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

//스플라인 트레일 파티클
#include "Default_MagicTraill_Effect.h"
//펑 파티클
#include "Confringo_Explosive_Effect.h"
//푸시시시 파티클
#include "Confringo_WandDust_Effect.h"

BEGIN(Client)
class CConfringo final : public CMagicBall
{
private:
	explicit CConfringo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CConfringo(const CConfringo& rhs);
	virtual ~CConfringo() = default;

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
	CConfringo_Explosive_Effect* m_pExplosiveEffect = { nullptr };
	CConfringo_WandDust_Effect* m_pWandDustEffect = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_float3				m_vLerpWeight[2] = {};

	_float				m_fLerpAcc = { 0.f };
	_bool				m_isExplosiveTrigger = { false };
	_float				m_fTimeScalePerDistance = { 1.0f };

private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CConfringo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END