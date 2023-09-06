#pragma once

/* =============================================== */
// 
//	�� : ������
//	�� :
//
/* =============================================== */

#include "Composite.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer;
class CShader;
class CModel;
class CTransform;
END

BEGIN(Engine)

class ENGINE_DLL CParticleSystem final : public CComposite
{
public:
	typedef CVIBuffer_Rect_Color_Instance::COLORINSTANCE COL_INSTANCE;
	typedef list<PARTICLE>::iterator PARTICLE_IT;

public:
	enum STATE { ALIVE, DELAY, WAIT, DEAD, STATE_END };

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
	void Tick(_float _fTimeDelta, CTransform* pTransform);
	HRESULT Render();

public:
	void Play();
	void Stop();
	void Restart();


public:
	void Enable();
	void Disable();

public:
	_bool IsEnable() { return m_MainModuleDesc.isEnable; }
	_bool IsAllDead() { return m_Particles[ALIVE].empty(); }

public:
	void Save(const _tchar* _pDirectoyPath);
	void Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_IT& _particle_iter);
	void Reset_Particles(STATE eGroup);
	void Reset_AllParticles();
	
public: // For. Imgui
	// ��ƼŬ �ؽ�ó ����.
	void ChangeMainTexture(const _tchar* pTexturePath);

	// �ν��Ͻ� ���� ����
	void RemakeBuffer(_uint iNumInstance);

private:
	// WAIT->DELAY�� �̵�.
	_bool Wating_One_Particle();

	// ����Ʈ ���� ���Ҹ� �ְ�޴� �Լ�.
	PARTICLE_IT TransitionTo(PARTICLE_IT& _particle_iter, list<PARTICLE>& _source, list<PARTICLE>& _dest);

	// ������
	_float4x4 LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY = false);

private:
	// ������ �� �ڵ���� �Ұ��� ������.
	void Play_On_Awake();

	// ��ƼŬ �ùķ��̼� �ӵ����
	void Simulation_Speed(_float& fTimeDelta);

	// �ν��Ͻ� ���� ���� resize, reserve�Լ��� �޸� �Ҵ�.
	void Resize_Container(_uint iNumInstance);

	// ���� �� �ð� ������ ó��.
	void Sum_TimeDelta(const _float& _fTimeDelta);

private:
	void Action_By_Age();
	void Action_By_Duration();
	void Action_By_StopOption();
	void Action_By_RateOverTime();
	void Action_By_Distance();
	void Action_By_Bursts();

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	MAIN_MODULE m_MainModuleDesc;
	EMMISION_MODULE m_EmissionModuleDesc;
	SHAPE_MODULE	m_ShapeModuleDesc;
	RENDERER_MODULE m_RendererModuleDesc;

private: 
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pTexture = { nullptr }; // ��¿� ��� �� �ؽ�ó
	CTexture* m_pClipTexture = { nullptr }; // �����׽�Ʈ�� ���� �ؽ�ó
	CVIBuffer_Rect_Color_Instance* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };
private:
	list<PARTICLE> m_Particles[STATE_END];
	vector<COL_INSTANCE>  m_ParticleMatrices;
	function<void()> m_StopAction;

public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath);
	virtual CComposite* Clone(void* _pArg) override;
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