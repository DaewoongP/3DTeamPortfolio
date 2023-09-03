#pragma once

/* =============================================== */
// 
//	정 : 전대인
//	부 :
//
/* =============================================== */

#include "Component.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "Modules.h"

BEGIN(Engine)

class ENGINE_DLL CParticleSystem final : public CComponent
{
public:
	typedef CVIBuffer_Rect_Color_Instance::COLORINSTANCE COL_INSTANCE;
	typedef list<PARTICLE>::iterator PARTICLE_IT;

public:
	enum STATE { ALIVE, WAIT, DEAD, STATE_END };

private:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	MAIN_MODULE* Get_MainModulePtr() {
		return &m_MainModuleDesc;
	}

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta, CVIBuffer_Rect_Color_Instance* pBuffer);
	HRESULT Render();

public:
	void Play_Particle();
	void Stop_Particle();
	HRESULT Bind_ParticleValue(class CShader* pShader);

public:
	void Enable();
	void Disable();

public:
	_bool IsEnable() { return m_isEnable; }
	_bool IsAllDead() { return m_Particles[ALIVE].empty(); }

public:
	void Save(const _tchar* _pDirectoyPath);
	void Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_IT& _particle_iter);
	void Reset_Particles(STATE eGroup);
	void Reset_AllParticles();
	
public: // For. Imgui
	void ReviveAll();
	void ChangeMainTexture(const _tchar* pTexturePath);
	void RemakeBuffer(_uint iNumInstance);

private:
	_bool Wating_One_Particle();
	PARTICLE_IT TransitionTo(PARTICLE_IT& _particle_iter, list<PARTICLE>& _source, list<PARTICLE>& _dest);
	PARTICLE_IT Alive_Particle(PARTICLE_IT& _particle_iter);

	_float4x4 LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY = false);

private:
	void Play_On_Awake();
	void TimeDelta_Calculate(_float& fTimeDelta);
	void Resize_Container(_uint iNumInstance);

private:
	void Action_By_LifteTime();
	void Action_By_Duration();
	void Action_By_StopOption();
	
public:
	MAIN_MODULE m_MainModuleDesc;
	EMMISION_MODULE m_EmissionModuleDesc;
	SHAPE_MODULE	m_ShapeModuleDesc;
	RENDERER_MODULE m_RendererModuleDesc;

private: // 출력에 사용 될 텍스처
	CTexture* m_pTexture = { nullptr };
	CTexture* m_pClipTexture = { nullptr };

private:
	list<PARTICLE> m_Particles[STATE_END];

	vector<COL_INSTANCE>  m_ParticleMatrices;
	_uint m_iActivatedParticleNum = { 0 };
	_float m_fParticleSystemAge = { 0.f };
	_bool m_isEnable = { true };
	function<void()> m_StopAction;

public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END





//enum MODULE {
//	MAIN, EMITTER, SHAPE, VELOCITY_OVER_LIFETIME,
//	LIMIT_VELOCITY_OVER_LIFETIME, INHERIT_VELOCITY, FORCE_OVER_LIFETIME,
//	COLOR_OVER_LIFETIME, COLOR_BY_SPEED, SIZE_OVER_LIFETIME,
//	SIZE_BY_SPEED, ROTATION_OVER_ROTATION, ROTATION_BY_SPEED,
//	EXTERNAL_FORCES, NOISE, COLLISION, TRIGGERS,
//	SUB_EMITTERS, TEXTURE_SHEET_ANIMATION, LIGHTS,
//	TRAILS, CUSTOM_DATA, RENDERER, MODULE_END
//};