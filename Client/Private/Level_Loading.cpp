#include "..\Public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_MainGame.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		if (FAILED(Loading_Logo(TEXT("Layer_Loading"))))
		{
			MSG_BOX("Failed Loading Logo Object");
			return E_FAIL;
		}
		break;
	case LEVEL_MAINGAME:
		if (FAILED(Loading_MainGame(TEXT("Layer_MainGame"))))
		{
			MSG_BOX("Failed Loading MainGame Object");
			return E_FAIL;
		}
		break;
	default:
		MSG_BOX("Failed Loading Objects");
		break;
	}

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);

	if(nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		// 로딩완료 체크
 		if (false == m_pLoader->Get_Finished())
			return;

		CLevel* pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MAINGAME:
			pLevel = CLevel_MainGame::Create(m_pDevice, m_pContext);
			break;
		default:
			MSG_BOX("Failed Create Next Level");
			break;
		}

		if (nullptr == pLevel)
			return;

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
		{
			MSG_BOX("Failed open Next Level");
			Safe_Release(pGameInstance);
			return;
		}
		
		Safe_Release(pGameInstance);
		return;
	}
}

HRESULT CLevel_Loading::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());
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

HRESULT CLevel_Loading::Loading_MainGame(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
