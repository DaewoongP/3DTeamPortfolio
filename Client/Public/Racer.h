#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidbody;
END

BEGIN(Client)
class CFlyGameManager;
END

BEGIN(Client)

class CRacer final : public CGameObject
{
private:
	explicit CRacer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CRacer(const CRacer& rhs);
	virtual ~CRacer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	HRESULT Add_Components();

private:
	CRigidBody*			m_pRigidBody = { nullptr };

public:
	static CRacer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END