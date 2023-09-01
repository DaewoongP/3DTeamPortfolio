#pragma once

#include "Engine_Defines.h"

// ��ü : ��� ��ƼŬ�� ������ ParticleSystem�� �ν��Ͻ��� ����.
// ��ƼŬ : �� ���ڵ��� �ǹ���.

enum CLIP_CHANNEL { RED, BLUE, GREEN, ALPHA, CHANNEL_END };

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

	enum EMMITER_VELOCITY { RIGIDBODY, TRANSFORM, EV_END };
	enum STOP_OPTION {
		SO_NONE // �ƹ��� �ɼ��� ����
		, SO_DISABLE // ��ƼŬ ��Ȱ��ȭ
		, SO_DESTROY // ��ü �Ҹ�
		, SO_CALLBACK // �Լ� ȣ��(�Լ��� ���ٽ��� ���������)
		, SO_END
	};

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);

	_float fDuration = { 5.0f }; // ��ü�� ����
	_bool isLooping = { false }; // ��ƼŬ�� �ݺ�
	_bool isPrewarm = { false }; // LoopȰ��ȭ �� ��� ����, Stop <-> Play ��ȯ �� ��ƼŬ�� �ʱ�ȭ ���� ����.
	_float2 fStartDelay = { 0.f, 0.f }; // �ʱ�ȭ �� ��ƼŬ ���� �ð��� ����
	_float2 fStartLifeTime = { 5.f, 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� ������ ����
	_float2 fStartSpeed = { 5.f, 5.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �ӵ�
	_bool is3DStartSize = { false }; // x,y,z�� ȸ���� Ȱ��ȭ�Ϸ��� true.
	_float2 fStartSize = { 1.f, 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ ũ��.
	_float3 f3DSizeXYZ = { 1.f, 1.f, 1.f }; // ��ƼŬ�� 3D �������� �����ݴϴ�
	_bool is3DStartRotation = { false }; // �ʱ�ȭ �� �������� ��ƼŬ 3���� ȸ����(���� x,y,z�� ����)
	_float2 fStartRotation = { 0.f, 0.f }; // �ʱ�ȭ �� �������� ��ƼŬ ���Ϸ���(������ ����� x,y�� ����)
	_float3 f3DRotationXYZ = { 0.f, 0.f, 0.f }; // ��ƼŬ�� 3D �������� �����ݴϴ�
	_float fFlipRotation = { 0.f }; // [0, 1]������ �������, 0.5�� ���� ��� ��ƼŬ �ʱ�ȭ �� 50%Ȯ���� �ݴ�������� ȸ���Ѵ�.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // �ʱ�ȭ �� �������� ��ƼŬ�� �÷�
	_float fGravityModifier = { 0.f }; // ��ƼŬ�� ���� �Ǵ� �߷°�
	_float fSimulationSpeed = { 1.f }; // ��ƼŬ ��� �ӵ�
	_bool isPlayOnAwake = { true }; // ��ü�� �������ڸ��� ��ƼŬ�� ����Ұ��� ����.
	EMMITER_VELOCITY eEmmiterVelocity = { RIGIDBODY }; // Inherit Velocity ���� Emission ��⿡ ���Ǵµ� �ӵ� ���� �� �����.
	_int iMaxParticles = { 1000 }; // �� ���� ������ �� �ִ� ��ƼŬ�� ���� ������.(�ν��Ͻ� ���� 100�̿��� 10���� �����ϸ� 10���� ����)
	_bool isAutoRandomSeed = { true }; // ��ƼŬ ���� �ֱ⸶�� ���� ���� �Ź� �ٲ���ϴ� �뵵.
	STOP_OPTION eStopAction = { SO_NONE }; // ��ü ������ ���ϰų� ��ƼŬ�� ��� ����� �Ϸ���� �� �ɼǿ� ���� �ൿ�� �޶�����.
};
struct ENGINE_DLL EMMISION_MODULE : public MODULE
{
	EMMISION_MODULE() : MODULE() { __super::isActivate = true; };
	~EMMISION_MODULE() { Bursts.clear(); }

	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // Time�� ���� Ʈ���� �ߵ�.
		_int2 iCount = { 30, 30 }; // 1���� Interval�� ������ ��ƼŬ ��
		_int iCycles = { 1 }; // Time�� ���� Interval�� ��ŭ Cycle�� �ݺ�
		_float fInterval = { 0.010f }; // Time�� ���� Interval�������� ����
		_float fProbability = { 1.f }; // Ʈ���Ű� �߻��� Ȯ�� [0 ,1]
	}BURST;

	_float2	fRateOverTime = { 10.f, 10.f }; // 1�� ���� �� ���� ��ƼŬ�� �վ�� ����.
	_float2	fRateOverDistance = { 0.f, 0.f }; // ������ �Ÿ��� ���� �� ���� ��ƼŬ�� �վ�� ����.
	vector<BURST> Bursts;
};

struct ENGINE_DLL SHAPE_MODULE : public MODULE
{
	enum SHAPE { S_SPHERE, S_HEMISPHERE, S_CONE, S_MESH, S_CIRCLE, S_EDGE, S_RECTANGLE, SHAPE_END };
	enum MODE { RANDOM, LOOP, PING_PONG, BURST_SPREAD, MODE_END };

	// Shape�� �ٴڿ��� �������� �������� �������� �����Ѵ�.
	enum EMIT_FROM { BASE, VOLUME, EMIT_FROM_END }; // ������� ������ �ظ�� ������ü �� �����ϴ� �ɼ�
	enum MESH { M_NONE, M_CUBE, M_CAPSULE, M_CYLINDER, M_PLANE, M_SPHERE, M_QUAD, };
	enum TYPE { VERTEX, EDGE, TRIANGLE, TYPE_END };

	SHAPE eShape;

	TYPE eType;
	MODE eTypeMode;

	MESH eMeshType = { M_NONE };
	_bool isSingleMaterial = { false }; // ��� ����޽� ������.
	_uint iMaterialNum = { 0 }; // ����� ����޽� ��.
	_bool isUseMeshColors = { true }; // ������ �������� ���� ������ ���ڿ� ���´�.
	_float fNormalOffset = { 0.f }; // 0�� ��� �޽��� �븻������ �������� ����, ������ �� ��ŭ ������ġ�� �븻 ���� �������� �̵��Ѵ�.

	EMIT_FROM eEmitFrom = { BASE };
	_float fAngle = { 25.f };
	_float fRadius = { 1.f };
	_float fDonutRadius = { 0.2f };
	_float fRadiusThickness = { 1.f }; // [0, 1]

	_float fArc = { 360.f };
	  MODE eArcMode = { RANDOM };
	 _float fSpread = { 0.f }; // [0, 1]

	string strTexture = ""; // ���� ����� ������ �ؽ�ó ���
	CLIP_CHANNEL eClipChannel; // Ŭ�� ä��(Ŭ�� : �����׽�Ʈ�� discard)
	_float fClipThreshold = { 0.f }; // [0, 1], �̰ͺ��� ���� ������ �����׽�Ʈ ������.
	_bool isColorAffectsParticles = { true };
	_bool isAlphaAffectsParticles = { true };
	_bool isBilinearFiltering = { false };

	_float3 fPosition = { 0.f, 0.f, 0.f };
	_float3 fRotation = { 0.f, 0.f, 0.f };
	_float3 fScale = { 1.f, 1.f, 1.f };

	_bool isAlignToDirection = false;
	_float fRandomizeDirection; //[0, 1]
	_float fSpherizeDirection; //[0, 1]
	_float fRandomizePosition;
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