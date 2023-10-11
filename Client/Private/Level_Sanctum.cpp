#include "Level_Sanctum.h"
#include"GameInstance.h"

#include"MapObject.h"
#include"MapObject_Ins.h"
#include"Player.h"

CLevel_Sanctum::CLevel_Sanctum(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Sanctum::Initialize()
{
	CGameInstance::GetInstance()->Clear_Lights();

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("Failed to Ready_Lights in Level_Sanctum");

		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		MSG_BOX("Failed to Ready_Layer_BackGround in Level_Sanctum");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData4.ddd"))))
	{
		MSG_BOX("Failed Load Map Object Level_Sanctum");

		return E_FAIL;
	}

	//if (FAILED(Load_MapObject_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins2.ddd"))))
	//{
	//	MSG_BOX("Failed Load Map Object_Ins");
	//
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Monsters(TEXT("Layer_Monster"))))
	{
		MSG_BOX("Failed to Ready_Layer_Monsters in Level_Sanctum");

		return E_FAIL;
	}


#ifdef _DEBUG
	if (FAILED(Ready_Debug(TEXT("Layer_Debug"))))
	{
		MSG_BOX("Failed Load Layer_Debug");

		return E_FAIL;
	}
#endif // _DEBUG


	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), TEXT("Layer_Magic"))))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), TEXT("Layer_Particle"))))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);

	ENDINSTANCE;


	return S_OK;
}

void CLevel_Sanctum::Tick(_float fTimeDelta)
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
	SetWindowText(g_hWnd, TEXT("Sanctum"));
#endif //_DEBUG
}

HRESULT CLevel_Sanctum::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Sanctum::Ready_Layer_Player(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SANCTUM, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sanctum::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SANCTUM, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Sanctum::Ready_Layer_Monsters(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_float4x4 Matrix = XMMatrixTranslation(-2.f, -22.f, 130.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_ConjuredDragon"), pLayerTag, TEXT("GameObject_ConjuredDragon"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_ConjuredDragon)");
		ENDINSTANCE;
		return E_FAIL;
	}
	
	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sanctum::Ready_Lights()
{
	BEGININSTANCE;
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPos = _float4(10.f, 100.f, 25.f, 1.f);
	LightDesc.vDir = _float4(0.33f, -0.99f, 0.33f, 0.f);

	LightDesc.vDiffuse = WHITEDEFAULT;
	LightDesc.vAmbient = WHITEDEFAULT;
	LightDesc.vSpecular = WHITEDEFAULT;

	if (FAILED(pGameInstance->Add_Light(LightDesc, nullptr, true)))
		return E_FAIL;

	ENDINSTANCE;
	return S_OK;
}

HRESULT CLevel_Sanctum::Load_MapObject(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	_uint iObjectNum = 0;

	DWORD    dwByte = 0;

	while (true)
	{
		CMapObject::MAPOBJECTDESC MapObjectDesc;

		if (!ReadFile(hFile, &MapObjectDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen");
		}

		if (!ReadFile(hFile, &MapObjectDesc.wszTag, MapObjectDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}
		BEGININSTANCE;

		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

		if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectDesc)))
		{
			MSG_BOX("Failed to Clone MapObject");
			ENDINSTANCE;
			return E_FAIL;
		}

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Sanctum::Load_MapObject_Ins(const _tchar* pObjectFilePath)
{
	return E_NOTIMPL;
}

#ifdef _DEBUG
HRESULT CLevel_Sanctum::Ready_Debug(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SANCTUM, TEXT("Prototype_GameObject_Camera_Debug"), pLayerTag, TEXT("GameObject_Camera_Debug"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Camera_Debug)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}
#endif // _DEBUG

CLevel_Sanctum* CLevel_Sanctum::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Sanctum* pInstance = New CLevel_Sanctum(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Sanctum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Sanctum::Free()
{
	__super::Free();
}
