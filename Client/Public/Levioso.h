#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"
//삥삥 도는 트레일 하나
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
	//윙가를 돌리는 함수임. 안부른지 0.3초 이상 지나면 죽음.
	void TrailAction(_float3 vPosition, _float fTimeDelta);

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

private:
	_float3				m_vTargetPosition = {};

	_float				m_fDeadTimer = { 1.0f };
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
	static CLevioso* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END