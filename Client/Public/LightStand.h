#pragma once
#include "MapObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRigidBody;
class CParticleSystem;
END

BEGIN(Client)

// 불 붙는 화로 클래스
class CLightStand final : public CMapObject
{
private:
	explicit CLightStand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CLightStand(const CLightStand& rhs);
	virtual ~CLightStand() = default;

public:
	_bool Get_FireOn() const { return m_isFireOn; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize_Level(_uint iCurrentLevelIndex) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual void OnCollisionEnter(COLLEVENTDESC CollisionEventDesc) override;

private:
	CParticleSystem* m_pEffect = { nullptr };
	CRigidBody* m_pRigidBody = { nullptr };

private:
	_bool	m_isFireOn = { false };

private:
	void	LightOn();

public:
	static CLightStand* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END