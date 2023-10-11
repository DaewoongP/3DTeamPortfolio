#include "..\Public\Level_Sky.h"
#include "GameInstance.h"

#include "MapObject.h"
#include "MapObject_Ins.h"

CLevel_Sky::CLevel_Sky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}
HRESULT CLevel_Sky::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("Failed to Ready_Lights in Level_Sky");

		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
	{
		MSG_BOX("Failed to Ready_Layer_Player in Level_Sky");

		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		MSG_BOX("Failed to Ready_Layer_BackGround in Level_Sky");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData5.ddd"))))
	{
		MSG_BOX("Failed to Load Map Object in Level_Sky");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins5.ddd"))))
	{
		MSG_BOX("Failed to Load Map Object_Ins in Level_Sky");

		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Ready_Debug(TEXT("Layer_Debug"))))
	{
		MSG_BOX("Failed to Load Layer_Debug in Level_Sky");

		return E_FAIL;
	}
#endif // _DEBUG

	BEGININSTANCE;

	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);

	ENDINSTANCE;

	return S_OK;
}

void CLevel_Sky::Tick(_float fTimeDelta)
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
	SetWindowText(g_hWnd, TEXT("Sky"));
#endif //_DEBUG
}

HRESULT CLevel_Sky::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Layer_Player(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main) in Level_Sky");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SKY, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player) in Level_Sky");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main) in Level_Sky");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SKY, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky) in Level_Sky");
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Lights()
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

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Load_MapObject(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject in Level_Sky");
		return E_FAIL;
	}

	_uint iObjectNum = 0;

	DWORD    dwByte = 0;

	while (true)
	{
		CMapObject::MAPOBJECTDESC MapObjectDesc;

		if (!ReadFile(hFile, &MapObjectDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos in Level_Sky");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen in Level_Sky");
		}

		if (!ReadFile(hFile, &MapObjectDesc.wszTag, MapObjectDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag in Level_Sky");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}
		BEGININSTANCE;

		// �Ϲ� �ʿ�����Ʈ
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

		if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectDesc)))
		{
			MSG_BOX("Failed to Clone MapObject in Level_Sky");
			ENDINSTANCE;
			return E_FAIL;
		}

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Sky::Load_MapObject_Ins(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins in Level_Sky");
		return E_FAIL;
	}

	DWORD	dwByte = 0;
	_uint	iCount = 0;

	while (true)
	{
		CMapObject_Ins::MAPOJBECTINSDESC MapObjectInsDesc;
		ZEROMEM(&MapObjectInsDesc);

		if (!ReadFile(hFile, &MapObjectInsDesc.iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iInstanceCnt in Level_Sky");
			return E_FAIL;
		}

		if (0 != MapObjectInsDesc.iInstanceCnt)
		{
			_float4x4 Matrix;

			for (size_t i = 0; i < MapObjectInsDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &Matrix, sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read MapObject_Ins pMatTransform in Level_Sky");
					return E_FAIL;
				}
			}
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins matTransform in Level_Sky");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iTagLen in Level_Sky");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.wszTag, MapObjectInsDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins wszTag in Level_Sky");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		BEGININSTANCE;

		wstring ws = TEXT("Prototype_Component_Model_Instance_");
		wstring wsTag = TEXT("Prototype_Component_Model_");
		wstring wsSave(MapObjectInsDesc.wszTag);
		_uint iLength = wsTag.size();

		wstring wsModelName = wsSave.substr(iLength);
		ws += wsModelName;

		ws += TEXT("_");

		_tchar wszNumber[MAX_PATH];
		_itow_s(iCount, wszNumber, 10);

		ws += wszNumber;

		lstrcpy(MapObjectInsDesc.wszTag, ws.c_str());
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (iCount));

		if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY,
			TEXT("Prototype_GameObject_MapObject_Ins"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectInsDesc)))
		{
			MSG_BOX("Failed to Install MapObject_Ins in Level_Sky");
			return E_FAIL;
		}

		++iCount; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Sky::Ready_Debug(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main) in Level_Smith");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SKY, TEXT("Prototype_GameObject_Camera_Debug"), pLayerTag, TEXT("GameObject_Camera_Debug"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Camera_Debug) in Level_Smith");
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}
#endif // _DEBUG

CLevel_Sky* CLevel_Sky::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Sky* pInstance = New CLevel_Sky(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Sky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Sky::Free()
{
	__super::Free();
}