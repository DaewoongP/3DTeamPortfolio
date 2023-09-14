
#pragma once

#include "GameObject.h"
#include "Engine_Defines.h"
#include "Tool_Defines.h"


BEGIN(Tool)

class CDummyTrail;
class CDummyParticle;
class CDummyMeshEffect;

END

BEGIN(Tool)
class CDummy_Effect final : public CGameObject
{
private:
	CDummy_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummy_Effect(const CDummy_Effect& rhs);
	virtual ~CDummy_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	void Tick_Imgui(_float fTimeDelta);

private: /* For. Component */
	CDummyTrail* m_pTrail = { nullptr };
	CDummyParticle* m_pParticleSystem = { nullptr };
	CDummyMeshEffect* m_pMeshEffect = { nullptr };

private:
	CTransform* m_pTrailTransform = { nullptr };
	CTransform* m_pParticleSystemTransform = { nullptr };
	CTransform* m_pMeshEffectTransform = { nullptr };

	_float3		m_vPrevPos = { _float3() };

	_float3 getEulerAnglesFromVector(const _float3& v)
	{
		_float3 euler;

		// Pitch
		euler.x = atan2(sqrt(v.y * v.y + v.z * v.z), v.x);

		// Yaw
		euler.y = atan2(v.z, v.x);

		// Roll (assuming it's 0 for now)
		euler.z = 0;

		return euler;
	}
private:
	HRESULT Add_Components();

public:
	static CDummy_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END

