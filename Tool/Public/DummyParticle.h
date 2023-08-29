#pragma once

#include "GameObject.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CRenderer;
class CShader;
class CTexture;
class CVIBuffer_Point_Instance;
class CParticleSystem;
END

BEGIN(Tool)
class DummyParticle : public CGameObject
{
private:
	typedef struct tagHitParticle
	{
		_bool        isAlive;
		_float4     vAccel;
		_float4     vVelocity;
		_float4x4    WorldMatrix;
		_double     dAge;
		_double     dLifeTime;
	}HITPARTICLE;

private:
	explicit DummyParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit DummyParticle(const DummyParticle& _rhs);
	virtual ~DummyParticle() = default;

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Tick(_float _fTimeDelta) override;
	virtual void Late_Tick(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	CShader* m_pShader = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CVIBuffer_Point_Instance* m_pBuffer = { nullptr };
	CParticleSystem* m_pParticle = { nullptr };

private:
	vector<HITPARTICLE>   m_Particles;
	_uint                 m_iNumParticles;

public:
	static DummyParticle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free(void) override;
};
END