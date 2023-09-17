#pragma once

#include "Engine_Defines.h"
#include "Ease.h"

// ��ü : ��� ��ƼŬ�� ������ ParticleSystem�� �ν��Ͻ��� ����.
// ��ƼŬ : �� ���ڵ��� �ǹ���.
typedef struct tagParticle
{
	_float		fAge = { 0.f };
	_float		fGravityAccel = { 0.f };
	_float4     vAccel = _float4();
	_float4     vVelocity = _float4();
	_float4x4   WorldMatrix = _float4x4();
	_float		fGenTime = { 0.f };
	_float      fLifeTime = { 0.f };
	_float		fAngle = { 0.f };
	_float4		vColor = { 1.f, 1.f, 1.f, 1.f };
	_float3		vScale = { 1.f, 1.f, 1.f };
	_uint		iCurIndex = { 0 };
}PARTICLE;
typedef list<PARTICLE>::iterator PARTICLE_IT;

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
	_float fParticleSystemAge = { 0.f };
	_float fDuration = { 30.0f }; // ��ü�� ����
	_bool isLooping = { false }; // ��ƼŬ�� �ݺ�
	_bool isPrewarm = { false }; // LoopȰ��ȭ �� ��� ����, Stop <-> Play ��ȯ �� ��ƼŬ�� �ʱ�ȭ ���� ����.
	_bool isStartDelayRange = { false };
	_float2 vStartDelayRange = { 0.f, 0.f };
	_float fStartDelay = { 0.f }; // �ʱ�ȭ �� ��ƼŬ ���� �ð��� ����
	_bool isStartLifeTimeRange = { false };
	_float2 vStartLifeTimeRange = { 5.f, 5.f };
	_float fStartLifeTime = { 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� ������ ����
	_bool isStartSpeedRange = { false };
	_float2 vStartSpeedRange = { 5.f, 5.f };
	_float fStartSpeed = { 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �ӵ�
	_bool isStartSizeRange = { false };
	_float2 vStartSizeRange = { 1.f, 1.f };
	_float fStartSize = { 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ ũ��.
	_bool is3DStartSize = { false }; // x,y,z�� ȸ���� Ȱ��ȭ�Ϸ��� true.
	_float3 v3DSizeXYZ = { 1.f, 1.f, 1.f }; // ��ƼŬ�� 3D �������� �����ݴϴ�
	_bool is3DStartRotation = { false }; // �ʱ�ȭ �� �������� ��ƼŬ 3���� ȸ����(���� x,y,z�� ����)
	_bool isStartRotationRange = { false };
	_float2 vStartRotationRange = { 0.f, 0.f }; // �ʱ�ȭ �� �������� ��ƼŬ ���Ϸ���(������ ����� x,y�� ����)
	_float fStartRotation = { 0.f };
	_float3 v3DRotationXYZ = { 0.f, 0.f, 0.f }; // ��ƼŬ�� 3D �������� �����ݴϴ�
	_float fFlipRotation = { 0.f }; // [0, 1]������ �������, 0.5�� ���� ��� ��ƼŬ �ʱ�ȭ �� 50%Ȯ���� �ݴ�������� ȸ���Ѵ�.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �÷�
	_float fGravityModifier = { 0.f }; // ��ƼŬ�� ���� �Ǵ� �߷°�
	_float fSimulationSpeed = { 1.f }; // ��ƼŬ ��� �ӵ�
	_bool isPlayOnAwake = { true }; // ��ü�� �������ڸ��� ��ƼŬ�� ����Ұ��� ����.
	string strEmmiterVelocity = { "RigidBody" }; // RigidBoyd, Transform // Inherit Velocity ���� Emission ��⿡ ���Ǵµ� �ӵ� ���� �� �����.
	_int iMaxParticles = { MAX_PARTICLE_NUM }; // �� ���� ������ �� �ִ� ��ƼŬ�� ���� ������.(�ν��Ͻ� ���� 100�̿��� 10���� �����ϸ� 10���� ����)
	_bool isAutoRandomSeed = { true }; // ��ƼŬ ���� �ֱ⸶�� ���� ���� �Ź� �ٲ���ϴ� �뵵.
	string strStopAction = {"None"}; // None, Disable, Destroy, Callback // ��ü ������ ���ϰų� ��ƼŬ�� ��� ����� �Ϸ���� �� �ɼǿ� ���� �ൿ�� �޶�����.
	_bool isDirectionRotation = { false };
};
struct ENGINE_DLL EMISSION_MODULE : public MODULE
{
	EMISSION_MODULE() : MODULE() { __super::isActivate = true; };
	~EMISSION_MODULE() { Bursts.clear(); }
	
	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Restart();

	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // Time�� ���� Ʈ���� �ߵ�.
		_int2 iCount = { 30, 30 }; // 1���� Interval�� ������ ��ƼŬ ��
		_uint iCycleCount = { 0 }; // Cycle�� ����� ������.
		_int iCycles = { 1 }; // Time�� ���� Interval�� ��ŭ Cycle�� �ݺ�
		_float fIntervalTimeAcc = { 0.f }; // Interval�� ����� �ð� ������.
		_float fInterval = { 0.010f }; // Time�� ���� Interval�������� ����
		_float fProbability = { 1.f }; // Ʈ���Ű� �߻��� Ȯ�� [0 ,1]
		_float fTriggerTimeAcc = { 0.f }; // Ʈ���� �߻� ���ǿ� ����� �ð� ������
		_bool isTrigger = { false }; // true�̸� ���� �ߵ�
	}BURST;

	_float3	vPrevPos = { _float3() };
	_float3	vCurPos = { _float3() };
	_float	fRateOverTime = { 10.f }; // 1�� ���� �� ���� ��ƼŬ�� �վ�� ����.
	_float	fRateOverTimeAcc = { 0.f };
	_float	fRateOverDistance = { 0.f }; // ������ �Ÿ��� ���� �� ���� ��ƼŬ�� �վ�� ����.
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
	_bool isSingleMaterial = { false }; // ��� ����޽� ������.
	_uint iMaterialNum = { 0 }; // ����� ����޽� ��.
	_bool isUseMeshColors = { true }; // ������ �������� ���� ������ ���ڿ� ���´�.
	_float fNormalOffset = { 0.f }; // 0�� ��� �޽��� �븻������ �������� ����, ������ �� ��ŭ ������ġ�� �븻 ���� �������� �̵��Ѵ�.
	

	_bool isfLengthRange = { false };
	_float2 vLength = { 0.f, 5.f };

	_bool isPhiRange = { true };
	_float2 vPhi = { 0.f, 360.f };
	   string strPhiMode = { "Random" }; // Random, Loop, Ping-Pong, Burst_Spread
	  _float fPhiSpread = { 0.f }; // [0, 1]

	_bool isThetaRange = { true };
	_float2 vTheta = { 0.f, 360.f };
	   string strThetaMode = { "Random" }; // Random, Loop, Ping-Pong, Burst_Spread
	  _float fThetaSpread = { 0.f }; // [0, 1] 1�� �������� �� �������� ����.

	_float fLoopPhi = { 0.f };
	_float fLoopTheta = { 0.f };
	_float fPhiInterval = { 1.f };
	_float fThetaInterval = { 1.f };

	_float fRadiusThickness = { 1.f };

	// Cone
	string strConeEmitFrom = { "Base" }; // Base, Volume
	_float fAngle = { 25.f };
	_float fBaseRadius = { 0.5f };
	_float fConeLength = { 1.f }; // Volume���� Length�� Ȱ���..
	///////////////////////////

	wstring wstrClipTexturePath = { TEXT("../../Resources/Effects/Textures/Default_Particle.png") }; // �Ʒ� ������ ä�ο� ����� �ؽ�ó
	string strClipChannel = { "Red" }; // Red, Greend, Blue, Alpha // Ŭ�� ä��(Ŭ�� : �����׽�Ʈ�� discard)
	_float fClipThreshold = { 0.33f }; // [0, 1], �̰ͺ��� ���� ������ �����׽�Ʈ ������.
	_bool isColorAffectsParticles = { true };
	_bool isAlphaAffectsParticles = { true };
	_bool isBilinearFiltering = { false };

	_float4x4 ShapeMatrix = { _float4x4() };

	_bool isAlignToDirection = false;
	_float fRandomizeDirection = { 0.f }; //[0, 1]
	_float fSpherizeDirection = { 0.f };  //[0, 1]
	_float fRandomizePosition = { 0.f };

	_bool isChase = { false };
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
};
struct ENGINE_DLL ROTATION_OVER_LIFETIME_MODULE : public MODULE
{
	ROTATION_OVER_LIFETIME_MODULE() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Restart();

	// ��ü ȸ���� ����� ����
	_bool isSeperateAxes = { false }; // �� Ȱ��ȭ�� �⺻ Z����
	_float3 AngularVelocityXYZ = { 0.f, 0.f, 0.f };
};
struct ENGINE_DLL COLOR_OVER_LIFETIME : public MODULE
{
	COLOR_OVER_LIFETIME() : MODULE() { };

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);
	void Action(PARTICLE_IT& _particle_iter, _float _fTimeDelta);
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
	void Action(PARTICLE_IT& _particle_iter, _float _fTimeDelta);
	void Restart();

	_bool isSeparateAxes = { false };
	_float3 vSizeXYZ = { 1.f, 1.f, 1.f };
	_float fSize = { 1.f };
	_float2 vSizeRange = { 0.f, 1.f };

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
	_uint iWidthLength = { 0 };
	_uint iHeightLength = { 0 };
	_bool isStartFrameRange = { false };
	_float2 vStartFrameRange = { 0.f, 1.f };
	_float fStartFrame = { 0.f }; // [0, 1]
	_float fUpdateInterval = { 0.0159f };
	_float fTimeAcc = { 0.f };
	_bool isUseNormalTexture = { false };
	wstring wstrNormalPath = TEXT("../../Resources/Effects/Textures/Flipbooks/VFX_T_Dust_8x8_N.png");
};
