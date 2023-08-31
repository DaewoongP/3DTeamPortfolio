#pragma once

#include "Component.h"
#include "VIBuffer_Point_Color_Instance.h"
BEGIN(Engine)

// 객체 : 모든 파티클을 돌리는 ParticleSystem의 인스턴스를 뜻함.
// 파티클 : 각 입자들을 의미함.
struct ENGINE_DLL MAIN_MODULE
{
	// 일단 넣기는 했는데 리지드바디 사용하는거라 보류.
	enum EMMITER_VELOCITY { RIGIDBODY, TRANSFORM, EV_END };

	// Stop 옵션
	enum STOP_OPTION {
		SO_NONE // 아무런 옵션이 없음
		, SO_DISABLE // 파티클 비활성화
		, SO_DESTROY // 객체 소멸
		, SO_CALLBACK // 함수 호출(함수나 람다식을 던져줘야함)
		, SO_END };

	_float fDuration = { 5.0f }; // 객체의 수명
	_bool isLooping = { false }; // 파티클의 반복
	_bool isPrewarm = { false }; // Loop활성화 시 사용 가능, Stop <-> Play 전환 시 파티클이 초기화 되지 않음.
	_float2 fStartDelay = { 0.f, 0.f }; // 초기화 시 파티클 시작 시간을 정함
	_float2 fStartLifeTime = { 5.f, 5.f }; // 초기화 시 정해지는 파티클의 수명을 정함
	_float2 fStartSpeed = { 5.f, 5.f }; // 초기화 시 정해지는 파티클의 속도
	_bool is3DStartSize = { false }; // x,y,z축 회전을 활성화하려면 true.
	_float2 fStartSize = { 1.f, 1.f }; // 초기화 시 정해지는 파티클 크기.
	_bool is3DStartRotation = { false }; // 초기화 시 정해지는 파티클 3차원 회전값(월드 x,y,z축 기준)
	_float2 fStartRotation = { 0.f, 0.f }; // 초기화 시 정해지는 파티클 오일러각(빌보드 행렬의 x,y축 기준)
	_float fFlipRotation = { 0.f }; // [0, 1]값으로 예를들어, 0.5의 값인 경우 파티클 초기화 시 50%확률로 반대방향으로 회전한다.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // 초기화 시 정해지는 파티클의 컬러.
	_float fGravityModifier = { 0.f }; // 파티클에 적용 되는 중력값
	_float fSimulationSpeed = { 1.f }; // 파티클 재생 속도
	_bool isPlayOnAwake = { true }; // 객체를 생성하자마자 파티클을 재생할건지 정함.
	EMMITER_VELOCITY eEmmiterVelocity = { RIGIDBODY }; //  Inherit Velocity 모듈과 Emission 모듈에 사용되는데 속도 정할 때 사용함.
	_int iMaxParticles = { 1000 }; // 한 번에 존재할 수 있는 파티클의 수를 제한함.(인스턴싱 수가 100이여도 10으로 제한하면 10개만 나옴)
	_bool isAutoRandomSeed = { true }; // 파티클 수명 주기마다 랜덤 값을 매번 바뀌게하는 용도.
	STOP_OPTION eStopAction = { SO_NONE }; // 객체 수명이 다하거나 파티클의 모든 재생이 완료됐을 때 옵션에 따라 행동이 달라진다.
};

struct ENGINE_DLL EMMISION_MODULE
{
	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // 재생 후 시작 시간
		_int iCount = { 30 }; // 한꺼번에 몇개의 파티클이 나올지 정함
		_int iCycles = { 1 }; // 한 번의 Interval에 몇 번 재생 할건지 정함.
		_float fInterval = { 0.010f }; // 초기화 간격
	}BURST;

	_int	iRateOverTime = { 10 }; // 1초 동안 몇 개의 파티클을 뿜어낼지 정함.
	_int	iRateOverDistance = { 0 }; // 움직인 거리에 따라 몇 개의 파티클을 뿜어낼지 정함.
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
	// NULL이 아닌 모든 모듈을 데이터 파일로 쓴다.
	void Save(const _tchar* _pDirectoyPath);

	// 파일이 존재하면 로드한다.(없는 모듈은 저장을 안해서 파일이 없을 수 있음.)
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