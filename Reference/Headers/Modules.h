#pragma once

#include "Engine_Defines.h"
#include "Ease.h"

// 객체 : 모든 파티클을 돌리는 ParticleSystem의 인스턴스를 뜻함.
// 파티클 : 각 입자들을 의미함.


BEGIN(Engine)
struct PARTICLE;
typedef list<PARTICLE>::iterator PARTICLE_IT;

struct ENGINE_DLL PT_NOTIFY
{
	void Reset() { CanPlay = true; }

	function<bool(const PARTICLE_IT&) > StartCondition;
	function<void()> Event;
	_bool isPlayOnce = { true };
	_bool CanPlay = { true };
};


struct PARTICLE
{
public:
	void Add_Notify(PT_NOTIFY ptNotify) {
		Events.push_back(ptNotify); 
	}

public:
	_float		fAge = { 0.f };
	_float		fGravityAccel = { 0.f };
	_float4     vAccel = _float4();
	_float4     vVelocity = _float4();
	_float4x4   WorldMatrix = _float4x4();
	_float		fGenTime = { 0.f };
	_float      fLifeTime = { 0.f };
	_float		fAngle = { 0.f };
	_float4		vColor = { 1.f, 1.f, 1.f, 1.f };
	_float3		vStartScale = { 1.f, 1.f, 1.f };
	_float3		vScale = { 1.f, 1.f, 1.f };
	_uint		iCurIndex = { 0 };
	_bool		isAlive = { false };
	list<PT_NOTIFY>	Events;

public:
	void PlayEvent(const PARTICLE_IT&);

	friend class CParticleSystem;
};
class CParticleSystem;





struct ENGINE_DLL MODULE
{
	MODULE() : isActivate(false) {};

	void Save(HANDLE hFile, _ulong& dwByte);
	void Load(HANDLE hFile, _ulong& dwByte);

	_bool isActivate = { false };
};
struct ENGINE_DLL MAIN_MODULE : public MODULE
{
	MAIN_MODULE() : MODULE() { __super::isActivate = true; };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Restart();

	_bool isEnable = { true };
	_float fDuration = { 30.0f }; // 객체의 수명
	_bool isLooping = { false }; // 파티클의 반복
	_bool isPrewarm = { false }; // Loop활성화 시 사용 가능, Stop <-> Play 전환 시 파티클이 초기화 되지 않음.
	_bool isStartDelayRange = { false };
	_float2 vStartDelayRange = { 0.f, 0.f };
	_float fStartDelay = { 0.f }; // 초기화 시 파티클 시작 시간을 정함
	_bool isStartLifeTimeRange = { false };
	_float2 vStartLifeTimeRange = { 5.f, 5.f };
	_float fStartLifeTime = { 5.f }; // 초기화 시 정해지는 파티클의 수명을 정함
	_bool isStartSpeedRange = { false };
	_float2 vStartSpeedRange = { 5.f, 5.f };
	_float fStartSpeed = { 5.f }; // 초기화 시 정해지는 파티클의 속도
	_bool isStartSizeRange = { false };
	_float2 vStartSizeRange = { 1.f, 1.f };
	_float fStartSize = { 1.f }; // 초기화 시 정해지는 파티클 크기.
	_bool is3DStartSize = { false }; // x,y,z축 회전을 활성화하려면 true.
	_float3 v3DSizeXYZ = { 1.f, 1.f, 1.f }; // 파티클의 3D 스케일을 정해줍니다
	_bool is3DStartRotation = { false }; // 초기화 시 정해지는 파티클 3차원 회전값(월드 x,y,z축 기준)
	_bool isStartRotationRange = { false };
	_float2 vStartRotationRange = { 0.f, 0.f }; // 초기화 시 정해지는 파티클 오일러각(빌보드 행렬의 x,y축 기준)
	_float fStartRotation = { 0.f };
	_float3 v3DRotationXYZ = { 0.f, 0.f, 0.f }; // 파티클의 3D 스케일을 정해줍니다
	_float fFlipRotation = { 0.f }; // [0, 1]값으로 예를들어, 0.5의 값인 경우 파티클 초기화 시 50%확률로 반대방향으로 회전한다.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // 초기화 시 정해지는 파티클의 컬러
	_float fGravityModifier = { 0.f }; // 파티클에 적용 되는 중력값
	_float fSimulationSpeed = { 1.f }; // 파티클 재생 속도
	_bool isPlayOnAwake = { true }; // 객체를 생성하자마자 파티클을 재생할건지 정함.
	string strEmmiterVelocity = { "RigidBody" }; // RigidBoyd, Transform // Inherit Velocity 모듈과 Emission 모듈에 사용되는데 속도 정할 때 사용함.
	_int iMaxParticles = { MAX_PARTICLE_NUM }; // 한 번에 존재할 수 있는 파티클의 수를 제한함.(인스턴싱 수가 100이여도 10으로 제한하면 10개만 나옴)
	_bool isAutoRandomSeed = { true }; // 파티클 수명 주기마다 랜덤 값을 매번 바뀌게하는 용도.
	string strStopAction = {"None"}; // None, Disable, Destroy, Callback // 객체 수명이 다하거나 파티클의 모든 재생이 완료됐을 때 옵션에 따라 행동이 달라진다.
	_bool isDirectionRotation = { false };

