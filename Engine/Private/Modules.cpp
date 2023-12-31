#include "..\Public\Modules.h"
#include "ParticleSystem.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
void MODULE::Save(HANDLE hFile, _ulong& dwByte)
{
	WriteFile(hFile, &isActivate, sizeof isActivate, &dwByte, nullptr);
}
void MODULE::Load(HANDLE hFile, _ulong& dwByte)
{
	ReadFile(hFile, &isActivate, sizeof isActivate, &dwByte, nullptr);
}

HRESULT MAIN_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &isEnable, sizeof isEnable, &dwByte, nullptr);
	WriteFile(hFile, &fDuration, sizeof fDuration, &dwByte, nullptr);
	WriteFile(hFile, &isLooping, sizeof isLooping, &dwByte, nullptr);
	WriteFile(hFile, &isPrewarm, sizeof isPrewarm, &dwByte, nullptr);
	WriteFile(hFile, &isStartDelayRange, sizeof isStartDelayRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartDelayRange, sizeof vStartDelayRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartDelay, sizeof fStartDelay, &dwByte, nullptr);
	WriteFile(hFile, &isStartLifeTimeRange, sizeof isStartLifeTimeRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartLifeTimeRange, sizeof vStartLifeTimeRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartLifeTime, sizeof fStartLifeTime, &dwByte, nullptr);
	WriteFile(hFile, &isStartSpeedRange, sizeof isStartSpeedRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartSpeedRange, sizeof vStartSpeedRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartSpeed, sizeof fStartSpeed, &dwByte, nullptr);
	WriteFile(hFile, &isStartSizeRange, sizeof isStartSizeRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartSizeRange, sizeof vStartSizeRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartSize, sizeof fStartSize, &dwByte, nullptr);
	WriteFile(hFile, &is3DStartSize, sizeof is3DStartSize, &dwByte, nullptr);
	WriteFile(hFile, &v3DSizeXYZ, sizeof v3DSizeXYZ, &dwByte, nullptr);
	WriteFile(hFile, &is3DStartAngle, sizeof is3DStartAngle, &dwByte, nullptr);
	WriteFile(hFile, &isStartAngleRange, sizeof isStartAngleRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartAngleRange, sizeof vStartAngleRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartAngle, sizeof fStartAngle, &dwByte, nullptr);
	WriteFile(hFile, &vStartAngle3D, sizeof vStartAngle3D, &dwByte, nullptr);
	WriteFile(hFile, &fFlipAngle, sizeof fFlipAngle, &dwByte, nullptr);
	WriteFile(hFile, &vStartColor, sizeof vStartColor, &dwByte, nullptr);
	WriteFile(hFile, &fGravityModifier, sizeof fGravityModifier, &dwByte, nullptr);
	WriteFile(hFile, &fSimulationSpeed, sizeof fSimulationSpeed, &dwByte, nullptr);
	WriteFile(hFile, &isPlayOnAwake, sizeof isPlayOnAwake, &dwByte, nullptr);
	WriteFile(hFile, strEmmiterVelocity.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &iMaxParticles, sizeof iMaxParticles, &dwByte, nullptr);
	WriteFile(hFile, &isAutoRandomSeed, sizeof isAutoRandomSeed, &dwByte, nullptr);
	WriteFile(hFile, strStopAction.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isDirectionRotation, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &isStartColorRange, sizeof(isStartColorRange), &dwByte, nullptr);
	WriteFile(hFile, &vStartColor2, sizeof(vStartColor2), &dwByte, nullptr);
	WriteFile(hFile, &vStartColor3, sizeof(vStartColor3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT MAIN_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	_char szBuffer[MAX_PATH];
	__super::Load(hFile, dwByte);

	ReadFile(hFile, &isEnable, sizeof isEnable, &dwByte, nullptr);
	ReadFile(hFile, &fDuration, sizeof fDuration, &dwByte, nullptr);
	ReadFile(hFile, &isLooping, sizeof isLooping, &dwByte, nullptr);
	ReadFile(hFile, &isPrewarm, sizeof isPrewarm, &dwByte, nullptr);
	ReadFile(hFile, &isStartDelayRange, sizeof isStartDelayRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartDelayRange, sizeof vStartDelayRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartDelay, sizeof fStartDelay, &dwByte, nullptr);
	ReadFile(hFile, &isStartLifeTimeRange, sizeof isStartLifeTimeRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartLifeTimeRange, sizeof vStartLifeTimeRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartLifeTime, sizeof fStartLifeTime, &dwByte, nullptr);
	ReadFile(hFile, &isStartSpeedRange, sizeof isStartSpeedRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartSpeedRange, sizeof vStartSpeedRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartSpeed, sizeof fStartSpeed, &dwByte, nullptr);
	ReadFile(hFile, &isStartSizeRange, sizeof isStartSizeRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartSizeRange, sizeof vStartSizeRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartSize, sizeof fStartSize, &dwByte, nullptr);
	ReadFile(hFile, &is3DStartSize, sizeof is3DStartSize, &dwByte, nullptr);
	ReadFile(hFile, &v3DSizeXYZ, sizeof v3DSizeXYZ, &dwByte, nullptr);
	ReadFile(hFile, &is3DStartAngle, sizeof is3DStartAngle, &dwByte, nullptr);
	ReadFile(hFile, &isStartAngleRange, sizeof isStartAngleRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartAngleRange, sizeof vStartAngleRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartAngle, sizeof fStartAngle, &dwByte, nullptr);
	ReadFile(hFile, &vStartAngle3D, sizeof vStartAngle3D, &dwByte, nullptr);
	ReadFile(hFile, &fFlipAngle, sizeof fFlipAngle, &dwByte, nullptr);
	ReadFile(hFile, &vStartColor, sizeof vStartColor, &dwByte, nullptr);
	ReadFile(hFile, &fGravityModifier, sizeof fGravityModifier, &dwByte, nullptr);
	ReadFile(hFile, &fSimulationSpeed, sizeof fSimulationSpeed, &dwByte, nullptr);
	ReadFile(hFile, &isPlayOnAwake, sizeof isPlayOnAwake, &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strEmmiterVelocity = szBuffer;
	ReadFile(hFile, &iMaxParticles, sizeof iMaxParticles, &dwByte, nullptr);
	ReadFile(hFile, &isAutoRandomSeed, sizeof isAutoRandomSeed, &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strStopAction = szBuffer;
	ReadFile(hFile, &isDirectionRotation, sizeof isDirectionRotation, &dwByte, nullptr);
	ReadFile(hFile, &isStartColorRange, sizeof(isStartColorRange), &dwByte, nullptr);
	ReadFile(hFile, &vStartColor2, sizeof(vStartColor2), &dwByte, nullptr);
	ReadFile(hFile, &vStartColor3, sizeof(vStartColor3), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
void MAIN_MODULE::Restart()
{
	fParticleSystemAge = 0.f;
}

HRESULT EMISSION_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("EmissionModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &vPrevPos, sizeof vPrevPos, &dwByte, nullptr);
	WriteFile(hFile, &vCurPos, sizeof vCurPos, &dwByte, nullptr);
	WriteFile(hFile, &fRateOverTime, sizeof fRateOverTime, &dwByte, nullptr);
	WriteFile(hFile, &fRateOverTimeAcc, sizeof fRateOverTimeAcc, &dwByte, nullptr);
	WriteFile(hFile, &fRateOverDistance, sizeof fRateOverDistance, &dwByte, nullptr);

	_uint iSize = Bursts.size();
	WriteFile(hFile, &iSize, sizeof iSize, &dwByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		WriteFile(hFile, &Bursts[i].fTime, sizeof Bursts[i].fTime, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].iCount, sizeof Bursts[i].iCount, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].iCycleCount, sizeof Bursts[i].iCycleCount, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].iCycles, sizeof Bursts[i].iCycles, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].fIntervalTimeAcc, sizeof Bursts[i].fIntervalTimeAcc, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].fInterval, sizeof Bursts[i].fInterval, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].fProbability, sizeof Bursts[i].fProbability, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].fTriggerTimeAcc, sizeof Bursts[i].fTriggerTimeAcc, &dwByte, nullptr);
		WriteFile(hFile, &Bursts[i].isTrigger, sizeof Bursts[i].isTrigger, &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}
