#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CLoadTrigger final : public CGameObject
{
private:
	explicit CLoadTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLoadTrigger(const CLoadTrigger& rhs);
	virtual ~CLoadTrigger() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	class CSeamless_Loader*			m_pLoader = { nullptr };
	CRenderer*						m_pRenderer = { nullptr };
	CRigidBody*						m_pRigidBody = { nullptr };

private:
	HRESULT Add_Components();
	void Loading();

public:
	static CLoadTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END