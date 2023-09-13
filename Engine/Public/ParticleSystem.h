#pragma once

/* =============================================== */
// 
//	�� : ������
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CModel;
class CTransform;
END

BEGIN(Engine)

class ENGINE_DLL CParticleSystem : public CGameObject
{
public:
	typedef VTXCOLINSTANCE COL_INSTANCE;
	typedef list<PARTICLE>::iterator PARTICLE_IT;

public:
	// ALIVE : ������Ʈ �ǰ� �ִ� ��ƼŬ
	// DELAY : ���ð��� ä������ ALIVE�� �Ѿ
	// WAIT : ��ƼŬ Ǯ ���
	// DEAD : ��ƼŬ�� ��Ȱ �� �� ���� ����.
	enum STATE { ALIVE, DELAY, WAIT, DEAD, STATE_END };

protected:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint iLevel);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render();

public:
	void Play();
	void Stop();
	virtual void Restart();

protected:
	void Enable();
	void Disable();

public:
	_bool IsEnable() { return m_MainModuleDesc.isEnable; }
	_bool IsAllDead() { return m_Particles[ALIVE].empty(); }

protected:
	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_IT& _particle_iter);
	void Reset_Particles(STATE eGroup);
	void Reset_AllParticles();
	void ResetStartPosition(PARTICLE_IT& _particle_iter);
	void ResetVelocity(PARTICLE_IT& _particle_iter);

protected:
	// WAIT->DELAY�� �̵�.
	_bool Wating_One_Particle();

	// ������
	_float4x4 LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY = false);

protected:
	// ������ �� �ڵ���� �Ұ��� ������.
	void Play_On_Awake();

	// ��ƼŬ �ùķ��̼� �ӵ����
	void Simulation_Speed(_float& fTimeDelta);

	// �ν��Ͻ� ���� ���� resize, reserve�Լ��� �޸� �Ҵ�.
	void Resize_Container(_uint iNumInstance);

	// ���� �� �ð� ������ ó��.
	void Sum_TimeDelta(const _float& _fTimeDelta);

protected:
	void Action_By_Age();
	void Action_By_Duration();
	void Action_By_StopOption();
	void Action_By_RateOverTime();
	void Action_By_Distance();
	void Action_By_Bursts();
	void Action_By_RotationOverLifeTime(PARTICLE_IT& _particle_iter, _float fTimeDelta);
	void Action_By_ColorOverLifeTime(PARTICLE_IT& _particle_iter, _float fTimeDelta);
protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

protected:
	MAIN_MODULE						m_MainModuleDesc;
	EMISSION_MODULE					m_EmissionModuleDesc;
	SHAPE_MODULE					m_ShapeModuleDesc;
	RENDERER_MODULE					m_RendererModuleDesc;
	COLOR_OVER_LIFETIME				m_ColorOverLifeTimeModuleDesc;
	SIZE_OVER_LIFETIME				m_SizeOverLifeTimeModuleDesc;
	ROTATION_OVER_LIFETIME_MODULE	m_RotationOverLifetimeModuleDesc;

protected: 
	//CVIBuffer_Geometry* m_pShapeBuffer = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pMainTexture = { nullptr }; // ��¿� ��� �� �ؽ�ó
	CTexture* m_pClipTexture = { nullptr }; // �����׽�Ʈ�� ���� �ؽ�ó
	CVIBuffer_Rect_Color_Instance* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };

protected:
	list<PARTICLE> m_Particles[STATE_END];
	vector<COL_INSTANCE>  m_ParticleMatrices;
	function<void()> m_StopAction;
	_uint m_iLevel = { 0 };

public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
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