HRESULT EMISSION_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("EmissionModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Load(hFile, dwByte);

	ReadFile(hFile, &vPrevPos, sizeof vPrevPos, &dwByte, nullptr);
	ReadFile(hFile, &vCurPos, sizeof vCurPos, &dwByte, nullptr);
	ReadFile(hFile, &fRateOverTime, sizeof fRateOverTime, &dwByte, nullptr);
	ReadFile(hFile, &fRateOverTimeAcc, sizeof fRateOverTimeAcc, &dwByte, nullptr);
	ReadFile(hFile, &fRateOverDistance, sizeof fRateOverDistance, &dwByte, nullptr);

	_uint iSize;
	ReadFile(hFile, &iSize, sizeof iSize, &dwByte, nullptr);
	Bursts.resize(iSize);
	for (_uint i = 0; i < iSize; ++i)
	{
		ReadFile(hFile, &Bursts[i].fTime, sizeof Bursts[i].fTime, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].iCount, sizeof Bursts[i].iCount, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].iCycleCount, sizeof Bursts[i].iCycleCount, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].iCycles, sizeof Bursts[i].iCycles, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].fIntervalTimeAcc, sizeof Bursts[i].fIntervalTimeAcc, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].fInterval, sizeof Bursts[i].fInterval, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].fProbability, sizeof Bursts[i].fProbability, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].fTriggerTimeAcc, sizeof Bursts[i].fTriggerTimeAcc, &dwByte, nullptr);
		ReadFile(hFile, &Bursts[i].isTrigger, sizeof Bursts[i].isTrigger, &dwByte, nullptr);
	}

	CloseHandle(hFile);
	return S_OK;
}
void EMISSION_MODULE::Action(CParticleSystem* pParticleSystem, _float _fTimeDelta)
{
	if (false == isActivate)
		return;

	// EmissionModuel
	for (auto& Burst : Bursts)
	{
		if (true == Burst.isTrigger)
		{
			Burst.fIntervalTimeAcc += _fTimeDelta;
		}

		Burst.fTriggerTimeAcc += _fTimeDelta;
	}
	fRateOverTimeAcc += _fTimeDelta;

	for (auto& Burst : Bursts)
	{
		if (false == Burst.isTrigger && Burst.fTriggerTimeAcc >= Burst.fTime && 0 == Burst.iCycleCount)
		{
			if (RandomBool(Burst.fProbability))
			{
				Burst.isTrigger = true;
				Burst.iCycleCount = 0;
			}
		}

		if (true == Burst.isTrigger)
		{
			// 일정한 간격으로 파티클을 발생시키는 코드.
			if (Burst.fIntervalTimeAcc >= Burst.fInterval)
			{
				for (_uint i = 0; i < Burst.iCount.x; ++i)
				{
					pParticleSystem->Wating_One_Particle();
				}
				Burst.fIntervalTimeAcc = 0.f;
				++Burst.iCycleCount;
			}

			// 모든 사이클을 돌면 Trigger을 비활성화
			if (Burst.iCycleCount >= Burst.iCycles)
			{
				Burst.isTrigger = false;
			}
		}
	}

	// 시간에 따른 생성
	if (fRateOverTime >= 0.001f)
	{
		// 초당 N개씩 생성하는 코드
		_float fTimePerParticle = 1.f / fRateOverTime;
		while (fRateOverTimeAcc >= fTimePerParticle)
		{
			pParticleSystem->Wating_One_Particle();
			fRateOverTimeAcc -= fTimePerParticle;
		}
	}

	// 거리 따른 생성
	_float fPositionDelta = { _float3(vCurPos - vPrevPos).Length() };
	if (fPositionDelta < 0.01f)
		return;
	_float fTotalParticles = fPositionDelta * fRateOverDistance + fAccumulatedError;
	_uint iParticleCount = (_uint)fTotalParticles;

	fAccumulatedError = fTotalParticles - iParticleCount;

	for (_uint i = 0; i < iParticleCount; ++i)
	{
		_float3 createPosition = _float3::Lerp(vPrevPos, vCurPos, ((_float)i / iParticleCount));
		pParticleSystem->Get_Transform()->Set_Position(createPosition);
		pParticleSystem->Wating_One_Particle();
	}
}
void EMISSION_MODULE::Restart()
{
	for (auto& Burst : Bursts)
	{
		Burst.iCycleCount = 0;
		Burst.fIntervalTimeAcc = 0.f;
		Burst.isTrigger = false;
		Burst.fTriggerTimeAcc = 0.f;
	}
	fRateOverTimeAcc = 0.f;
	fAccumulatedError = 0.0f;
}

