
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTrail;
class CParticleSystem;
END


BEGIN(Client)

class CConfringo_Explosive_Effect final : public CGameObject
{
private:
	CConfringo_Explosive_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConfringo_Explosive_Effect(const CConfringo_Explosive_Effect& rhs);
	virtual ~CConfringo_Explosive_Effect() = default;

public:
	void Set_Position(_float3 vPos);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Play_Particle(_float3 vPos);
	_bool Get_Particle_State_End();
	void	ResetParticle();

private: /* For. Component */
	CParticleSystem* m_pParticleSystem = { nullptr };
	CParticleSystem* m_pExplosiveParticle[3] = { nullptr };

private:
	CTransform* m_pParticleTransform = { nullptr };
	CTransform* m_pExplosiveTransform[3] = { nullptr };
	
private:
	_uint m_iLevel = { 0 };

private:
	HRESULT Add_Components();

public:
	static CConfringo_Explosive_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

