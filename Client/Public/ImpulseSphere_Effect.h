#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CVIBuffer;
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
	void Play(_float3 vPos);
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

private:
	_float4 m_vColor = { 1.f, 0.6f, 0.6f, 1.f };
	_bool m_isEnable = { false };
	_float m_fScale = { 0.f };
	_float m_fRimPower = { 0.34f };

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
