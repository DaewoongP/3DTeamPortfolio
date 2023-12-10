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
	virtual HRESULT Initialize_Prototype(_uint iLevel) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionStay(COLLEVENTDESC CollisionEventDesc) override;
	virtual void OnCollisionExit(COLLEVENTDESC CollisionEventDesc) override;
	virtual HRESULT Reset() { return S_OK; }

private:
	HRESULT Add_Components();

public:
	static CRevelio* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END