	_float fParticleSystemAge = { 0.f };
};
struct ENGINE_DLL EMISSION_MODULE : public MODULE
{
	EMISSION_MODULE() : MODULE() { __super::isActivate = true; };
	~EMISSION_MODULE() { Bursts.clear(); }
	
	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Setting_PrevPos(_float3 vPos) { vPrevPos = vPos; }
	void Action(CParticleSystem* pParticleSystem, _float _fTimeDelta);
	void Restart();

	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // Time초 마다 트리거 발동.
		_int2 iCount = { 30, 30 }; // 1번의 Interval에 방출할 파티클 수
		_uint iCycleCount = { 0 }; // Cycle에 사용할 누적값.
		_int iCycles = { 1 }; // Time초 마다 Interval번 만큼 Cycle번 반복
		_float fIntervalTimeAcc = { 0.f }; // Interval에 사용할 시간 누적값.
		_float fInterval = { 0.010f }; // Time초 마다 Interval간격으로 방출
		_float fProbability = { 1.f }; // 트리거가 발생할 확률 [0 ,1]
		_float fTriggerTimeAcc = { 0.f }; // 트리거 발생 조건에 사용할 시간 누적값
		_bool isTrigger = { false }; // true이면 로직 발동
	}BURST;

	_float3	vPrevPos = { _float3() };
	_float3	vCurPos = { _float3() };
	_float	fRateOverTime = { 10.f }; // 1초 동안 몇 개의 파티클을 뿜어낼지 정함.
	_float	fRateOverTimeAcc = { 0.f };
	_float	fRateOverDistance = { 0.f }; // 움직인 거리에 따라 몇 개의 파티클을 뿜어낼지 정함.
	vector<BURST> Bursts;
	_float fAccumulatedError = { 0.0f };
};
struct ENGINE_DLL SHAPE_MODULE : public MODULE
{
	SHAPE_MODULE() : MODULE() { __super::isActivate = true; };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Restart();
	void Set_ShapeLook(_float3 vSourPos, _float3 vDestPos);

	string strShape = { "Sphere" }; // Shpere, Box, Mesh, Sprite, Rectangle
	string strBoxEmitFrom = { "Volume" }; // Volume, Sheel, Edge

	string strMeshType = { "Vertex" }; // Vertex, Edge, Triangle
	string strMeshTypeMode = { "Random" }; // Random, Loop, Ping-Pong

	string strMesh = { "None" }; // "None", "Cube", "Capsule", "Cylinder", "Plane", "Sphere", "Quad" };
	_bool isSingleMaterial = { false }; // 모든 서브메쉬 사용안함.
	_uint iMaterialNum = { 0 }; // 사용할 서브메쉬 수.
	_bool isUseMeshColors = { true }; // 버퍼의 꼭지점이 가진 색상을 입자와 섞는다.
	_float fNormalOffset = { 0.f }; // 0인 경우 메쉬의 노말벡터의 꼬리에서 시작, 오프셋 값 만큼 시작위치는 노말 벡터 방향으로 이동한다.
	

	_bool isLengthRange = { false }; //--------
	_float2 vLength = { 0.f, 5.f };

	_bool isPhiRange = { true };
	_float2 vPhi = { 0.f, 360.f };
	   string strPhiMode = { "Random" }; // Random, Loop, Ping-Pong, Burst_Spread
	  _float fPhiSpread = { 0.f }; // [0, 1]

	_bool isThetaRange = { true };
	_float2 vTheta = { 0.f, 360.f };
	   string strThetaMode = { "Random" }; // Random, Loop, Ping-Pong, Burst_Spread
	  _float fThetaSpread = { 0.f }; // [0, 1] 1에 가까울수록 전 지점에서 터짐.

	_float fLoopPhi = { 0.f }; // ---------
	_float fLoopTheta = { 0.f }; // -------
	_float fPhiInterval = { 1.f };
	_float fThetaInterval = { 1.f };

	_float fRadiusThickness = { 1.f };

