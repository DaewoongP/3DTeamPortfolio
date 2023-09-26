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
	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("Failed Ready_Lights");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		MSG_BOX("Failed Ready_Layer_Player");

		return E_FAIL;
	}
	
	BEGININSTANCE;

	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	ENDINSTANCE;

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

	if (false)//trigger on
	{
		Light_Out();
		
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

HRESULT CLevel_Vault::Light_Out()
{
	BEGININSTANCE
	CLight::LIGHTDESC SettingLight;
	ZEROMEM(&SettingLight);
	
	Color vColor = (XMVectorLerp(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f),1.f));

	SettingLight.vDiffuse = _float4(vColor.x,vColor.y, vColor.z, 0.f);
	SettingLight.vSpecular = _float4(vColor.x, vColor.y, vColor.z, 0.f);
	SettingLight.vAmbient = _float4(vColor.x, vColor.y, vColor.z, 0.f);
	pGameInstance->Set_Light(CLight::TYPE_DIRECTIONAL, SettingLight);

	ENDINSTANCE

		return S_OK;
}

HRESULT CLevel_Vault::Ready_Layer_Player(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_VAULT, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Vault::Ready_Lights()
{
	BEGININSTANCE;
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPos = _float4(2.f, 30.f, 2.f, 1.f);
	LightDesc.vLookAt = _float4(20.f, 0.f, 20.f, 1.f);
	LightDesc.vDir = _float4(0.33f, -0.99f, 0.33f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr == pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc))
		return E_FAIL;

	ENDINSTANCE;
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
