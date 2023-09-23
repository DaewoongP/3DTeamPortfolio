#include "..\Public\Level_GreatHall.h"
#include "GameInstance.h"

CLevel_GreatHall::CLevel_GreatHall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GreatHall::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	return S_OK;
}

void CLevel_GreatHall::Tick(_float fTimeDelta)
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

HRESULT CLevel_GreatHall::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_GreatHall* CLevel_GreatHall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GreatHall* pInstance = New CLevel_GreatHall(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_GreatHall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GreatHall::Free()
{
	__super::Free();
}
