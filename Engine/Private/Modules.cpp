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

	WriteFile(hFile, &fDuration, sizeof fDuration, &dwByte, nullptr);
	WriteFile(hFile, &isLooping, sizeof isLooping, &dwByte, nullptr);
	WriteFile(hFile, &isPrewarm, sizeof isPrewarm, &dwByte, nullptr);
	WriteFile(hFile, &fStartDelay, sizeof fStartDelay, &dwByte, nullptr);
	WriteFile(hFile, &fStartLifeTime, sizeof fStartLifeTime, &dwByte, nullptr);
	WriteFile(hFile, &fStartSpeed, sizeof fStartSpeed, &dwByte, nullptr);
	WriteFile(hFile, &is3DStartSize, sizeof is3DStartSize, &dwByte, nullptr);
	WriteFile(hFile, &f3DSizeXYZ, sizeof f3DSizeXYZ, &dwByte, nullptr);
	WriteFile(hFile, &fStartSize, sizeof fStartSize, &dwByte, nullptr);
	WriteFile(hFile, &is3DStartRotation, sizeof is3DStartRotation, &dwByte, nullptr);
	WriteFile(hFile, &fStartRotation, sizeof fStartRotation, &dwByte, nullptr);
	WriteFile(hFile, &f3DRotationXYZ, sizeof f3DRotationXYZ, &dwByte, nullptr);
	WriteFile(hFile, &fFlipRotation, sizeof fFlipRotation, &dwByte, nullptr);
	WriteFile(hFile, &vStartColor, sizeof vStartColor, &dwByte, nullptr);
	WriteFile(hFile, &fGravityModifier, sizeof fGravityModifier, &dwByte, nullptr);
	WriteFile(hFile, &fSimulationSpeed, sizeof fSimulationSpeed, &dwByte, nullptr);
	WriteFile(hFile, &isPlayOnAwake, sizeof isPlayOnAwake, &dwByte, nullptr);
	WriteFile(hFile, &strEmmiterVelocity, sizeof strEmmiterVelocity, &dwByte, nullptr);
	WriteFile(hFile, &iMaxParticles, sizeof iMaxParticles, &dwByte, nullptr);
	WriteFile(hFile, &isAutoRandomSeed, sizeof isAutoRandomSeed, &dwByte, nullptr);
	WriteFile(hFile, &strStopAction, sizeof strStopAction, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}
HRESULT MAIN_MODULE::Load(const _tchar* _pDirectoyPath)
{
	fs::path fsFilePath = _pDirectoyPath;
	fsFilePath = fsFilePath / TEXT("MainModule.ptc");

	HANDLE hFile = CreateFile(fsFilePath.wstring().data(),
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return E_FAIL;
	}
	_ulong	dwByte = 0;

	ReadFile(hFile, &fDuration, sizeof fDuration, &dwByte, nullptr);
	ReadFile(hFile, &isLooping, sizeof isLooping, &dwByte, nullptr);
	ReadFile(hFile, &isPrewarm, sizeof isPrewarm, &dwByte, nullptr);
	ReadFile(hFile, &fStartDelay, sizeof fStartDelay, &dwByte, nullptr);
	ReadFile(hFile, &fStartLifeTime, sizeof fStartLifeTime, &dwByte, nullptr);
	ReadFile(hFile, &fStartSpeed, sizeof fStartSpeed, &dwByte, nullptr);
	ReadFile(hFile, &is3DStartSize, sizeof is3DStartSize, &dwByte, nullptr);
	ReadFile(hFile, &f3DSizeXYZ, sizeof f3DSizeXYZ, &dwByte, nullptr);
	ReadFile(hFile, &fStartSize, sizeof fStartSize, &dwByte, nullptr);
	ReadFile(hFile, &is3DStartRotation, sizeof is3DStartRotation, &dwByte, nullptr);
	ReadFile(hFile, &fStartRotation, sizeof fStartRotation, &dwByte, nullptr);
	ReadFile(hFile, &f3DRotationXYZ, sizeof f3DRotationXYZ, &dwByte, nullptr);
	ReadFile(hFile, &fFlipRotation, sizeof fFlipRotation, &dwByte, nullptr);
	ReadFile(hFile, &vStartColor, sizeof vStartColor, &dwByte, nullptr);
	ReadFile(hFile, &fGravityModifier, sizeof fGravityModifier, &dwByte, nullptr);
	ReadFile(hFile, &fSimulationSpeed, sizeof fSimulationSpeed, &dwByte, nullptr);
	ReadFile(hFile, &isPlayOnAwake, sizeof isPlayOnAwake, &dwByte, nullptr);
	ReadFile(hFile, &strEmmiterVelocity, sizeof strEmmiterVelocity, &dwByte, nullptr);
	ReadFile(hFile, &iMaxParticles, sizeof iMaxParticles, &dwByte, nullptr);
	ReadFile(hFile, &isAutoRandomSeed, sizeof isAutoRandomSeed, &dwByte, nullptr);
	ReadFile(hFile, &strStopAction, sizeof strStopAction, &dwByte, nullptr);

	CloseHandle(hFile);

	return S_OK;
}