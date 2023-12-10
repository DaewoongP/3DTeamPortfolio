
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CParticleSystem;
END


BEGIN(Client)

class CConfringo_WandDust_Effect final : public CGameObject
{
private:
	CConfringo_WandDust_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConfringo_WandDust_Effect(const CConfringo_WandDust_Effect& rhs);
	virtual ~CConfringo_WandDust_Effect() = default;

public:
	void Set_Position(_float3 vPos);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Play_Particle(_float3 vPos);

private: /* For. Component */
	CParticleSystem* m_pParticleDustExpand= { nullptr };
	CParticleSystem* m_pParticleDustConcentration = { nullptr };

private:
	CTransform* m_pExpandTransform = { nullptr };
	CTransform* m_pConcentrationTransform = { nullptr };
	
private:
	_uint m_iLevel = { 0 };

private:
	HRESULT Add_Components();

public:
	static CConfringo_WandDust_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

