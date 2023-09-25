#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"
#include "Wingardium_Effect.h"


// 윙 가르디움 레비오우사는 타겟을 설정하고 발동하는 마법입니다.
// 따라서 타겟이 없으면 안나갑니다.
// 윙 가르디움 레비오우사는 플레이어가 종료시키거나, 다른 마법을 적중시키기 전까지는 안죽습니다.

BEGIN(Client)
class CWingardiumleviosa final : public CMagicBall
{
private:
	explicit CWingardiumleviosa(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWingardiumleviosa(const CWingardiumleviosa& rhs);
	virtual ~CWingardiumleviosa() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Reset() { return S_OK; }

private:
	CWingardium_Effect* m_pEffect = { nullptr };
	CTransform* m_pEffectTrans = { nullptr };

private:
	_float3				m_vTargetPosition = {};
	_bool				m_bDeadTrigger = { false };

	_float				m_fSettingTimer = { 8.f };

private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CWingardiumleviosa* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END