#include "..\Public\MainModule.h"

CMainModule::CMainModule()
	: CParticleModule()
{
}

CMainModule::CMainModule(const CMainModule& _rhs)
	: CParticleModule(_rhs)
	, m_MainModuleDesc(_rhs.m_MainModuleDesc)
{
}
 // ../../Resources/Effects/Particles/Particles/Fire
HRESULT CMainModule::Initialize_Prototype(const _tchar* _pFilePath)
{
	// ../../Resources/Effects/Particles/Particles/Fire/_pFilePath.ptc
	fs::path fsFilePath = _pFilePath;
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
	
	ReadFile(hFile, &m_MainModuleDesc, sizeof MAIN_MODULE, &dwByte, nullptr);

	CloseHandle(hFile);
	return S_OK;
}

CMainModule* CMainModule::Create(const _tchar* _pFilePath)
{
	CMainModule* pInstance = new CMainModule();

	if (FAILED(pInstance->Initialize_Prototype(_pFilePath)))
	{
		// 일부러 안넣었음.
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainModule::Free()
{
	__super::Free();
}