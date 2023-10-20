#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CVIBuffer;
class CParticleSystem;
class CMeshEffect;
END

BEGIN(Client)
class CBreath_Effect : public CGameObject
{
private:
	explicit CBreath_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBreath_Effect(const CBreath_Effect& rhs);
	virtual ~CBreath_Effect() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	_bool is_Null(const _float3* pStartPos, _float3* pTargetPos);

public:
	void Play(const _float3* pStartPos, _float3* pTargetPos, _float fTerrainYPos);
	void Stop();
	void Reset();

#ifdef _DEBUG
public:
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG

private:
	//CMeshEffect*	 m_pMeshEffect_Breath = { nullptr };
	CParticleSystem* m_pParticle_Breath_RedFire = { nullptr };
	CParticleSystem* m_pParticle_Breath_WhiteFire = { nullptr };
	CParticleSystem* m_pParticle_Breath_RedBurn = { nullptr };
	CParticleSystem* m_pParticle_Breath_WhiteBurn = { nullptr };
	CParticleSystem* m_pParticle_Breath_Vomit = { nullptr };
	CParticleSystem* m_pParticle_Breath_Distortion = { nullptr };
	class CCoolTime* m_pAttachedTime = { nullptr };
	class CCoolTime* m_pStopTime = { nullptr };

private:
	_uint m_iLevel = { 0 };
	_bool m_isEnable = { false };
	const _float3* m_pStartPos = { nullptr };
	_float3* m_pTargetPos = { nullptr };
	_float m_fTerrainYPos = { 0.f };
	_bool m_isBurnStart = { false };
	_bool m_isBurnStop = { false };

private:
	HRESULT Add_Components();

public:
	static CBreath_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};
END
