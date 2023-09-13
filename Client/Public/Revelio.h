#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "MagicBall.h"

BEGIN(Client)
class CRevelio final : public CMagicBall
{
private:
	explicit CRevelio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRevelio(const CRevelio& rhs);
	virtual ~CRevelio() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionStay(COLLISIONDESC CollisionDesc) override;
	virtual void OnCollisionExit(COLLISIONDESC CollisionDesc) override;

private:
	HRESULT Add_Components();
	virtual HRESULT Add_Effect();

public:
	static CRevelio* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END