#include "..\Public\Level_Loading.h"
#include "GameInstance.h"
#include "Main0_Loader.h"
#include "Main1_Loader.h"
#include "Main2_Loader.h"
#include "Main3_Loader.h"

#include "Level_Logo.h"
#include "Level_CliffSide.h"
#include "Level_Vault.h"
#include "Level_GreatHall.h"
#include "Level_Smith.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID, _bool isStaticLoaded)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 현재 씬 설정.
	pGameInstance->Set_CurrentScene(TEXT("Scene_Loading"), true);
	Safe_Release(pGameInstance);

	m_eNextLevelID = eNextLevelID;

	m_isLoadingLevel = true;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		if (FAILED(Loading_Logo(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Logo Object");
			return E_FAIL;
		}
		break;
	case LEVEL_CLIFFSIDE:
		if (FAILED(Loading_Cliffside(TEXT("Layer_CliffSide"))))
		{
			MSG_BOX("Failed Loading MainGame Object");
			return E_FAIL;
		}
		break;
	case LEVEL_VAULT:
		if (FAILED(Loading_Vault(TEXT("Layer_Vault"))))
		{
			MSG_BOX("Failed Loading MainGame Object");
			return E_FAIL;
		}
		break;
	case LEVEL_GREATHALL:
		if (FAILED(Loading_GreatHall(TEXT("Layer_GreatHall"))))
		{
			MSG_BOX("Failed Loading MainGame Object");
			return E_FAIL;
		}
		break;
	case LEVEL_SMITH:
		if (FAILED(Loading_Hogsmeade(TEXT("Layer_Hogsmeade"))))
		{
			MSG_BOX("Failed Loading MainGame Object");
			return E_FAIL;
		}
		break;
	default:
		MSG_BOX("Failed Loading Objects");
		break;
	}

	m_pMain0_Loader = CMain0_Loader::Create(m_pDevice, m_pContext, eNextLevelID, isStaticLoaded);
	m_pMain1_Loader = CMain1_Loader::Create(m_pDevice, m_pContext, eNextLevelID, isStaticLoaded);
	m_pMain2_Loader = CMain2_Loader::Create(m_pDevice, m_pContext, eNextLevelID, isStaticLoaded);
	m_pMain3_Loader = CMain3_Loader::Create(m_pDevice, m_pContext, eNextLevelID, isStaticLoaded);

	if(nullptr == m_pMain0_Loader || 
		nullptr == m_pMain1_Loader || 
		nullptr == m_pMain2_Loader || 
		nullptr == m_pMain3_Loader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr == m_pMain0_Loader ||
		nullptr == m_pMain1_Loader ||
		nullptr == m_pMain2_Loader ||
		nullptr == m_pMain3_Loader)
		return;

#ifdef _DEBUG
	Render_Finished();
#endif // _DEBUG

	if (false == Check_Thread_ExitCode(m_pMain0_Loader->Get_Thread()) ||
		false == Check_Thread_ExitCode(m_pMain1_Loader->Get_Thread()) ||
		false == Check_Thread_ExitCode(m_pMain2_Loader->Get_Thread()) || 
		false == Check_Thread_ExitCode(m_pMain3_Loader->Get_Thread()))
	{
		MSG_BOX("Loading Failed");
		Safe_Release(m_pMain0_Loader);
		Safe_Release(m_pMain1_Loader);
		Safe_Release(m_pMain2_Loader);
		Safe_Release(m_pMain3_Loader);
		PostQuitMessage(0);
		return;
	}

	// 로딩완료 체크
	if (false == m_pMain0_Loader->Get_Finished() ||
		false == m_pMain1_Loader->Get_Finished() ||
		false == m_pMain2_Loader->Get_Finished() ||
		false == m_pMain3_Loader->Get_Finished())
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Clear_Resources();

	CLevel* pLevel = { nullptr };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
		break;
	case LEVEL_CLIFFSIDE:
		pLevel = CLevel_Cliffside::Create(m_pDevice, m_pContext);
		break;
	case LEVEL_VAULT:
		pLevel = CLevel_Vault::Create(m_pDevice, m_pContext);
		break;
	case LEVEL_GREATHALL:
		pLevel = CLevel_GreatHall::Create(m_pDevice, m_pContext);
		break;
	case LEVEL_SMITH:
		pLevel = CLevel_Smith::Create(m_pDevice, m_pContext);
		break;
	default:
		MSG_BOX("Failed Create Next Level");
		__debugbreak();
		break;
	}

	if (nullptr == pLevel)
	{
		Safe_Release(pGameInstance);
		return;
	}

	if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
	{
		MSG_BOX("Failed open Next Level");
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, m_szFinishedLoader.c_str());

#endif //_DEBUG
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Logo(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 로고 이동전 로딩씬에 대한 객체 생성


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Cliffside(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 로고 이동전 로딩씬에 대한 객체 생성

	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Loading"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Loading)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_tchar wszFilePath[MAX_PATH] = TEXT("");
	lstrcpy(wszFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Loading1.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Group_Loading"),
		pLayerTag, TEXT("Prototype_GameObject_UI_Group_Loading1"), wszFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_UI_Group_Loading)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Vault(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 로고 이동전 로딩씬에 대한 객체 생성

	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Loading"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Loading)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_tchar wszFilePath[MAX_PATH] = TEXT("");
	lstrcpy(wszFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Loading2.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_LOADING, TEXT("Prototype_GameObject_UI_Group_Loading"),
		pLayerTag, TEXT("Prototype_GameObject_UI_Group_Loading1"), wszFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_UI_Group_Loading)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Loading_GreatHall(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Hogsmeade(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	// 로고 이동전 로딩씬에 대한 객체 생성

	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Loading"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Loading)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

_bool CLevel_Loading::Check_Thread_ExitCode(HANDLE hWnd)
{
	_ulong dwData = { 0 };

	if (TRUE == GetExitCodeThread(hWnd, &dwData))
	{
		if (-1 == dwData)
		{
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG
void CLevel_Loading::Render_Finished()
{
	m_szFinishedLoader = TEXT("");
	if (true == m_pMain0_Loader->Get_Finished())
	{
		m_szFinishedLoader += TEXT("0");
	}
	if (true == m_pMain1_Loader->Get_Finished())
	{
		m_szFinishedLoader += TEXT("1");
	}
	if (true == m_pMain2_Loader->Get_Finished())
	{
		m_szFinishedLoader += TEXT("2");
	}
	if (true == m_pMain3_Loader->Get_Finished())
	{
		m_szFinishedLoader += TEXT("3");
	}

	if (true == m_pMain0_Loader->Get_Finished() &&
		true == m_pMain1_Loader->Get_Finished() &&
		true == m_pMain2_Loader->Get_Finished() &&
		true == m_pMain3_Loader->Get_Finished())
	{
		m_szFinishedLoader = TEXT("로딩 완료");
	}
}
#endif // _DEBUG

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID, _bool isStaticLoaded)
{
	CLevel_Loading* pInstance = New CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID, isStaticLoaded)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pMain0_Loader);
	Safe_Release(m_pMain1_Loader);
	Safe_Release(m_pMain2_Loader);
	Safe_Release(m_pMain3_Loader);
}