	// Cone
	string strConeEmitFrom = { "Base" }; // Base, Volume
	_float fAngle = { 25.f };
	_float fBaseRadius = { 0.5f };
	_float fConeLength = { 1.f }; // Volume에서 Length로 활용됨..
	///////////////////////////
	
	wstring wstrClipTexturePath = { TEXT("../../Resources/Effects/Textures/Default_Particle.png") }; // 아래 인자의 채널에 사용할 텍스처
	string strClipChannel = { "Red" }; // Red, Greend, Blue, Alpha // 클립 채널(클립 : 알파테스트로 discard)
	_float fClipThreshold = { 0.f }; // [0, 1], 이것보다 작은 값들은 알파테스트 실패함.
	_bool isColorAffectsParticles = { true };
	_bool isAlphaAffectsParticles = { true };
	_bool isBilinearFiltering = { false };

	_float4x4 ShapeMatrix = { _float4x4() };

	_bool isAlignToDirection = false;
	_float fRandomizeDirection = { 0.f }; //[0, 1]
	_float fSpherizeDirection = { 0.f };  //[0, 1]
	_float fRandomizePosition = { 0.f };

	_bool isChase = { false };
	_bool isCameraAxis = { false };
};

struct ENGINE_DLL ROTATION_OVER_LIFETIME_MODULE : public MODULE
{
	ROTATION_OVER_LIFETIME_MODULE() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(PARTICLE_IT& _particle_iter, _float _fTimeDelta);
	void Restart();

	// 자체 회전에 사용할 값들
	_bool isSeparateAxes = { false };
	_float2 vAngularVelocityX = { 1.f, 1.f };
	_float2 vAngularVelocityY = { 1.f, 1.f };
	_float2 vAngularVelocityZ = { 1.f, 1.f };
	CEase::EASE eEaseX = { CEase::OUT_QUINT };
	CEase::EASE eEaseY = { CEase::OUT_QUINT };
	CEase::EASE eEaseZ = { CEase::OUT_QUINT };
};
struct ENGINE_DLL COLOR_OVER_LIFETIME : public MODULE
{
	COLOR_OVER_LIFETIME() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(PARTICLE_IT& _particle_iter, _float4 vMainColor, _float fTimeDelta);
	void Restart();

	_float4 vStartColor = { 0.f, 0.f, 0.f, 1.f };
	_float4 vEndColor = { 1.f, 1.f, 1.f, 1.f };
	CEase::EASE eEase = { CEase::OUT_QUINT };
};
struct ENGINE_DLL SIZE_OVER_LIFETIME : public MODULE
{
	SIZE_OVER_LIFETIME() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(_float3 vStartSize, PARTICLE_IT& _particle_iter, _float _fTimeDelta);
	void Reset(PARTICLE_IT& _particle_iter);
	void Restart();

	enum OPTION { OPTION_UNIT, OPTION_3D, OPTION_RANGE};
	
	_bool isSeparateAxes = { false };
	_float2 vSizeX = { 1.f, 1.f };
	_float2 vSizeY = { 1.f, 1.f };
	_float2 vSizeZ = { 1.f, 1.f };
	CEase::EASE eEaseX = { CEase::OUT_QUINT };
	CEase::EASE eEaseY = { CEase::OUT_QUINT };
	CEase::EASE eEaseZ = { CEase::OUT_QUINT };

	_float fSizeTimeAcc = { 0.f };
};
struct ENGINE_DLL TEXTURE_SHEET_ANIMATION : public MODULE
{
	TEXTURE_SHEET_ANIMATION() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(PARTICLE_IT& _particle_iter, _float fTimeDelta);
	void CalculateMaxSize();
	void Reset(PARTICLE_IT& _particle_iter);
	void Restart();

	_uint iMaxIndex = { 0 };
	_uint iWidthLength = { 1 };
	_uint iHeightLength = { 1 };
	_bool isStartFrameRange = { false };
	_float2 vStartFrameRange = { 0.f, 1.f };
	_float fStartFrame = { 0.f }; // [0, 1]
	_float fUpdateInterval = { 0.0159f };
	_bool isUseNormalTexture = { false };
	wstring wstrNormalPath = TEXT("../../Resources/Effects/Textures/Default_Normal.png");
	_bool isLoopOption = { false };

	_float fTimeAcc = { 0.f };
};

struct ENGINE_DLL NOISE_MODULE : public MODULE
{
	NOISE_MODULE() : MODULE() {  };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Reset(PARTICLE_IT& _particle_iter);
	void Restart();

