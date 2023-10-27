#pragma once

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer;
class CParticleSystem;
END

BEGIN(Client)
class CFireWorks : public CGameObject
{
	enum  FIRETYPE
	{
		TYPE_CIRCLE,TYPE_SPREAD,TYPE_END
	};

	typedef struct tagFire
	{
		_float4x4 WorldMatrix; // 상태 행렬
		FIRETYPE m_eType;
	}FireDesc;

private:
	explicit CFireWorks(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CFireWorks(const CFireWorks& rhs);
	virtual ~CFireWorks() = default;

public:
	HRESULT Initialize_Prototype(_uint iLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

	HRESULT Play(_float3 vPosition);

private:
	CShader* m_pShader = { nullptr };
	CShader* m_pShadowShader = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CModel* m_pModel = { nullptr };
	vector<CParticleSystem*> m_vecParticle;
	_float3 FirePosition = _float3(0.f, 0.f, 0.f);
	_uint m_iLevel = 0;
	_float m_fHeight = { 0.f };
	FIRETYPE m_eType;

private:
	HRESULT Add_Components();
	

public:
	static CFireWorks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END