#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CVIBuffer;
class CParticleSystem;
END
BEGIN(Client)


class CImpulseSphere_Effect : public CGameObject
{
private:
	explicit CImpulseSphere_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CImpulseSphere_Effect(const CImpulseSphere_Effect& rhs);
	virtual ~CImpulseSphere_Effect() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Play(_float3 vPos, _float fTerrainYPos = -34.f);
	void Stop();
	void Reset();

#ifdef _DEBUG
public:
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG

public:
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CVIBuffer* m_pVIBuffer = { nullptr };
	CCoolTime* m_pCoolTime = { nullptr };
	CTexture* m_pNoiseTexture = { nullptr };
	
private:
	CParticleSystem* m_pPulse_Rock_Particle = { nullptr };
	CParticleSystem* m_pPulse_SplashWater_Particle = { nullptr };
	CParticleSystem* m_pPulse_SplashWater2_Particle = { nullptr };
	CParticleSystem* m_pPulse_GlowSphere_Particle = { nullptr };

private:
	_float m_fCircleOffset = { 2.f };
	_float4 m_vColor = { 1.f, 0.376f, 0.376f, 1.f };
	_bool m_isEnable = { false };
	_float m_fScale = { 0.f };
	_float m_fFresnelPower = { 0.84f };
	_float m_fEmissivePower = { 0.6f };
	_float m_fRatio = { 11.1f };
	_float m_fTerrainYPos;
	_float m_fHeight = { 0.f };
	_float m_fCircleRadius = { 0.f };
	_bool m_isTouchGround = { false };
	_bool m_isStart = { false };

private:
	_uint m_iLevel = { 0 };

private:
	HRESULT Add_Components();
	HRESULT Set_ShaderResources();

public:
	static CImpulseSphere_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
