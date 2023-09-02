#pragma once

#include "Engine_Defines.h"

// 객체 : 모든 파티클을 돌리는 ParticleSystem의 인스턴스를 뜻함.
// 파티클 : 각 입자들을 의미함.

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
		SO_NONE // 아무런 옵션이 없음
		, SO_DISABLE // 파티클 비활성화
		, SO_DESTROY // 객체 소멸
		, SO_CALLBACK // 함수 호출(함수나 람다식을 던져줘야함)
		, SO_END
	};

	HRESULT Save(const _tchar* _pDirectoyPath);
	HRESULT Load(const _tchar* _pDirectoyPath);

	_float fDuration = { 5.0f }; // 객체의 수명
	_bool isLooping = { false }; // 파티클의 반복
	_bool isPrewarm = { false }; // Loop활성화 시 사용 가능, Stop <-> Play 전환 시 파티클이 초기화 되지 않음.
	_float2 fStartDelay = { 0.f, 0.f }; // 초기화 시 파티클 시작 시간을 정함
	_float2 fStartLifeTime = { 5.f, 5.f }; // 초기화 시 정해지는 파티클의 수명을 정함
	_float2 fStartSpeed = { 5.f, 5.f }; // 초기화 시 정해지는 파티클의 속도
	_bool is3DStartSize = { false }; // x,y,z축 회전을 활성화하려면 true.
	_float2 fStartSize = { 1.f, 1.f }; // 초기화 시 정해지는 파티클 크기.
	_float3 f3DSizeXYZ = { 1.f, 1.f, 1.f }; // 파티클의 3D 스케일을 정해줍니다
	_bool is3DStartRotation = { false }; // 초기화 시 정해지는 파티클 3차원 회전값(월드 x,y,z축 기준)
	_float2 fStartRotation = { 0.f, 0.f }; // 초기화 시 정해지는 파티클 오일러각(빌보드 행렬의 x,y축 기준)
	_float3 f3DRotationXYZ = { 0.f, 0.f, 0.f }; // 파티클의 3D 스케일을 정해줍니다
	_float fFlipRotation = { 0.f }; // [0, 1]값으로 예를들어, 0.5의 값인 경우 파티클 초기화 시 50%확률로 반대방향으로 회전한다.
	_float4 vStartColor = { 1.f, 1.f, 1.f, 1.f }; // 초기화 시 정해지는 파티클의 컬러
	_float fGravityModifier = { 0.f }; // 파티클에 적용 되는 중력값
	_float fSimulationSpeed = { 1.f }; // 파티클 재생 속도
	_bool isPlayOnAwake = { true }; // 객체를 생성하자마자 파티클을 재생할건지 정함.
	EMMITER_VELOCITY eEmmiterVelocity = { RIGIDBODY }; // Inherit Velocity 모듈과 Emission 모듈에 사용되는데 속도 정할 때 사용함.
	_int iMaxParticles = { 1000 }; // 한 번에 존재할 수 있는 파티클의 수를 제한함.(인스턴싱 수가 100이여도 10으로 제한하면 10개만 나옴)
	_bool isAutoRandomSeed = { true }; // 파티클 수명 주기마다 랜덤 값을 매번 바뀌게하는 용도.
	STOP_OPTION eStopAction = { SO_NONE }; // 객체 수명이 다하거나 파티클의 모든 재생이 완료됐을 때 옵션에 따라 행동이 달라진다.
};
struct ENGINE_DLL EMMISION_MODULE : public MODULE
{
	EMMISION_MODULE() : MODULE() { __super::isActivate = true; };
	~EMMISION_MODULE() { Bursts.clear(); }

	typedef struct tagBurst
	{
		_float fTime = { 0.f }; // Time초 마다 트리거 발동.
		_int2 iCount = { 30, 30 }; // 1번의 Interval에 방출할 파티클 수
		_int iCycles = { 1 }; // Time초 마다 Interval번 만큼 Cycle번 반복
		_float fInterval = { 0.010f }; // Time초 마다 Interval간격으로 방출
		_float fProbability = { 1.f }; // 트리거가 발생할 확률 [0 ,1]
	}BURST;

	_float2	fRateOverTime = { 10.f, 10.f }; // 1초 동안 몇 개의 파티클을 뿜어낼지 정함.
	_float2	fRateOverDistance = { 0.f, 0.f }; // 움직인 거리에 따라 몇 개의 파티클을 뿜어낼지 정함.
	vector<BURST> Bursts;
};

struct ENGINE_DLL SHAPE_MODULE : public MODULE
{
	enum SHAPE { S_SPHERE, S_HEMISPHERE, S_CONE, S_MESH, S_CIRCLE, S_EDGE, S_RECTANGLE, SHAPE_END };
	enum MODE { RANDOM, LOOP, PING_PONG, BURST_SPREAD, MODE_END };

	// Shape의 바닥에서 방출할지 볼륨에서 방출할지 결정한다.
	enum EMIT_FROM { BASE, VOLUME, EMIT_FROM_END }; // 예를들어 원뿔의 밑면과 원뿔전체 중 선택하는 옵션
	enum MESH { M_NONE, M_CUBE, M_CAPSULE, M_CYLINDER, M_PLANE, M_SPHERE, M_QUAD, };
	enum TYPE { VERTEX, EDGE, TRIANGLE, TYPE_END };

	SHAPE eShape;

	TYPE eType;
	MODE eTypeMode;

	MESH eMeshType = { M_NONE };
	_bool isSingleMaterial = { false }; // 모든 서브메쉬 사용안함.
	_uint iMaterialNum = { 0 }; // 사용할 서브메쉬 수.
	_bool isUseMeshColors = { true }; // 버퍼의 꼭지점이 가진 색상을 입자와 섞는다.
	_float fNormalOffset = { 0.f }; // 0인 경우 메쉬의 노말벡터의 꼬리에서 시작, 오프셋 값 만큼 시작위치는 노말 벡터 방향으로 이동한다.

	EMIT_FROM eEmitFrom = { BASE };
	_float fAngle = { 25.f };
	_float fRadius = { 1.f };
	_float fDonutRadius = { 0.2f };
	_float fRadiusThickness = { 1.f }; // [0, 1]

	_float fArc = { 360.f };
	  MODE eArcMode = { RANDOM };
	 _float fSpread = { 0.f }; // [0, 1]

	string strTexture = ""; // 방출 모양을 결정할 텍스처 경로
	CLIP_CHANNEL eClipChannel; // 클립 채널(클립 : 알파테스트로 discard)
	_float fClipThreshold = { 0.f }; // [0, 1], 이것보다 작은 값들은 알파테스트 실패함.
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