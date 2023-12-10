#pragma once

#include "Enemy.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CPulse final : public CGameObject
{
public:
	typedef struct tagPulseInitDesc
	{
		const CGameObject* pTarget = { nullptr };
		_float3 vPosition;
		_float fSpeed = { 0.f };
		_float fLifeTime = { 0.f };
	}PULSEINITDESC;

private:
	explicit CPulse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPulse(const CPulse& rhs);
	virtual ~CPulse() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset(const PULSEINITDESC& ResetDesc);

private:
	CEnemy::COLLISIONREQUESTDESC m_CollisionRequestDesc;
	const CGameObject* m_pTarget = { nullptr };
	_float m_fMoveSpeed = { 0.f };
	_float m_fLifeTime = { 0.f };
	_float m_fTimeAcc = { 0.f };
	_bool m_isEnable = { false };

private:
	CRigidBody* m_pRigidBody = { nullptr };
#ifdef _DEBUG
	CRenderer* m_pRenderer = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components();

public:
	static CPulse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END