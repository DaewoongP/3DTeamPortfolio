#pragma once

/* =============================================== */
// 
//	정 : 전대인
//	부 :
//
/* =============================================== */

#include "Component.h"
#include "VIBuffer_Point_Color_Instance.h"
#include "Modules.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem final : public CComponent
{
public:
	typedef CVIBuffer_Point_Color_Instance::COLORINSTANCE COL_P_INSTANCE;

public:
	friend class CParticleModule;

	enum MODULE { MAIN, EMITTER, SHAPE, VELOCITY_OVER_LIFETIME,
		LIMIT_VELOCITY_OVER_LIFETIME, INHERIT_VELOCITY, FORCE_OVER_LIFETIME,
		COLOR_OVER_LIFETIME, COLOR_BY_SPEED, SIZE_OVER_LIFETIME,
		SIZE_BY_SPEED, ROTATION_OVER_ROTATION, ROTATION_BY_SPEED,
		EXTERNAL_FORCES, NOISE, COLLISION, TRIGGERS,
		SUB_EMITTERS, TEXTURE_SHEET_ANIMATION, LIGHTS,
		TRAILS, CUSTOM_DATA, RENDERER, MODULE_END};

private:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	MAIN_MODULE* Get_MainModulePtr() {
		return &m_MainModuleDesc;
	}

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta, CVIBuffer_Point_Color_Instance* pBuffer);
	HRESULT Render();

public:
	void Play_Particle();
	void Stop_Particle();
	void Bind_ParticleValue(class CShader* pShader);

public:
	void Enable();
	void Disable();

public:
	_bool IsEnable() { return m_isEnable; }
	
public:
	void Save(const _tchar* _pDirectoyPath);
	void Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_DESC& _particleDesc);
	void Reset_AllParticle();
	void Dead_Particle(PARTICLE_DESC& _particleDesc);

private:
	void TimeDelta_Calculate(_float& fTimeDelta);
	void Play_On_Awake();
	void Vector_Size_Allocate(_uint iNumInstance);
	_bool Continue_Option(PARTICLE_DESC& _particleDesc);

private:
	void Action_By_LifteTime(PARTICLE_DESC& _particleDesc);
	void Action_By_Duration();
	void Action_By_StopOption();
	
public: // Modules 모든 모듈마다 get만들어주는거 너무 애바같아서..
	MAIN_MODULE m_MainModuleDesc;
	EMMISION_MODULE m_EmissionModuleDesc;
	SHAPE_MODULE	m_ShapeModuleDesc;

private:
	vector<PARTICLE_DESC>	 m_ParticleDescs;
	vector<COL_P_INSTANCE>  m_ParticleMatrices;
	_uint m_iActivatedParticleNum = { 0 };
	_uint m_iDeadCount = { 0 };
	_uint m_iNumInstance;
	_float m_fParticleSystemAge = { 0.f };
	_bool m_isEnable = { true };
	function<void()> m_StopAction;
public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END