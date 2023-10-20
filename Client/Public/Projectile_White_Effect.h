#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer;
class CShader;
class CMeshEffect;
class CParticleSystem;
class CTrail;
class CModel;
class CTexture;
END

BEGIN(Client)

class CProjectile_White_Effect : public CGameObject
{
private:
	explicit CProjectile_White_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CProjectile_White_Effect(const CProjectile_White_Effect& rhs);
	virtual ~CProjectile_White_Effect() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

#ifdef _DEBUG
public:
	void Tick_Imgui(_float fTimeDelta);
#endif // _DEBUG
private:
	CRenderer* m_pRenderer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };
	CTexture* m_pEmissiveTexture = { nullptr };
	
private:
	_uint m_iLevel = { 0 };
	_float3 m_vCamPos = { _float3() };

private:
	_bool m_isGlow = { false };
	_bool m_isBlur = { false };
	_float2 m_vTiling = { 1.f, 1.f };
	_float2 m_vOffset = { 0.f, 0.f };
	_float4 m_vColor = { 1.f, 1.f, 1.f, 1.f };
	_float m_fEmissiveStrength = { 1.3f };

private:
	HRESULT Add_Components();
	HRESULT Set_ShaderResources();

public:
	static CProjectile_White_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END
