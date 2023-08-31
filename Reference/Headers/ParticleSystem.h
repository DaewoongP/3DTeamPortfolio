#pragma once

#include "Component.h"
#include "VIBuffer_Point_Color_Instance.h"
BEGIN(Engine)

// ��ü : ��� ��ƼŬ�� ������ ParticleSystem�� �ν��Ͻ��� ����.
// ��ƼŬ : �� ���ڵ��� �ǹ���.
struct ENGINE_DLL MAIN_MODULE
{
	// �ϴ� �ֱ�� �ߴµ� ������ٵ� ����ϴ°Ŷ� ����.
	enum EMMITER_VELOCITY { RIGIDBODY, TRANSFORM, EV_END };

	// Stop �ɼ�
	enum STOP_OPTION {
		SO_NONE // �ƹ��� �ɼ��� ����
		, SO_DISABLE // ��ƼŬ ��Ȱ��ȭ
		, SO_DESTROY // ��ü �Ҹ�
		, SO_CALLBACK // �Լ� ȣ��(�Լ��� ���ٽ��� ���������)
		, SO_END };

	_float fDuration = { 5.0f }; // ��ü�� ����
	_bool isLooping = { false }; // ��ƼŬ�� �ݺ�
	_bool isPrewarm = { false }; // LoopȰ��ȭ �� ��� ����, Stop <-> Play ��ȯ �� ��ƼŬ�� �ʱ�ȭ ���� ����.
	_float2 fStartDelay = { 0.f, 0.f }; // �ʱ�ȭ �� ��ƼŬ ���� �ð��� ����
	_float2 fStartLifeTime = { 5.f, 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� ������ ����
	_float2 fStartSpeed = { 5.f, 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �ӵ�
	_bool is3DStartSize = { false }; // x,y,z�� ȸ���� Ȱ��ȭ�Ϸ��� true.
	_float2 fStartSize = { 1.f, 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ ũ��.
	_bool is3DStartRotation = { false }; // �ʱ�ȭ �� �������� ��ƼŬ 3���� ȸ����(���� x,y,z�� ����)
	_float2 fStartRotation = { 0.f, 0.f }; // �ʱ�ȭ �� �������� ��ƼŬ ���Ϸ���(������ ����� x,y�� ����)
	_float fFlipRotation = { 0.f }; // [0, 1]������ �������, 0.5�� ���� ��� ��ƼŬ �ʱ�ȭ �� 50%Ȯ���� �ݴ�������� ȸ���Ѵ�.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �÷�.
	_float fGravityModifier = { 0.f }; // ��ƼŬ�� ���� �Ǵ� �߷°�
	_float fSimulationSpeed = { 1.f }; // ��ƼŬ ��� �ӵ�
	_bool isPlayOnAwake = { true }; // ��ü�� �������ڸ��� ��ƼŬ�� ����Ұ��� ����.
	EMMITER_VELOCITY eEmmiterVelocity = { RIGIDBODY }; //  Inherit Velocity ���� Emission ��⿡ ���Ǵµ� �ӵ� ���� �� �����.
	_int iMaxParticles = { 1000 }; // �� ���� ������ �� �ִ� ��ƼŬ�� ���� ������.(�ν��Ͻ� ���� 100�̿��� 10���� �����ϸ� 10���� ����)
	_bool isAutoRandomSeed = { true }; // ��ƼŬ ���� �ֱ⸶�� ���� ���� �Ź� �ٲ���ϴ� �뵵.
	STOP_OPTION eStopAction = { SO_NONE }; // ��ü ������ ���ϰų� ��ƼŬ�� ��� ����� �Ϸ���� �� �ɼǿ� ���� �ൿ�� �޶�����.
};

struct ENGINE_DLL EMMISION_MODULE
{
	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // ��� �� ���� �ð�
		_int iCount = { 30 }; // �Ѳ����� ��� ��ƼŬ�� ������ ����
		_int iCycles = { 1 }; // �� ���� Interval�� �� �� ��� �Ұ��� ����.
		_float fInterval = { 0.010f }; // �ʱ�ȭ ����
	}BURST;

	_int	iRateOverTime = { 10 }; // 1�� ���� �� ���� ��ƼŬ�� �վ�� ����.
	_int	iRateOverDistance = { 0 }; // ������ �Ÿ��� ���� �� ���� ��ƼŬ�� �վ�� ����.
	vector<BURST> Bursts;
};

struct ENGINE_DLL SHAPE_MODULE
{
	enum SHAPE {SPHERE, HEMISPHERE, CONE, RECTANGLE, MESH, SHAPE_END };

	typedef struct tagSphereParameter
	{
		_float fAngle;
		_float fRadius;
		_float fRadiusThickness;
		_float fArc;
		_float4x4 ShapeMatrix;
		_bool AlignToDirection = { false };
	}SPHERE_PARAM;

	SPHERE_PARAM eSphereParam;
};

typedef struct tagParticleDesc
{
	_float		fAge = { 0.f };
	_bool       isAlive = { true };
	_float4     vAccel = _float4();
	_float4     vVelocity = _float4();
	_float4x4   WorldMatrix = _float4x4();
	_float      fLifeTime = { 0.f };
	_float4		vColor = { 1.f, 1.f, 1.f, 1.f };
	_float3		vScale = { 1.f, 1.f, 1.f };
}PARTICLE_DESC;

class CParticleModule;

class ENGINE_DLL CParticleSystem final : public CComponent
{
public:
	typedef CVIBuffer_Point_Color_Instance::COLORINSTANCE COL_P_INSTANCE;

public:
	friend class CParticleModule;

	enum MODULE { VELOCITY_OVER_LIFETIME,
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

	CParticleModule* Get_ModulePtr(MODULE _eModule) const {
		return m_Modules[_eModule];
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
	// NULL�� �ƴ� ��� ����� ������ ���Ϸ� ����.
	void Save(const _tchar* _pDirectoyPath);

	// ������ �����ϸ� �ε��Ѵ�.(���� ����� ������ ���ؼ� ������ ���� �� ����.)
	void Load(const _tchar* _pDirectoyPath);

	void Reset_Particle(PARTICLE_DESC& _particleDesc);
	void Reset_AllParticle();
	void Dead_Particle(PARTICLE_DESC& _particleDesc);

private:
	HRESULT Save_MainModule(const _tchar* _pDirectoyPath);
	HRESULT Load_MainModule(const _tchar* _pDirectoyPath);

private:
	CParticleModule* Module_Factory(MODULE _eModule);
	void TimeDelta_Calculate(_float& fTimeDelta);
	void Play_On_Awake();
	void Vector_Size_Allocate(_uint iNumInstance);
	_bool Continue_Option(PARTICLE_DESC& _particleDesc);

private:
	void Action_By_LifteTime(PARTICLE_DESC& _particleDesc);
	void Action_By_Duration();
	void Action_By_StopOption();
	
private:
	MAIN_MODULE m_MainModuleDesc;
	EMMISION_MODULE m_EmissionModuleDesc;
	vector<PARTICLE_DESC>	 m_ParticleDescs;
	vector<CParticleModule*> m_Modules;

private:
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