	enum OPTION { CONSTANT, RANGE, CURVE, OPTION_END };
	_bool isSeparateAxes = { false }; 
	string strStrengthOption = "Constant"; // Constant, Range, Curve
	_float fStrength = { 1.f };
	_float2 vStrengthRange = { 1.f, 1.f };
	_float3 vStrength3DMin = { 1.f, 1.f, 1.f }; // 노이즈에 미치는 영향력
	_float3 vStrength3DMax = { 1.f, 1.f, 1.f }; // 노이즈에 미치는 영향력
	CEase::EASE eStrengthEaseX = { CEase::OUT_QUINT };
	CEase::EASE eStrengthEaseY = { CEase::OUT_QUINT };
	CEase::EASE eStrengthEaseZ = { CEase::OUT_QUINT };

	_float fFrequency = { 0.5f }; // 낮을수록 부드러운 노이즈 높을수록 급격한 변화

	string strScrollSpeedOption = "Constant"; // Constant, Curve
	_float fScrollSpeed = { 0.5f }; // 노이즈 필드를 시간에 흐름에 따라 움직이는 속도.
	CEase::EASE eScrollSpeedEase = { CEase::OUT_QUINT };

	_bool isDamping = { true }; // 활성화하면 세기가 빈도에 비례합니다. 두 값을 묶으면 파티클의 동작을 그대로 유지하면서 크기는 달라지도록 노이즈 필드를 스케일할 수 있습니다.
	_uint iOctaves = { 1 }; // 노이즈 레이어를 얼마나 많이 생성할지 결정
	_float fOctaveMultiplier = { 0.5f }; // [0, 1]
	_uint iOctaveScale = { 1 }; // [1, )

	_bool isRemap = { false }; // 최종 노이즈값을 다시 매핑
	CEase::EASE eRemapEase = { CEase::OUT_QUINT };

	string strPositionAmountOption = "Constant"; // Constant, Range, Curve
	_float2 vPositionAmount = { 0.f, 0.f };
	CEase::EASE ePositionAmountEase = { CEase::OUT_QUINT };

	string strRotationAmountOption = "Constant"; // Constant, Range, Curve
	_float2 vRotationAmount = { 0.f, 0.f }; // 단위는 초당 도
	CEase::EASE eRotationAmountEase = { CEase::OUT_QUINT };

	string strSizeAmountOption = "Constant"; // Constant, Range, Curve
	_float2 vSizeAmount = { 0.f, 0.f };
	CEase::EASE eSizeAmountEase = { CEase::OUT_QUINT };
};

struct ENGINE_DLL VELOCITY_OVER_LIFETIME : public MODULE
{
	VELOCITY_OVER_LIFETIME() : MODULE() {};

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(PARTICLE_IT& _particle_iter, _float fTimeDelta);
	void Reset(PARTICLE_IT& _particle_iter);
	void Restart();

	string strSpace = { "World" }; // World, Local
	// Linear
	_float3 vLinear = { _float3() };
	_float3 vLinearMin = { _float3() };
	_float3 vLinearMax = { _float3() };
	CEase::EASE eLinearEaseX;
	CEase::EASE eLinearEaseY;
	CEase::EASE eLinearEaseZ;
	
	string strLinearOption = { "Constant" }; // Constant, Range, Curve
	
	_float3 vLinearStartCurve3D = { _float3() };
	_float3 vLinearEndCurve3D = { _float3() };

	// Orbital
	_float3 vOrbital = { _float3() };
	_float3 vOrbitalMin = { _float3() };
	_float3 vOrbitalMax = { _float3() };
	CEase::EASE eOrbitalEase;

	// Offset
	_float3 vOffset = { _float3() };
	_float3 vOffsetMin = { _float3() };
	_float3 vOffsetMax = { _float3() };
	CEase::EASE eOffsetEase;

	// Radial
	_float fRadial;
	_float2 vRadialRange;
	CEase::EASE eRadialEase;

	// SpeedModifier
	_float fSpeedModifier;
	_float2 vSpeedModifierRange;
	CEase::EASE eSpeedModifierEase;
};

struct ENGINE_DLL RENDERER_MODULE : public MODULE
{
	RENDERER_MODULE() : MODULE() { __super::isActivate = true; };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Restart();

	wstring wstrShaderTag = { TEXT("Shader_VtxRectColInstance") };
	wstring wstrMaterialPath = { TEXT("../../Resources/Effects/Textures/Default_Particle.png") };
	_bool isDeleteY = { false };
	_bool isUseGradientTexture = { false };
	wstring wstrGraientTexture = { TEXT("../../Resources/Effects/Textures/Gradients/Default_Gradient.png") };
	string strPass = { "Default" };
	_bool isGlow = { false };
};

END