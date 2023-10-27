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
	enum  FIRESTATE
	{
		TYPE_UP,TYPE_BURST,TYPE_READY,TYPE_END
	};

	typedef struct tagFire
	{
		_float4x4 WorldMatrix; // 상태 행렬
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
	
	vector<CParticleSystem*> m_vecParticle;
	_float3 FirePosition = _float3(0.f, 0.f, 0.f);
	_uint m_iLevel = 0;
	_float m_fHeight = { 0.f };
	FIRESTATE m_eType;
	_float m_fTimeAcc = { 0.f };
	_uint m_iRandTime = { 0 };
	_float m_fRandHeight = { 0.f };
private:
	HRESULT Add_Components();
	

public:
	static CFireWorks* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END