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

_uint WINAPI Thread_SeamlessTest(void* pArg)
{
	CSeamless_Loader* pLoader = reinterpret_cast<CSeamless_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CSeamless_Loader::Initialize()
{
	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_SeamlessTest, this, 0, nullptr);

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

	if (FAILED(m_pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Combat"), pLayerTag, TEXT("GameObject_Golem_Combat0"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Combat)");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Merlin"), pLayerTag, TEXT("GameObject_Golem_Merlin0"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Merlin)");
		return E_FAIL;
	}

	return S_OK;
}

CSeamless_Loader* CSeamless_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSeamless_Loader* pInstance = New CSeamless_Loader(pDevice, pContext);

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
