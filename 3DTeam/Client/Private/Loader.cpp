#include "..\Public\Loader.h"
#include "GameInstance.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint WINAPI Thread_Main(void* pArg)
{
	CLoader* pLoader = reinterpret_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 0;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// ũ��Ƽ�� ���� ���� �ʱ�ȭ
	InitializeCriticalSection(&m_Critical_Section);

	// ������ ���� �Լ� ȣ��
	// 3��° ���ڷ� ������ �Լ������� ����.
	// 4��° ���ڷ� ������ �Լ��� �Ű������� �־��� �� ����.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLoader::Loading()
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	// ũ��Ƽ�� ���� �����ؼ� �ٸ� �����尡 �� �ȿ� �ִ� ���� ���� �������� ���ϰ� ó��.
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVELID::LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("�ؽ��� �ε� ��."));


	lstrcpy(m_szLoading, TEXT("�� �ε� ��."));
	

	lstrcpy(m_szLoading, TEXT("���̴� �ε� ��."));


	lstrcpy(m_szLoading, TEXT("��ü �ε� ��."));


	lstrcpy(m_szLoading, TEXT("�ε� �Ϸ�."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	// �ε��� ���������� ��ٷ��� �ϹǷ� infinite �ɼ��� �־� �ε��� ���������� ������ ���.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
