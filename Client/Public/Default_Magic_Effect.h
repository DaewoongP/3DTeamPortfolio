
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
	virtual HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	void Play_Particle(_float3 vPos);
	void Set_Position(_float3 vPos);
	void Enable_Trail(_bool _isEnable);
	HRESULT Reset_Trail();
	_bool IsEnable() {
		_bool value = { false };
		if(m_pParticleSystem!=nullptr)
			value = m_pParticleSystem->IsEnable();
		return value;
	}

private: /* For. Component */
	CTrail* m_pTrail = { nullptr };
	CParticleSystem* m_pParticleSystem = { nullptr };

private:
	CTransform* m_pParticleTransform = { nullptr };
	CTransform* m_pTrailTransform = { nullptr };

private:
	_uint m_iLevel = { 0 };

private:
	HRESULT Add_Components();

public:
	static CDefault_Magic_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END

