#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
class CMeshEffect;
#ifdef _DEBUG
class CRenderer;
#endif // _DEBUG
END

BEGIN(Client)

class CTrigger_Vault final : public CGameObject
{
private:
	explicit CTrigger_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CTrigger_Vault(const CTrigger_Vault& rhs);
	virtual ~CTrigger_Vault() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	CRigidBody*			m_pRigidBody = { nullptr };
	CMeshEffect*		m_pMeshEffect = { nullptr };

private:
	_float				m_fAccTime = { 0.f };
	_bool				m_isStartEffect = { false };

#ifdef _DEBUG
private:
	CRenderer*			m_pRenderer = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components();

public:
	static CTrigger_Vault* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END