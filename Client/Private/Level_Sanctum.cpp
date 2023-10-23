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
	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Monsters(TEXT("Layer_Monster")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Shader(), E_FAIL);
	
	BEGININSTANCE;
	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	ENDINSTANCE;

	return S_OK;
}

void CLevel_Sanctum::Tick(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_ESCAPE, CInput_Device::KEY_DOWN))
	{
		if (!lstrcmp(TEXT("Scene_Main"), pGameInstance->Get_CurrentSceneTag()))
		{
			pGameInstance->Set_CurrentScene(TEXT("Scene_FieldGuide"), false);
		}
		else
		{
			pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
		}
	}

	ENDINSTANCE;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Sanctum"));
#endif //_DEBUG
}

HRESULT CLevel_Sanctum::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SANCTUM, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky)");
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

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Sanctum::Ready_Layer_Monsters(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	_float4x4 Matrix = XMMatrixTranslation(-34.f, -33.f, 129.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_ConjuredDragon"), pLayerTag, TEXT("GameObject_ConjuredDragon"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_ConjuredDragon)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(41.f)) * XMMatrixTranslation(-8.1f, 2.2f, 173.7f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_DarkWizard_Spawn"), pLayerTag, TEXT("Spawn_0"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Spawn_0)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(-63.f)) * XMMatrixTranslation(5.6f, 1.2f, 176.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_DarkWizard_Spawn"), pLayerTag, TEXT("Spawn_1"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Spawn_1)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(23.f)) * XMMatrixTranslation(-4.75f, 2.45f, 171.3f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_DarkWizard_Spawn"), pLayerTag, TEXT("Spawn_2"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Spawn_2)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(-27.f)) * XMMatrixTranslation(2.3f, 3.2f, 171.1f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, TEXT("Prototype_GameObject_DarkWizard_Spawn"), pLayerTag, TEXT("Spawn_3"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Spawn_3)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Load_Monsters(TEXT("../../Resources/GameData/MonsterData/Sanctum.mon"));

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

HRESULT CLevel_Sanctum::Ready_Shader()
{
	BEGININSTANCE;

	CRenderer* pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")));
	pRenderer->Defualt_Shading();



	Safe_Release(pRenderer);

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

HRESULT CLevel_Sanctum::Load_Monsters(const wstring& wstrMonsterFilePath)
{
	HANDLE hFile = CreateFile(wstrMonsterFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	_uint iMonsterNum = 0;
	_ulong dwByte = { 0 };

	while (true)
	{
		_float4x4 WorldMatrix = _float4x4();
		wstring wstrMonsterTag = { TEXT("") };
		string strComponentTag = { "" };
		wstring wstrModelFilePath = { TEXT("") };
		wstring wstrPrototypeModelTag = { TEXT("") };

		/* Read Monster WorldMatrix */
		if (!ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read WorldMatrix");
			CloseHandle(hFile);
			return E_FAIL;
		}

		/* Read Monster Tag */
		_uint iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read iLength");
			CloseHandle(hFile);
		}
		_tchar wszTag[MAX_PATH] = { TEXT("") };
		if (!ReadFile(hFile, wszTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrMonsterTag = wszTag;

		/* Read Monster ComponentTag */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read wstrComponentTag.iTagLen");
			return E_FAIL;
		}
		_char szTag[MAX_PATH] = { "" };
		if (!ReadFile(hFile, szTag, iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read wstrComponentTag.wszTag");
			return E_FAIL;
		}
		strComponentTag = szTag;

		/* Read Monster PrototypeModelTag */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(wszTag);
		if (!ReadFile(hFile, wszTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrPrototypeModelTag = wszTag;

		/* Read Monster ModelFilePath */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(wszTag);
		if (!ReadFile(hFile, wszTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelFilePath = wszTag;

		if (dwByte == 0)
		{
			break;
		}

		wstring wstrPrototypeTag = TEXT("Prototype_GameObject_");
		wstrPrototypeTag += wstrMonsterTag;
		wstring wstrComponentTag = strToWStr(strComponentTag);

		BEGININSTANCE;
		if (FAILED(pGameInstance->Add_Component(LEVEL_SANCTUM, LEVEL_SANCTUM, wstrPrototypeTag.c_str(),
			TEXT("Layer_Monster"), wstrComponentTag.c_str(), &WorldMatrix)))
		{
			MSG_BOX("[Load_Monsters] Failed Load Monster");
			ENDINSTANCE;
			return E_FAIL;
		}
		ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

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
