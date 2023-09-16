#include "..\Public\Modules.h"

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
	WriteFile(hFile, &is3DStartRotation, sizeof is3DStartRotation, &dwByte, nullptr);
	WriteFile(hFile, &isStartRotationRange, sizeof isStartRotationRange, &dwByte, nullptr);
	WriteFile(hFile, &vStartRotationRange, sizeof vStartRotationRange, &dwByte, nullptr);
	WriteFile(hFile, &fStartRotation, sizeof fStartRotation, &dwByte, nullptr);
	WriteFile(hFile, &v3DRotationXYZ, sizeof v3DRotationXYZ, &dwByte, nullptr);
	WriteFile(hFile, &fFlipRotation, sizeof fFlipRotation, &dwByte, nullptr);
	WriteFile(hFile, &vStartColor, sizeof vStartColor, &dwByte, nullptr);
	WriteFile(hFile, &fGravityModifier, sizeof fGravityModifier, &dwByte, nullptr);
	WriteFile(hFile, &fSimulationSpeed, sizeof fSimulationSpeed, &dwByte, nullptr);
	WriteFile(hFile, &isPlayOnAwake, sizeof isPlayOnAwake, &dwByte, nullptr);
	WriteFile(hFile, strEmmiterVelocity.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &iMaxParticles, sizeof iMaxParticles, &dwByte, nullptr);
	WriteFile(hFile, &isAutoRandomSeed, sizeof isAutoRandomSeed, &dwByte, nullptr);
	WriteFile(hFile, strStopAction.data(), sizeof(_char) * MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &isDirectionRotation, sizeof(_bool), &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT MAIN_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");

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
	ReadFile(hFile, &is3DStartRotation, sizeof is3DStartRotation, &dwByte, nullptr);
	ReadFile(hFile, &isStartRotationRange, sizeof isStartRotationRange, &dwByte, nullptr);
	ReadFile(hFile, &vStartRotationRange, sizeof vStartRotationRange, &dwByte, nullptr);
	ReadFile(hFile, &fStartRotation, sizeof fStartRotation, &dwByte, nullptr);
	ReadFile(hFile, &v3DRotationXYZ, sizeof v3DRotationXYZ, &dwByte, nullptr);
	ReadFile(hFile, &fFlipRotation, sizeof fFlipRotation, &dwByte, nullptr);
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
	WriteFile(hFile, &isAlignToDirection, sizeof(isAlignToDirection), &dwByte, nullptr);
	WriteFile(hFile, &fRandomizeDirection, sizeof(fRandomizeDirection), &dwByte, nullptr);
	WriteFile(hFile, &fSpherizeDirection, sizeof(fSpherizeDirection), &dwByte, nullptr);
	WriteFile(hFile, &fRandomizePosition, sizeof(fRandomizePosition), &dwByte, nullptr);
	WriteFile(hFile, &fRadiusThickness, sizeof(fRadiusThickness), &dwByte, nullptr);
	WriteFile(hFile, &fAngle, sizeof(fAngle), &dwByte, nullptr);
	WriteFile(hFile, &fBaseRadius, sizeof(fBaseRadius), &dwByte, nullptr);
	WriteFile(hFile, &fConeLength, sizeof(fConeLength), &dwByte, nullptr);
	WriteFile(hFile, &isChase, sizeof(isChase), &dwByte, nullptr);
	
	CloseHandle(hFile);
	return S_OK;
}
HRESULT SHAPE_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("ShapeModule.ptc");

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
	ReadFile(hFile, &isAlignToDirection, sizeof(isAlignToDirection), &dwByte, nullptr);
	ReadFile(hFile, &fRandomizeDirection, sizeof(fRandomizeDirection), &dwByte, nullptr);
	ReadFile(hFile, &fSpherizeDirection, sizeof(fSpherizeDirection), &dwByte, nullptr);
	ReadFile(hFile, &fRandomizePosition, sizeof(fRandomizePosition), &dwByte, nullptr);

	ReadFile(hFile, &fRadiusThickness, sizeof(fRadiusThickness), &dwByte, nullptr);
	ReadFile(hFile, &fAngle, sizeof(fAngle), &dwByte, nullptr);
	ReadFile(hFile, &fBaseRadius, sizeof(fBaseRadius), &dwByte, nullptr);
	ReadFile(hFile, &fConeLength, sizeof(fConeLength), &dwByte, nullptr);

	ReadFile(hFile, &isChase, sizeof(isChase), &dwByte, nullptr);
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

	CloseHandle(hFile);
	return S_OK;
}
HRESULT RENDERER_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("RendererModule.ptc");

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

	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrShaderTag = wszBuffer;
	ReadFile(hFile, wszBuffer, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);
	wstrMaterialPath = wszBuffer;

	CloseHandle(hFile);
	return S_OK;
}
void RENDERER_MODULE::Restart()
{
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

	WriteFile(hFile, &isSeperateAxes, sizeof isSeperateAxes, &dwByte, nullptr);
	WriteFile(hFile, &AngularVelocityXYZ, sizeof AngularVelocityXYZ, &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}
HRESULT ROTATION_OVER_LIFETIME_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("RotationOverLifeTimeModule.ptc");

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

	ReadFile(hFile, &isSeperateAxes, sizeof isSeperateAxes, &dwByte, nullptr);
	ReadFile(hFile, &AngularVelocityXYZ, sizeof AngularVelocityXYZ, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
void ROTATION_OVER_LIFETIME_MODULE::Restart()
{
}

HRESULT COLOR_OVER_LIFETIME::Save(const _tchar* _pDirectoyPath)
{
	return S_OK;
}
HRESULT COLOR_OVER_LIFETIME::Load(const _tchar* _pDirectoyPath)
{
	return S_OK;
}
void COLOR_OVER_LIFETIME::Restart()
{
}

HRESULT SIZE_OVER_LIFETIME::Save(const _tchar* _pDirectoyPath)
{
	return S_OK;
}
HRESULT SIZE_OVER_LIFETIME::Load(const _tchar* _pDirectoyPath)
{
	return S_OK;
}
void SIZE_OVER_LIFETIME::Restart()
{
}
