#include "..\Public\Seamless_Loader.h"
#include "GameInstance.h"

CSeamless_Loader::CSeamless_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint WINAPI Thread_Seamless(void* pArg)
{
	CSeamless_Loader* pLoader = reinterpret_cast<CSeamless_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CSeamless_Loader::Initialize()
{
	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);
	
	// 쓰레드 시작 함수 호출
	// 3번째 인자로 시작할 함수포인터 대입.
	// 4번째 인자로 시작할 함수의 매개변수로 넣어줄 값 대입.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Seamless, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSeamless_Loader::Loading()
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	// 크리티컬 섹션 시작해서 다른 쓰레드가 이 안에 있는 동안 값을 변경하지 못하게 처리.
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	hr = Ready_Layer_Debug(TEXT("Layer_Debug"));

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	m_isFinished = true;

	return S_OK;
}

HRESULT CSeamless_Loader::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	std::lock_guard<std::mutex> lock(mtx);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Dummy"), pLayerTag, TEXT("GameObject_Dummy"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Dummy)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Combat"), pLayerTag, TEXT("GameObject_Test_Monster"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Monster)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Player"), pLayerTag, TEXT("GameObject_Test_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Player)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Professor_Fig"), pLayerTag, TEXT("GameObject_Professor_Fig"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Professor_Fig)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CSeamless_Loader* CSeamless_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSeamless_Loader* pInstance = new CSeamless_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CSeamless_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSeamless_Loader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
