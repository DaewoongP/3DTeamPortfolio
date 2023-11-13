#include "..\Public\Level_Logo.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	// 현재 씬 설정.
	pGameInstance->Set_CurrentScene(TEXT("Scene_Logo"), true);
	FAILED_CHECK_RETURN(pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/BGM/")), E_FAIL);
	m_iChennelNum = pGameInstance->Play_Sound(TEXT("Intro.wav"), 0.6f);
	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Logo::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 스테이지 이동
	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_SMITH, false))))
		{
			MSG_BOX("Failed Open LEVEL_LOGO to LEVEL_MAIN");
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif //_DEBUG
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Logo"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Logo)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_LOGO, LEVEL_LOGO, TEXT("Prototype_GameObject_UI_Group_Logo"),
		pLayerTag, TEXT("Prototype_GameObject_UI_Group_Logo"))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_UI_Group_Logo)");
		__debugbreak();
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo* pInstance = New CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
