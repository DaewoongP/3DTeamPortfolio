#pragma once

#include "Client_Defines.h"
#include "Engine_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CMeshEffect;
class CParticleSystem;
class CTrail;
END


BEGIN(Client)
class CSantum_MapEffects : public CGameObject
{
private:
	typedef struct tagCMeshEffect
	{
		CMeshEffect* MeshEffect = { nullptr };
		_float fTimeAcc = { 0.f };
		_float fResetTime = { 5.f };
	}MapMeshEffect;

private:
	typedef struct tagParticleEffect
	{
		CParticleSystem* ParticleEffect = { nullptr };
		_float fTimeAcc = { 0.f };
		_float fResetTime = { 5.f };
	}MapParticleEffect;
private:
	explicit CSantum_MapEffects(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSantum_MapEffects(const CSantum_MapEffects& rhs);
	virtual ~CSantum_MapEffects() = default;

public:
	void Set_Phase2() { m_isPhase1 = false; }

public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	vector<MapMeshEffect> m_MapMeshEffects;
	vector< MapParticleEffect> m_MapParticleEffects;
	_bool m_isPhase1 = { true };

private:
	HRESULT Add_Components();

public:
	static CSantum_MapEffects* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free(void) override;

};
END
