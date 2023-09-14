
#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Default_Magic_Effect.h"

BEGIN(Engine)
class CTrail;
class CParticleSystem;
END


BEGIN(Client)

class CDefault_Magic_Effect final : public CGameObject
{
private:
	CDefault_Magic_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDefault_Magic_Effect(const CDefault_Magic_Effect& rhs);
	virtual ~CDefault_Magic_Effect() = default;

public:
	void Set_Position(_float3 vPos);

public:
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Play_Particle(_float3 vPos);
	void Play_ConeEmit(_float3 vPos);

	void Enable_Trail(_bool _isEnable);
	HRESULT Reset_Trail();
	_bool IsEnable() {
		_bool value = { false };
		if(m_pParticleSystem!=nullptr)
			value = m_pParticleSystem->IsEnable();
		return value;
	}
	void Enable_TraceParticle(_bool _isEnable);

private: /* For. Component */
	CTrail* m_pTrail = { nullptr };
	CParticleSystem* m_pParticleSystem = { nullptr };
	CParticleSystem* m_pTraceParticle = { nullptr };
	CParticleSystem* m_pConeEmitParticle = { nullptr };

private:
	CTransform* m_pParticleTransform = { nullptr };
	CTransform* m_pTraceParticleTransform = { nullptr };
	CTransform* m_pTrailTransform = { nullptr };
	CTransform* m_pConeEmitTransform = { nullptr };
	
private:
	_uint m_iLevel = { 0 };
	_float3 m_vPrevPos = { _float3() };

private:
	HRESULT Add_Components();

public:
	static CDefault_Magic_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

