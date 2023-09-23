#include "..\Public\Level_Smith.h"
#include "GameInstance.h"

CLevel_Smith::CLevel_Smith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Smith::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Smith::Tick(_float fTimeDelta)
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

HRESULT CLevel_Smith::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Smith* CLevel_Smith::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Smith* pInstance = New CLevel_Smith(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Smith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Smith::Free()
{
	__super::Free();
}
