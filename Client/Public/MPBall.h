#pragma once
#include "GameObject.h"
#include"Client_Defines.h"

BEGIN(Client)
class CMPBall :
    public CGameObject
{
private:
	explicit CMPBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMPBall(const CMPBall& rhs);
	virtual ~CMPBall() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint m_iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;


private:
	HRESULT Add_Components();
	virtual HRESULT Add_RigidBody();

private:
	CRigidBody* m_pRigidBody = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTransform* m_pTransform = { nullptr };

private:
	COLLISIONFLAG			m_eCollisionFlag;
	_uint m_iLevel = { 0 };


public:
	static CMPBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};
END
