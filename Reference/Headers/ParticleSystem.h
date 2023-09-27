#pragma once

/* =============================================== */
// 
//	�� : ������
//	�� :
//
/* =============================================== */

#include "GameObject.h"
#include "VIBuffer_Rect_Color_Instance.h"
#include "VIBuffer_Color_Index_Instance.h"
#include "Modules.h"

BEGIN(Engine)
class CRenderer;
class CTexture;
class CShader;
class CModel;
class CTransform;
class CMotionBlurInstance;
class CVIBuffer_Rect_Particle_Instance;
END

BEGIN(Engine)

class ENGINE_DLL CParticleSystem : public CGameObject
{
public:
	// ALIVE : ������Ʈ �ǰ� �ִ� ��ƼŬ
	// DELAY : ���ð��� ä������ ALIVE�� �Ѿ
	// WAIT : ��ƼŬ Ǯ ���(����on�� ��� ��ƼŬ�� �׾��� �� ����� �´�.)
	// DEAD : ���� ��ƼŬ ��Ȱ �� �� ����.
	enum STATE { ALIVE, DELAY, WAIT, DEAD, STATE_END };

public:
	wstring Get_ParticleTag() { return m_szParticleTag; }
	void Set_ParticleTag(wstring szParticleTag) { m_szParticleTag = szParticleTag; }

protected:
	explicit CParticleSystem(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CParticleSystem(const CParticleSystem& _rhs);
	virtual ~CParticleSystem() = default;

public:
	_uint Get_DeadCount() { return m_Particles[DEAD].size(); }
	_bool Is_AllDead() { return (m_Particles[ALIVE].empty() && m_Particles[DELAY].empty()); }
	_bool IsEnable() { return m_MainModuleDesc.isEnable; }
	_bool Is_AliveEmpty() { return m_Particles[ALIVE].empty(); }
	void Add_Notify(PT_NOTIFY PTNotify);

	list<PARTICLE>::iterator Begin(STATE eState) { return m_Particles[eState].begin(); }
	list<PARTICLE>::iterator End(STATE eState) { return m_Particles[eState].end(); }		
	void PlayEvent(const PARTICLE_IT& Particle_iter);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* _pDirectoryPath, _uint iLevel);
	virtual HRESULT Initialize(void* _pArg) override;
	void Tick(_float _fTimeDelta);
	void Late_Tick(_float _fTimeDelta) override;
	HRESULT Render();

public: 
	// ex) MAIN_MODULE& MainModule = m_pParticleSystem->Get_MainModuleRef();
	MAIN_MODULE&					Get_MainModuleRef() { return m_MainModuleDesc; }
	EMISSION_MODULE&				Get_EmissionModuleRef() { return m_EmissionModuleDesc; }
	SHAPE_MODULE&					Get_ShapeModuleRef() { return m_ShapeModuleDesc; }
	RENDERER_MODULE&				Get_RendererModuleRef() { return m_RendererModuleDesc; }
	VELOCITY_OVER_LIFETIME& Get_VelocityOverLifetimeModuleRer() { return m_VelocityOverLifeTimeModuleDesc; }
	COLOR_OVER_LIFETIME&			Get_ColorOverLifetimeModuleRef() { return m_ColorOverLifeTimeModuleDesc; }
	SIZE_OVER_LIFETIME&				Get_SizeOverLifetimeModuleRef() { return m_SizeOverLifeTimeModuleDesc; }
	ROTATION_OVER_LIFETIME_MODULE&	Get_RotationOverLifetimeModuleRef() { return m_RotationOverLifetimeModuleDesc; }

	
public:
	void Play(_float3 vPosition = _float3());
	void Stop();
	virtual void Restart();
	// Stop vs Disable
	// Stop : ���ο� ��ƼŬ�� ������ ���´�.(Alive�� Delay�� ���� ��ƼŬ���� ������ ������ �����ϰ�����.)
	// Disable : ��ƼŬ ��ü�� ��Ȱ��ȭ ��Ų��.(Tick�� Late Tick�� ������ ����)
public:
	void Enable();
	void Enable(_float3 vPos);
	void Disable();

protected:
	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);

public:
	void Reset_Particle(PARTICLE_IT& _particle_iter);
	void Reset_AllParticles();
	void ResetStartPosition(PARTICLE_IT& _particle_iter);

protected:
	// WAIT->DELAY�� �̵�.
	PARTICLE_IT Wating_One_Particle();

	// ������
	_float4x4 LookAt(_float3 vPos, _float3 _vTarget, _bool _isDeleteY = false);

protected:
	// ������ �� �ڵ���� �Ұ��� ������.
	void Play_On_Awake();

	// ��ƼŬ �ùķ��̼� �ӵ����
	void Simulation_Speed(_float& fTimeDelta);

	// �ν��Ͻ� ���� ���� resize, reserve�Լ��� �޸� �Ҵ�.
	void Resize_Container(_uint iNumInstance);
	
protected:
	void Action_By_Age();
	void Action_By_Duration();
	void Action_By_StopOption();

protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

protected:
	MAIN_MODULE						m_MainModuleDesc;
	EMISSION_MODULE					m_EmissionModuleDesc;
	SHAPE_MODULE					m_ShapeModuleDesc;
	VELOCITY_OVER_LIFETIME			m_VelocityOverLifeTimeModuleDesc;
	COLOR_OVER_LIFETIME				m_ColorOverLifeTimeModuleDesc;
	SIZE_OVER_LIFETIME				m_SizeOverLifeTimeModuleDesc;
	ROTATION_OVER_LIFETIME_MODULE	m_RotationOverLifetimeModuleDesc;
	TEXTURE_SHEET_ANIMATION			m_TextureSheetAnimationModuleDesc;
	NOISE_MODULE					m_NoiseModuleDesc;
	RENDERER_MODULE					m_RendererModuleDesc;

protected: 
	//CVIBuffer_Geometry* m_pShapeBuffer = { nullptr };
	CRenderer* m_pRenderer = { nullptr };
	CTexture* m_pMainTexture = { nullptr }; // ��¿� ��� �� �ؽ�ó
	CTexture* m_pClipTexture = { nullptr }; // �����׽�Ʈ�� ���� �ؽ�ó
	CTexture* m_pGradientTexture = { nullptr }; // �����׽�Ʈ�� ���� �ؽ�ó
	CTexture* m_pNormalTexture = { nullptr }; // �ؽ�ó��Ʈ�� ���� �ؽ�ó
	CVIBuffer_Rect_Particle_Instance* m_pBuffer = { nullptr };
	CShader* m_pShader = { nullptr };
	CModel* m_pModel = { nullptr };
	//CMotionBlurInstance* m_pMotionBlurInstance = { nullptr };

protected:
	list<PARTICLE> m_Particles[STATE_END];
	vector<VTXPARTICLEINSTANCE>  m_ParticleMatrices;
	function<void()> m_StopAction;
	_uint m_iLevel = { 0 };
	_bool m_isStop = { false };
	wstring m_szParticleTag = { TEXT("") };

public:
	static CParticleSystem* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pDirectoryPath, _uint iLevel = 0);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

	friend struct EMISSION_MODULE;
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