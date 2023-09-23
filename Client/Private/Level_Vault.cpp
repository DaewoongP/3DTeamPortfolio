#include "..\Public\Level_Vault.h"
#include "GameInstance.h"

CLevel_Vault::CLevel_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Vault::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;

    return S_OK;
}

void CLevel_Vault::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_T, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	}
	if (pGameInstance->Get_DIKeyState(DIK_Y, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Set_CurrentScene(TEXT("Scene_FieldGuide"), false);
	}

	ENDINSTANCE;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Vault"));
#endif //_DEBUG
}

HRESULT CLevel_Vault::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Vault* CLevel_Vault::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Vault* pInstance = New CLevel_Vault(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Vault");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Vault::Free()
{
	__super::Free();
}
