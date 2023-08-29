#include "..\Private\MainModule.h"

CMainModule::CMainModule()
	: CParticleModule()
{
}

CMainModule::CMainModule(const CMainModule& _rhs)
	: CParticleModule(_rhs)
	, m_fDuration(_rhs.m_fDuration)
	, m_isLooping(_rhs.m_isLooping)
	, m_isPrewarm(_rhs.m_isPrewarm)
	, m_fStartDelay(_rhs.m_fStartDelay)
	, m_fStartLifeTime(_rhs.m_fStartLifeTime)
	, m_fStartSpeed(_rhs.m_fStartSpeed)
{
}
 // ../../Resources/Effects/Particles/Particles/Fire
HRESULT CMainModule::Initialize_Prototype(const _tchar* _pFilePath)
{
	// ../../Resources/Effects/Particles/Particles/Fire/_pFilePath.ptc
	fs::path fsFilePath = _pFilePath;
	fsFilePath = fsFilePath / TEXT("MainModule");

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
	
	ReadFile(hFile, &m_fDuration, sizeof m_fDuration, &dwByte, nullptr);
	ReadFile(hFile, &m_isLooping, sizeof m_isLooping, &dwByte, nullptr);
	ReadFile(hFile, &m_fStartDelay, sizeof m_fStartDelay, &dwByte, nullptr);
	ReadFile(hFile, &m_fStartLifeTime, sizeof m_fStartLifeTime, &dwByte, nullptr);
	ReadFile(hFile, &m_fStartSpeed, sizeof m_fStartSpeed, &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}

CMainModule* CMainModule::Create(const _tchar* _pFilePath)
{
	CMainModule* pInstance = new CMainModule();

	if (FAILED(pInstance->Initialize_Prototype(_pFilePath)))
	{
		MSG_BOX("Failed to Created CMainModule");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainModule::Free()
{
	__super::Free();
}