#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"
//»æ»æ µµ´Â Æ®·¹ÀÏ ÇÏ³ª
#include "Default_MagicTraill_Effect.h"
#include "Wingardium_Effect.h"

BEGIN(Client)
class CLevioso final : public CMagicBall
{
private:
	explicit CLevioso(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLevioso(const CLevioso& rhs);
	virtual ~CLevioso() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;

private:
	CDefault_MagicTraill_Effect* m_pEffect = { nullptr };
	CWingardium_Effect* m_pWingardiumEffect = { nullptr };
	CTransform* m_pWingardiumEffectTrans = { nullptr };
	CTransform* m_pEffectTrans = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_float				m_fLerpAcc = { 0.f };
	_bool				m_bWingardiumActionTrigger = { false };

	_float				m_fSettingTimer = { 0.2f };
	_float				m_fTimerForSearch = { 0.f };
private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CLevioso* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END