HRESULT SHAPE_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("ShapeModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, strShape.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strBoxEmitFrom.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strMeshType.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strMeshTypeMode.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strMesh.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isSingleMaterial, sizeof(isSingleMaterial), &dwByte, nullptr);
	WriteFile(hFile, &iMaterialNum, sizeof(iMaterialNum), &dwByte, nullptr);
	WriteFile(hFile, &isUseMeshColors, sizeof(isUseMeshColors), &dwByte, nullptr);
	WriteFile(hFile, &fNormalOffset, sizeof(fNormalOffset), &dwByte, nullptr);
	WriteFile(hFile, &vLength, sizeof(vLength), &dwByte, nullptr);
	WriteFile(hFile, strConeEmitFrom.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isPhiRange, sizeof(isPhiRange), &dwByte, nullptr);
	WriteFile(hFile, &vPhi, sizeof(vPhi), &dwByte, nullptr);
	WriteFile(hFile, strPhiMode.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &fPhiSpread, sizeof(fPhiSpread), &dwByte, nullptr);
	WriteFile(hFile, &isThetaRange, sizeof(isThetaRange), &dwByte, nullptr);
	WriteFile(hFile, &vTheta, sizeof(vTheta), &dwByte, nullptr);
	WriteFile(hFile, strThetaMode.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &fThetaSpread, sizeof(fThetaSpread), &dwByte, nullptr);
	WriteFile(hFile, &fThetaInterval, sizeof(fThetaInterval), &dwByte, nullptr);
	WriteFile(hFile, &fPhiInterval, sizeof(fPhiInterval), &dwByte, nullptr);
	WriteFile(hFile, wstrClipTexturePath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strClipChannel.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &fClipThreshold, sizeof(fClipThreshold), &dwByte, nullptr);
	WriteFile(hFile, &isColorAffectsParticles, sizeof(isColorAffectsParticles), &dwByte, nullptr);
	WriteFile(hFile, &isAlphaAffectsParticles, sizeof(isAlphaAffectsParticles), &dwByte, nullptr);
	WriteFile(hFile, &isBilinearFiltering, sizeof(isBilinearFiltering), &dwByte, nullptr);
	WriteFile(hFile, &ShapeMatrix, sizeof(ShapeMatrix), &dwByte, nullptr);
	ShapeMatrixInit = ShapeMatrix;
	WriteFile(hFile, &isAlignToDirection, sizeof(isAlignToDirection), &dwByte, nullptr);
	WriteFile(hFile, &fRandomizeDirection, sizeof(fRandomizeDirection), &dwByte, nullptr);
	WriteFile(hFile, &fSpherizeDirection, sizeof(fSpherizeDirection), &dwByte, nullptr);
	WriteFile(hFile, &fRandomizePosition, sizeof(fRandomizePosition), &dwByte, nullptr);
	WriteFile(hFile, &fRadiusThickness, sizeof(fRadiusThickness), &dwByte, nullptr);
	WriteFile(hFile, &fAngle, sizeof(fAngle), &dwByte, nullptr);
	WriteFile(hFile, &fBaseRadius, sizeof(fBaseRadius), &dwByte, nullptr);
	WriteFile(hFile, &fConeLength, sizeof(fConeLength), &dwByte, nullptr);
	WriteFile(hFile, &isChase, sizeof(isChase), &dwByte, nullptr);
	WriteFile(hFile, &isLengthRange, sizeof(isLengthRange), &dwByte, nullptr);
	WriteFile(hFile, &fLoopPhi, sizeof(fLoopPhi), &dwByte, nullptr);
	WriteFile(hFile, &fLoopTheta, sizeof(fLoopTheta), &dwByte, nullptr);
	WriteFile(hFile, &isCameraAxis, sizeof(isCameraAxis), &dwByte, nullptr);
	WriteFile(hFile, &vShapeRotation, sizeof(vShapeRotation), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
HRESULT SHAPE_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("ShapeModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	_char szBuffer[MAX_PATH];
	_tchar wszBuffer[MAX_PATH];
	__super::Load(hFile, dwByte);

	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strShape = szBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strBoxEmitFrom = szBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strMeshType = szBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strMeshTypeMode = szBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strMesh = szBuffer;
	ReadFile(hFile, &isSingleMaterial, sizeof(isSingleMaterial), &dwByte, nullptr);
	ReadFile(hFile, &iMaterialNum, sizeof(iMaterialNum), &dwByte, nullptr);
	ReadFile(hFile, &isUseMeshColors, sizeof(isUseMeshColors), &dwByte, nullptr);
	ReadFile(hFile, &fNormalOffset, sizeof(fNormalOffset), &dwByte, nullptr);
	ReadFile(hFile, &vLength, sizeof(vLength), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strConeEmitFrom = szBuffer;
	ReadFile(hFile, &isPhiRange, sizeof(isPhiRange), &dwByte, nullptr);
	ReadFile(hFile, &vPhi, sizeof(vPhi), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strPhiMode = szBuffer;
	ReadFile(hFile, &fPhiSpread, sizeof(fPhiSpread), &dwByte, nullptr);
	ReadFile(hFile, &isThetaRange, sizeof(isThetaRange), &dwByte, nullptr);
	ReadFile(hFile, &vTheta, sizeof(vTheta), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strThetaMode = szBuffer;
	ReadFile(hFile, &fThetaSpread, sizeof(fThetaSpread), &dwByte, nullptr);
	ReadFile(hFile, &fThetaInterval, sizeof(fThetaInterval), &dwByte, nullptr);
	ReadFile(hFile, &fPhiInterval, sizeof(fPhiInterval), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrClipTexturePath = wszBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strClipChannel = szBuffer;
	ReadFile(hFile, &fClipThreshold, sizeof(fClipThreshold), &dwByte, nullptr);
	ReadFile(hFile, &isColorAffectsParticles, sizeof(isColorAffectsParticles), &dwByte, nullptr);
	ReadFile(hFile, &isAlphaAffectsParticles, sizeof(isAlphaAffectsParticles), &dwByte, nullptr);
	ReadFile(hFile, &isBilinearFiltering, sizeof(isBilinearFiltering), &dwByte, nullptr);
	ReadFile(hFile, &ShapeMatrix, sizeof(ShapeMatrix), &dwByte, nullptr);
	ShapeMatrixInit = ShapeMatrix;
	ReadFile(hFile, &isAlignToDirection, sizeof(isAlignToDirection), &dwByte, nullptr);
	ReadFile(hFile, &fRandomizeDirection, sizeof(fRandomizeDirection), &dwByte, nullptr);
	ReadFile(hFile, &fSpherizeDirection, sizeof(fSpherizeDirection), &dwByte, nullptr);
	ReadFile(hFile, &fRandomizePosition, sizeof(fRandomizePosition), &dwByte, nullptr);

	ReadFile(hFile, &fRadiusThickness, sizeof(fRadiusThickness), &dwByte, nullptr);
	ReadFile(hFile, &fAngle, sizeof(fAngle), &dwByte, nullptr);
	ReadFile(hFile, &fBaseRadius, sizeof(fBaseRadius), &dwByte, nullptr);
	ReadFile(hFile, &fConeLength, sizeof(fConeLength), &dwByte, nullptr);

	ReadFile(hFile, &isChase, sizeof(isChase), &dwByte, nullptr);

	ReadFile(hFile, &isLengthRange, sizeof(isLengthRange), &dwByte, nullptr);
	ReadFile(hFile, &fLoopPhi, sizeof(fLoopPhi), &dwByte, nullptr);
	ReadFile(hFile, &fLoopTheta, sizeof(fLoopTheta), &dwByte, nullptr);
	ReadFile(hFile, &isCameraAxis, sizeof(isCameraAxis), &dwByte, nullptr);
	ReadFile(hFile, &vShapeRotation, sizeof(vShapeRotation), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
void SHAPE_MODULE::Set_ShapeLook(_float3 vSourPos, _float3 vDestPos)
{
	_float3 vRight;
	_float3 vUp = _float3(0.f, 1.f, 0.f);
	_float3 vLook = vDestPos - vSourPos;
	_float fLength = vLook.Length();

	// 제로벡터이면 리턴
	if (fLength <= 0.001f)
		return;

	vLook.Normalize();
	vRight = vUp.Cross(vLook);
	vUp = vLook.Cross(vRight);

	ShapeMatrix.Right(vRight);
	ShapeMatrix.Up(vUp);
	ShapeMatrix.Look(vLook);
}


void SHAPE_MODULE::Restart()
{
	fLoopPhi = vPhi.x;
	fLoopTheta = vTheta.x;
}


HRESULT RENDERER_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("RendererModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, wstrShaderTag.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, wstrMaterialPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isDeleteY, sizeof(isDeleteY), &dwByte, nullptr);
	WriteFile(hFile, &isUseGradientTexture, sizeof(isUseGradientTexture), &dwByte, nullptr);
	WriteFile(hFile, wstrGraientTexture.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strPass.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isGlow, sizeof(isGlow), &dwByte, nullptr);
	WriteFile(hFile, &isDistortion, sizeof(isDistortion), &dwByte, nullptr); 
	WriteFile(hFile, &isDiffuse, sizeof(isDiffuse), &dwByte, nullptr);
	WriteFile(hFile, wstrDistortionTexture.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

	WriteFile(hFile, &vStartOffset, sizeof(vStartOffset), &dwByte, nullptr);
	WriteFile(hFile, &vDeltaOffset, sizeof(vDeltaOffset), &dwByte, nullptr);
	WriteFile(hFile, &vStartTiling, sizeof(vStartTiling), &dwByte, nullptr);
	WriteFile(hFile, &vDeltaTiling, sizeof(vDeltaTiling), &dwByte, nullptr);

	WriteFile(hFile, &isEmission, sizeof(isEmission), &dwByte, nullptr);
	WriteFile(hFile, &fEmissionFrequency, sizeof(fEmissionFrequency), &dwByte, nullptr);
	WriteFile(hFile, &vEmissionRemap, sizeof(vEmissionRemap), &dwByte, nullptr);
	WriteFile(hFile, &vEmissionColor, sizeof(vEmissionColor), &dwByte, nullptr);
	WriteFile(hFile, strEmissionChannel.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, wstrEmissionPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	CloseHandle(hFile);
	return S_OK;
}
HRESULT RENDERER_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("RendererModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	_tchar wszBuffer[MAX_PATH];
	_char szBuffer[MAX_PATH];

	__super::Load(hFile, dwByte);

	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrShaderTag = wszBuffer;
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrMaterialPath = wszBuffer;
	ReadFile(hFile, &isDeleteY, sizeof(isDeleteY), &dwByte, nullptr);
	ReadFile(hFile, &isUseGradientTexture, sizeof(isUseGradientTexture), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	if (true == isUseGradientTexture)
		wstrGraientTexture = wszBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strPass = szBuffer;
	ReadFile(hFile, &isGlow, sizeof(isGlow), &dwByte, nullptr);
	ReadFile(hFile, &isDistortion, sizeof(isDistortion), &dwByte, nullptr);
	ReadFile(hFile, &isDiffuse, sizeof(isDiffuse), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrDistortionTexture = wszBuffer;
	ReadFile(hFile, &vStartOffset, sizeof(vStartOffset), &dwByte, nullptr);
	ReadFile(hFile, &vDeltaOffset, sizeof(vDeltaOffset), &dwByte, nullptr);
	ReadFile(hFile, &vStartTiling, sizeof(vStartTiling), &dwByte, nullptr);
	ReadFile(hFile, &vDeltaTiling, sizeof(vDeltaTiling), &dwByte, nullptr);

	ReadFile(hFile, &isEmission, sizeof(isEmission), &dwByte, nullptr);
	ReadFile(hFile, &fEmissionFrequency, sizeof(fEmissionFrequency), &dwByte, nullptr);
	ReadFile(hFile, &vEmissionRemap, sizeof(vEmissionRemap), &dwByte, nullptr);
	ReadFile(hFile, &vEmissionColor, sizeof(vEmissionColor), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	if(true == isEmission)
		strEmissionChannel = szBuffer;
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	if (true == isEmission)
		wstrEmissionPath = wszBuffer;
	CloseHandle(hFile);
	return S_OK;
}

void RENDERER_MODULE::Action(_float fTimeDelta)
{
	vOffset.x += vDeltaOffset.x * fTimeDelta;
	vOffset.y += vDeltaOffset.y * fTimeDelta;
	vTililing.x += vDeltaTiling.x * fTimeDelta;
	vTililing.y += vDeltaTiling.y * fTimeDelta;
}

void RENDERER_MODULE::Restart()
{
	vOffset = vStartOffset;
	vTililing = vStartTiling;
}

HRESULT RENDERER_MODULE::Bind_Values(CShader* pShader, CTexture* pEmissionTexture)
{
	if (FAILED(pShader->Bind_RawValue("g_vOffset", &vOffset, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vTililing", &vTililing, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue("g_isEmission", &isEmission, sizeof(isEmission))))
		return E_FAIL;
	if (isEmission)
	{
		if (FAILED(pEmissionTexture->Bind_ShaderResource(pShader, "g_EmissionTexture")))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_fEmissionFrequency", &fEmissionFrequency, sizeof(fEmissionFrequency))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vEmissionRemap", &vEmissionRemap, sizeof(vEmissionRemap))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vEmissionColor", &vEmissionColor, sizeof(vEmissionColor))))
			return E_FAIL;

		_int iEmissionChannel = { 0 };
		if (strEmissionChannel == "Red") { iEmissionChannel = 0; }
		else if (strEmissionChannel == "Green") { iEmissionChannel = 1; }
		else if (strEmissionChannel == "Blue") { iEmissionChannel = 2; }
		else if (strEmissionChannel == "Alpha") { iEmissionChannel = 3; }
		if (FAILED(pShader->Bind_RawValue("g_iEmissionChannel", &iEmissionChannel, sizeof(_int))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT VELOCITY_OVER_LIFETIME::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("VelocityOverLifeTimeModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &vLinear, sizeof(vLinear), &dwByte, nullptr);
	WriteFile(hFile, &vLinearMin, sizeof(vLinearMin), &dwByte, nullptr);
	WriteFile(hFile, &vLinearMax, sizeof(vLinearMax), &dwByte, nullptr);
	WriteFile(hFile, strSpace.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, strLinearOption.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &vOrbital, sizeof(vOrbital), &dwByte, nullptr);
	WriteFile(hFile, &vOrbitalMin, sizeof(vOrbitalMin), &dwByte, nullptr);
	WriteFile(hFile, &vOrbitalMax, sizeof(vOrbitalMax), &dwByte, nullptr);
	WriteFile(hFile, strOrbitalOption.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &vOffset, sizeof(vOffset), &dwByte, nullptr);
	WriteFile(hFile, &fRadial, sizeof(fRadial), &dwByte, nullptr);
	WriteFile(hFile, &vRadialRange, sizeof(vRadialRange), &dwByte, nullptr);
	WriteFile(hFile, strRadialOption.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &fSpeedModifier, sizeof(fRadial), &dwByte, nullptr);
	WriteFile(hFile, &vSpeedModifierRange, sizeof(vSpeedModifierRange), &dwByte, nullptr);
	WriteFile(hFile, strSpeedModifierOption.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
HRESULT VELOCITY_OVER_LIFETIME::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("VelocityOverLifeTimeModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	_char szBuffer[MAX_PATH];

	__super::Load(hFile, dwByte);

	ReadFile(hFile, &vLinear, sizeof(vLinear), &dwByte, nullptr);
	ReadFile(hFile, &vLinearMin, sizeof(vLinearMin), &dwByte, nullptr);
	ReadFile(hFile, &vLinearMax, sizeof(vLinearMax), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strSpace = szBuffer;
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strLinearOption = szBuffer;
	ReadFile(hFile, &vOrbital, sizeof(vOrbital), &dwByte, nullptr);
	ReadFile(hFile, &vOrbitalMin, sizeof(vOrbitalMin), &dwByte, nullptr);
	ReadFile(hFile, &vOrbitalMax, sizeof(vOrbitalMax), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strOrbitalOption = szBuffer;
	ReadFile(hFile, &vOffset, sizeof(vOffset), &dwByte, nullptr);
	ReadFile(hFile, &fRadial, sizeof(fRadial), &dwByte, nullptr);
	ReadFile(hFile, &vRadialRange, sizeof(vRadialRange), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strRadialOption = szBuffer;
	ReadFile(hFile, &fSpeedModifier, sizeof(fRadial), &dwByte, nullptr);
	ReadFile(hFile, &vSpeedModifierRange, sizeof(vSpeedModifierRange), &dwByte, nullptr);
	ReadFile(hFile, szBuffer, sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	strSpeedModifierOption = szBuffer;

	CloseHandle(hFile);
	return S_OK;
}
void VELOCITY_OVER_LIFETIME::Action(PARTICLE_IT& _particle_iter, const _float4x4* pLocalMatrix, _float fTimeDelta)
{
	if (false == isActivate)
		return;
	_float3 vRight, vUp, vLook, vPos;
	_float3 vOrbitCenter, vOrbitVelocity, vParticlePos, vRadial;
	_float3 vOrbitCenterDir;
	
	// 필요한 지역변수들 값 계산
	vRight = pLocalMatrix->Right();
	vUp = pLocalMatrix->Up();
	vLook = pLocalMatrix->Look();
	vPos = pLocalMatrix->Translation();

	// 구심점 계산
	vOrbitCenter = vPos - _particle_iter->WorldMatrix.Translation() + vOffset;
	vOrbitCenter.Normalize(vOrbitCenterDir);
	vParticlePos = _particle_iter->WorldMatrix.Translation();

	// Linear 계산
	if ("Local" == strSpace)
	{
		_float3 vNormalRight, vNormalUp, vNormalLook;
		vRight.Normalize(vNormalRight);
		vUp.Normalize(vNormalUp);
		vLook.Normalize(vNormalLook);
		
		// Orbit 계산

		if ("Constant" == strOrbitalOption)
		{
			vOrbitVelocity =
				(vOrbital.x * vNormalRight.Cross(vOrbitCenterDir) +
					vOrbital.y * vNormalUp.Cross(vOrbitCenterDir) +
					vOrbital.z * vNormalLook.Cross(vOrbitCenterDir)) * fTimeDelta;
		}
		else
		{
			vOrbitVelocity =
				(_particle_iter->vOrbitVelocity.x * vNormalRight.Cross(vOrbitCenterDir) +
					_particle_iter->vOrbitVelocity.y * vNormalUp.Cross(vOrbitCenterDir) +
					_particle_iter->vOrbitVelocity.z * vNormalLook.Cross(vOrbitCenterDir)) * fTimeDelta;
		}
		
	}
	else // World
	{
		// Orbit 계산
		if ("Constant" == strOrbitalOption)
		{
			vOrbitVelocity =
				(vOrbital.x * _float3(1.f, 0.f, 0.f).Cross(vOrbitCenterDir) +
					vOrbital.y * _float3(0.f, 1.f, 0.f).Cross(vOrbitCenterDir) +
					vOrbital.z * _float3(0.f, 0.f, 1.f).Cross(vOrbitCenterDir)) * fTimeDelta;
		}
		else
		{
			vOrbitVelocity =
				(_particle_iter->vOrbitVelocity.x * _float3(1.f, 0.f, 0.f).Cross(vOrbitCenterDir) +
					_particle_iter->vOrbitVelocity.y * _float3(0.f, 1.f, 0.f).Cross(vOrbitCenterDir) +
					_particle_iter->vOrbitVelocity.z * _float3(0.f, 0.f, 1.f).Cross(vOrbitCenterDir)) * fTimeDelta;
		}
	}

	// Radial 적용
	vRadial = -1.f * vOrbitCenterDir * fRadial * fTimeDelta;

	// Orbit 적용
	vParticlePos += (vRadial + vOrbitVelocity) * fSpeedModifier;

	// 위치 적용
	_particle_iter->WorldMatrix.Translation(vParticlePos);
}
void VELOCITY_OVER_LIFETIME::Restart()
{

}
void VELOCITY_OVER_LIFETIME::Reset(PARTICLE_IT& _particle_iter, const _float4x4* pLocalMatrix)
{
	if (false == isActivate)
		return;

	// 자동으로 0,0,0으로 초기화됨.
	_float3 vRight, vUp, vLook;
	_float3 vNormalRight, vNormalUp, vNormalLook;
	_float3 vVelocity;

	// 필요한 지역변수들 값 계산
	vRight = pLocalMatrix->Right();
	vUp = pLocalMatrix->Up();
	vLook = pLocalMatrix->Look();
	vRight.Normalize(vNormalRight);
	vUp.Normalize(vNormalUp);
	vLook.Normalize(vNormalLook);

	// SpeedModifier계산
	if ("Range" == strSpeedModifierOption)
	{
		fSpeedModifier = Random_Generator(vSpeedModifierRange.x, vSpeedModifierRange.y);
	}

	// Velocity 계산
	if ("Local" == strSpace) // Local일 경우
	{
		if ("Constant" == strLinearOption)
		{
			vVelocity += vLinear.x * vNormalRight;
			vVelocity += vLinear.y * vNormalUp;
			vVelocity += vLinear.z * vNormalLook;
		}
		else if ("Range" == strLinearOption)
		{
			vVelocity += Random_Generator(vLinearMin.x, vLinearMax.x) * vNormalRight;
			vVelocity += Random_Generator(vLinearMin.y, vLinearMax.y) * vNormalUp;
			vVelocity += Random_Generator(vLinearMin.z, vLinearMax.z) * vNormalLook;
		}
	}
	else // World일 경우
	{
		if ("Constant" == strLinearOption)
		{
			vVelocity += vLinear.x * _float3(1.f, 0.f, 0.f);
			vVelocity += vLinear.y * _float3(0.f, 1.f, 0.f);
			vVelocity += vLinear.z * _float3(0.f, 0.f, 1.f);
		}
		else if ("Range" == strLinearOption)
		{
			vVelocity += Random_Generator(vLinearMin.x, vLinearMax.x) * _float3(1.f, 0.f, 0.f);
			vVelocity += Random_Generator(vLinearMin.y, vLinearMax.y) * _float3(0.f, 1.f, 0.f);
			vVelocity += Random_Generator(vLinearMin.z, vLinearMax.z) * _float3(0.f, 0.f, 1.f);
		}
	}

	if ("Range" == strOrbitalOption)
	{
		_particle_iter->vOrbitVelocity.x = Random_Generator(vOrbitalMin.x, vOrbitalMax.x);
		_particle_iter->vOrbitVelocity.y = Random_Generator(vOrbitalMin.y, vOrbitalMax.y);
		_particle_iter->vOrbitVelocity.z = Random_Generator(vOrbitalMin.z, vOrbitalMax.z);
	}
	

	_particle_iter->vVelocity += (vVelocity * fSpeedModifier).TransNorm();
}

HRESULT ROTATION_OVER_LIFETIME_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("RotationOverLifeTimeModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &isAngularVelocityRange, sizeof(isAngularVelocityRange), &dwByte, nullptr);
	WriteFile(hFile, &fAngularVelocity, sizeof(fAngularVelocity), &dwByte, nullptr);
	WriteFile(hFile, &vAngularVelocityRange, sizeof(vAngularVelocityRange), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
HRESULT ROTATION_OVER_LIFETIME_MODULE::Load(const _tchar* _pDirectoryPath)
{
	fs::path fsFilePath = _pDirectoryPath;
	fsFilePath = fsFilePath / TEXT("RotationOverLifeTimeModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	__super::Load(hFile, dwByte);

	ReadFile(hFile, &isAngularVelocityRange, sizeof(isAngularVelocityRange), &dwByte, nullptr);
	ReadFile(hFile, &fAngularVelocity, sizeof(fAngularVelocity), &dwByte, nullptr);
	ReadFile(hFile, &vAngularVelocityRange, sizeof(vAngularVelocityRange), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
void ROTATION_OVER_LIFETIME_MODULE::Action(PARTICLE_IT& _particle_iter, _float _fTimeDelta)
{
	if (false == isActivate)
		return;

	// 이 구조에서는 빌보드를 가정(메쉬나 사용안함)하기 때문에 Angle에 대해서만 다룸.
	_particle_iter->fAngle += _particle_iter->fAngularVelocity * _fTimeDelta;
}
void ROTATION_OVER_LIFETIME_MODULE::Reset(PARTICLE_IT& _particle_iter)
{
	if (false == isActivate)
		return;

	if (false == isAngularVelocityRange)
	{
		_particle_iter->fAngularVelocity = fAngularVelocity;
	}
	else // Range인 경우
	{
		_particle_iter->fAngularVelocity = Random_Generator(vAngularVelocityRange.x, vAngularVelocityRange.y);
	}
}

HRESULT COLOR_OVER_LIFETIME::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("ColorOverLifeTimeModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &vStartColor, sizeof(vStartColor), &dwByte, nullptr);
	WriteFile(hFile, &vEndColor, sizeof(vEndColor), &dwByte, nullptr);
	WriteFile(hFile, &eEase, sizeof(eEase), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT COLOR_OVER_LIFETIME::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("ColorOverLifeTimeModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Load(hFile, dwByte);

	ReadFile(hFile, &vStartColor, sizeof(vStartColor), &dwByte, nullptr);
	ReadFile(hFile, &vEndColor, sizeof(vEndColor), &dwByte, nullptr);
	ReadFile(hFile, &eEase, sizeof(eEase), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
void COLOR_OVER_LIFETIME::Action(PARTICLE_IT& _particle_iter, _float fTimeDelta)
{
	if (false == isActivate)
		return;

	_float4 changeAmount = vEndColor - vStartColor;

	_particle_iter->vColor.x = _particle_iter->vStartColor.x * CEase::Ease(eEase, _particle_iter->fAge
		, vStartColor.x
		, changeAmount.x
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.y = _particle_iter->vStartColor.y * CEase::Ease(eEase, _particle_iter->fAge
		, vStartColor.y
		, changeAmount.y
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.z = _particle_iter->vStartColor.z * CEase::Ease(eEase, _particle_iter->fAge
		, vStartColor.z
		, changeAmount.z
		, _particle_iter->fLifeTime);

	_particle_iter->vColor.w = _particle_iter->vStartColor.w * CEase::Ease(eEase, _particle_iter->fAge
		, vStartColor.w
		, changeAmount.w
		, _particle_iter->fLifeTime);

	//_particle_iter->vColor = m_MainModuleDesc.vStartColor * _float3::Lerp(vStartColor.xyz()
	//	, vEndColor.xyz(), _particle_iter->fAge / _particle_iter->fLifeTime).TransCoord();

	// 레이어에서 빼고, 세이프 릴리즈도 해주고,
	// 풀매니저 -> 레퍼런스 카운트를 1을 가지고 있어서.
}
void COLOR_OVER_LIFETIME::Restart()
{
}

HRESULT SIZE_OVER_LIFETIME::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("SizeOverLifeTimeModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &isSeparateAxes, sizeof(isSeparateAxes), &dwByte, nullptr);
	WriteFile(hFile, &vSizeX, sizeof(vSizeX), &dwByte, nullptr);
	WriteFile(hFile, &vSizeY, sizeof(vSizeY), &dwByte, nullptr);
	WriteFile(hFile, &vSizeZ, sizeof(vSizeZ), &dwByte, nullptr);
	WriteFile(hFile, &eEaseX, sizeof(eEaseX), &dwByte, nullptr);
	WriteFile(hFile, &eEaseY, sizeof(eEaseY), &dwByte, nullptr);
	WriteFile(hFile, &eEaseZ, sizeof(eEaseZ), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT SIZE_OVER_LIFETIME::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("SizeOverLifeTimeModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;
	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Load(hFile, dwByte);

	ReadFile(hFile, &isSeparateAxes, sizeof(isSeparateAxes), &dwByte, nullptr);
	ReadFile(hFile, &vSizeX, sizeof(vSizeX), &dwByte, nullptr);
	ReadFile(hFile, &vSizeY, sizeof(vSizeY), &dwByte, nullptr);
	ReadFile(hFile, &vSizeZ, sizeof(vSizeZ), &dwByte, nullptr);
	ReadFile(hFile, &eEaseX, sizeof(eEaseX), &dwByte, nullptr);
	ReadFile(hFile, &eEaseY, sizeof(eEaseY), &dwByte, nullptr);
	ReadFile(hFile, &eEaseZ, sizeof(eEaseZ), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
void SIZE_OVER_LIFETIME::Action(_float3 vStartSize, PARTICLE_IT& _particle_iter, _float _fTimeDelta)
{
	if (false == isActivate)
		return;
	{
		_float min = vSizeX.x;
		_float max = vSizeX.y;
		_float fChangeAmount = max - min;
		_particle_iter->vScale.x = _particle_iter->vStartScale.x * CEase::Ease(eEaseX, _particle_iter->fAge
			, min
			, fChangeAmount
			, _particle_iter->fLifeTime);
	}

	if (true == isSeparateAxes)
	{
		{
			_float min = vSizeY.x;
			_float max = vSizeY.y;
			_float fChangeAmount = max - min;
			_particle_iter->vScale.y = _particle_iter->vStartScale.y * CEase::Ease(eEaseY, _particle_iter->fAge
				, min
				, fChangeAmount
				, _particle_iter->fLifeTime);
		}

		{
			_float min = vSizeZ.x;
			_float max = vSizeZ.y;
			_float fChangeAmount = max - min;
			_particle_iter->vScale.z = _particle_iter->vStartScale.z * CEase::Ease(eEaseZ, _particle_iter->fAge
				, min
				, fChangeAmount
				, _particle_iter->fLifeTime);
		}
	}
}
void SIZE_OVER_LIFETIME::Restart()
{
}

HRESULT TEXTURE_SHEET_ANIMATION::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("TextureSheetAnimationModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &iMaxIndex, sizeof iMaxIndex, &dwByte, nullptr);
	WriteFile(hFile, &iWidthLength, sizeof(iWidthLength), &dwByte, nullptr);
	WriteFile(hFile, &iHeightLength, sizeof(iHeightLength), &dwByte, nullptr);
	WriteFile(hFile, &isStartFrameRange, sizeof(isStartFrameRange), &dwByte, nullptr);
	WriteFile(hFile, &vStartFrameRange, sizeof(vStartFrameRange), &dwByte, nullptr);
	WriteFile(hFile, &fStartFrame, sizeof(fStartFrame), &dwByte, nullptr);
	WriteFile(hFile, &fUpdateInterval, sizeof(fUpdateInterval), &dwByte, nullptr);
	WriteFile(hFile, &isUseNormalTexture, sizeof(isUseNormalTexture), &dwByte, nullptr);
	WriteFile(hFile, wstrNormalPath.data(), sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isLoopOption, sizeof(isLoopOption), &dwByte, nullptr);
	WriteFile(hFile, &isAnimation, sizeof(isAnimation), &dwByte, nullptr);
	WriteFile(hFile, &isSeletedIndexRange, sizeof(isSeletedIndexRange), &dwByte, nullptr);
	WriteFile(hFile, &vSeletedIndexRange, sizeof(vSeletedIndexRange), &dwByte, nullptr);
	WriteFile(hFile, &iSeletedIndex, sizeof(iSeletedIndex), &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
HRESULT TEXTURE_SHEET_ANIMATION::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("TextureSheetAnimationModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	_tchar wszBuffer[MAX_PATH];
	__super::Load(hFile, dwByte);

	ReadFile(hFile, &iMaxIndex, sizeof iMaxIndex, &dwByte, nullptr);
	ReadFile(hFile, &iWidthLength, sizeof(iWidthLength), &dwByte, nullptr);
	ReadFile(hFile, &iHeightLength, sizeof(iHeightLength), &dwByte, nullptr);
	ReadFile(hFile, &isStartFrameRange, sizeof(isStartFrameRange), &dwByte, nullptr);
	ReadFile(hFile, &vStartFrameRange, sizeof(vStartFrameRange), &dwByte, nullptr);
	ReadFile(hFile, &fStartFrame, sizeof(fStartFrame), &dwByte, nullptr);
	ReadFile(hFile, &fUpdateInterval, sizeof(fUpdateInterval), &dwByte, nullptr);
	ReadFile(hFile, &isUseNormalTexture, sizeof(isUseNormalTexture), &dwByte, nullptr);
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	if (true == isUseNormalTexture)
		wstrNormalPath = wszBuffer;
	ReadFile(hFile, &isLoopOption, sizeof(isLoopOption), &dwByte, nullptr);
	ReadFile(hFile, &isAnimation, sizeof(isAnimation), &dwByte, nullptr);
	ReadFile(hFile, &isSeletedIndexRange, sizeof(isSeletedIndexRange), &dwByte, nullptr);
	ReadFile(hFile, &vSeletedIndexRange, sizeof(vSeletedIndexRange), &dwByte, nullptr);
	ReadFile(hFile, &iSeletedIndex, sizeof(iSeletedIndex), &dwByte, nullptr);
	CloseHandle(hFile);
	// 파티클 텍스처 시트 구현해!
	return S_OK;
}
void TEXTURE_SHEET_ANIMATION::Action(PARTICLE_IT& _particle_iter, _float fTimeDelta)
{
	if (false == isActivate)
		return;

	if (true == isAnimation)
	{
		fTimeAcc += fTimeDelta;
		while (fTimeAcc >= fUpdateInterval)
		{
			fTimeAcc -= fUpdateInterval;
			++_particle_iter->iCurIndex;
			if (_particle_iter->iCurIndex > iMaxIndex)
			{
				if (false == isLoopOption)
				{
					_particle_iter->fLifeTime = 0.f;
					_particle_iter->iCurIndex = iMaxIndex; // 마지막 1프레임 살아나는거 잡는 코드
				}
				else
				{
					_particle_iter->iCurIndex = 0;
				}
			}
		}
	}
}
void TEXTURE_SHEET_ANIMATION::Reset(PARTICLE_IT& _particle_iter)
{
	if (false == isActivate)
		return;

	if (true == isAnimation) // 텍스처시트 애니메이션
	{
		if (true == isStartFrameRange)
			fStartFrame = Random_Generator(vStartFrameRange.x, vStartFrameRange.y);

		_particle_iter->iCurIndex = _uint(iMaxIndex * fStartFrame);
	}
	else // 그냥 uv만 자르기
	{
		if (true == isSeletedIndexRange) // 랜덤로직
			_particle_iter->iCurIndex = static_cast<_uint>(Random_Generator(
				static_cast<_float>(vSeletedIndexRange.x),
				static_cast<_float>(vSeletedIndexRange.y + 0.99f))); // 마지막 인덱스도 나오도록 +0.99f를 함
		else
			_particle_iter->iCurIndex = iSeletedIndex;
	}
}
void TEXTURE_SHEET_ANIMATION::Restart()
{
	if (false == isActivate)
		return;

	fTimeAcc = 0.f;
}

void TEXTURE_SHEET_ANIMATION::CalculateMaxSize()
{
	iMaxIndex = iWidthLength * iHeightLength - 1;
}

HRESULT NOISE_MODULE::Save(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("NoiseModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_WRITE
		, 0
		, 0
		, CREATE_ALWAYS
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	__super::Save(hFile, dwByte);

	WriteFile(hFile, &fPersistence, sizeof fPersistence, &dwByte, nullptr);
	WriteFile(hFile, &fFrequency, sizeof fFrequency, &dwByte, nullptr);
	WriteFile(hFile, &fAmplitude, sizeof fAmplitude, &dwByte, nullptr);
	WriteFile(hFile, &iNumOctaves, sizeof iNumOctaves, &dwByte, nullptr);
	WriteFile(hFile, &vRemap, sizeof vRemap, &dwByte, nullptr);
	WriteFile(hFile, &vPositionAmount, sizeof vPositionAmount, &dwByte, nullptr);
	WriteFile(hFile, &vSizeAmount, sizeof vSizeAmount, &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}

HRESULT NOISE_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("NoiseModule.ptc");
	if (false == fs::exists(fsFilePath))
		return S_OK;

	HANDLE hFile = CreateFile(fsFilePath.wstring().data()
		, GENERIC_READ
		, 0
		, 0
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	_ulong dwByte = 0;
	__super::Load(hFile, dwByte);

	ReadFile(hFile, &fPersistence, sizeof fPersistence, &dwByte, nullptr);
	ReadFile(hFile, &fFrequency, sizeof fFrequency, &dwByte, nullptr);
	ReadFile(hFile, &fAmplitude, sizeof fAmplitude, &dwByte, nullptr);
	ReadFile(hFile, &iNumOctaves, sizeof iNumOctaves, &dwByte, nullptr);
	ReadFile(hFile, &vRemap, sizeof vRemap, &dwByte, nullptr);
	ReadFile(hFile, &vPositionAmount, sizeof vPositionAmount, &dwByte, nullptr);
	ReadFile(hFile, &vSizeAmount, sizeof vSizeAmount, &dwByte, nullptr);

	CloseHandle(hFile);
	// 파티클 텍스처 시트 구현해!
	return S_OK;
}

HRESULT NOISE_MODULE::Bind_Values(CShader* pShader)
{
	if (false == isActivate)
	{
		if (FAILED(pShader->Bind_RawValue("g_isNoiseActivated", &isActivate, sizeof(isActivate))))
			return E_FAIL;

		return S_OK;
	}
	if (FAILED(pShader->Bind_RawValue("g_isNoiseActivated", &isActivate, sizeof(isActivate))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_fAmplitude", &fAmplitude, sizeof(fAmplitude))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_fFrequency", &fFrequency, sizeof(fFrequency))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_iNumOctaves", &iNumOctaves, sizeof(iNumOctaves))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_fPersistence", &fPersistence, sizeof(fPersistence))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_vRemap", &vRemap, sizeof(vRemap))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_vPositionAmount", &vPositionAmount, sizeof(vPositionAmount))))
		return E_FAIL;
	if(FAILED(pShader->Bind_RawValue("g_vSizeAmount", &vSizeAmount, sizeof(vSizeAmount))))
		return E_FAIL;
	
	return S_OK;
}

void PARTICLE::Restart()
{
	fAge = { 0.f };
	fGravityAccel = { 0.f };
	vVelocity = _float4();
	WorldMatrix = _float4x4();
	fGenTime = { 0.f };
	fLifeTime = { 0.f };
	fAngularVelocity = { 0.f };
	fAngle = { 0.f };
	vColor = { 1.f, 1.f, 1.f, 1.f };
	vStartScale = { 1.f, 1.f, 1.f };
	vScale = { 1.f, 1.f, 1.f };
	iCurIndex = { 0 };
	isAlive = { true };
}


