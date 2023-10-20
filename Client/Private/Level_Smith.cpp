#include "..\Public\Level_Smith.h"
#include "GameInstance.h"

#include "MapObject.h"
#include "MapObject_Ins.h"
#include "Level_Loading.h"
#include "Trigger.h"
#include "Dummy_NPC.h"

#include "Cat.h"

CLevel_Smith::CLevel_Smith(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Smith::Initialize()
{
	std::lock_guard<std::mutex> lock(mtx);
	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Monsters(TEXT("Layer_Monster")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_NPC(TEXT("Layer_NPC")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_UI(TEXT("Layer_UI")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Event(TEXT("Layer_Event")), E_FAIL);

	BEGININSTANCE;
	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	ENDINSTANCE;

	return S_OK;
}

void CLevel_Smith::Tick(_float fTimeDelta)
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

	if (pGameInstance->Get_DIKeyState(DIK_F12, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CLIFFSIDE));
	}
	if (pGameInstance->Get_DIKeyState(DIK_F10, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VAULT));
	}
	if (pGameInstance->Get_DIKeyState(DIK_F11, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SANCTUM));
	}

	ENDINSTANCE;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Hogsmeade"));
#endif //_DEBUG
}

HRESULT CLevel_Smith::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SMITH, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky) in Level_Smith");
		return E_FAIL;
	}

	if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData3.ddd"))))
	{
		MSG_BOX("Failed to Load Map Object in Level_Smith");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins3.ddd"))))
	{
		MSG_BOX("Failed to Load Map Object_Ins in Level_Smith");

		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Smith::Ready_Layer_Monsters(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 Matrix = XMMatrixTranslation(86.f, 10.f, 129.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Armored_Troll"), pLayerTag, TEXT("GameObject_Armored_Troll"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Armored_Troll)");
		ENDINSTANCE;
		return E_FAIL;
	}
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Smith::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	_float4x4 Matrix = XMMatrixTranslation(93.4f, 6.25f, 80.7f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Vendor"), pLayerTag, TEXT("GameObject_Vendor"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Vendor)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(97.4f, 6.3f, 80.7f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Oakes"), pLayerTag, TEXT("GameObject_Oakes"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Oakes)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(93.4f, 6.3f, 82.7f);
	CDummy_NPC::NPCINITDESC InitDesc;
	InitDesc.WorldMatrix = Matrix;
	InitDesc.wstrAnimationTag = TEXT("Idle");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Arcane");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_M_C");
	InitDesc.MeshPartsTags[2] = TEXT("Prototype_Component_MeshPart_Head_NPC_M");
	InitDesc.MeshPartsTags[4] = TEXT("Prototype_Component_MeshPart_Player_Arm");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DarkArts");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Arcane_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	InitDesc.wstrCustomModelTag = TEXT("Prototype_Component_Model_CustomModel_NPC_M");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("GameObject_Dummy_NPC"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Dummy_NPC)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Smith::Ready_Lights()
{
	BEGININSTANCE;
	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPos = _float4(45.f, 54.7f, 112.5f, 1.f);
	LightDesc.vLookAt = _float4(82.8f, 0.f, 80.9f, 1.f);
	LightDesc.vDir = LightDesc.vLookAt - LightDesc.vPos;

	LightDesc.vDiffuse = WHITEDEFAULT;
	LightDesc.vAmbient = WHITEDEFAULT;
	LightDesc.vSpecular = WHITEDEFAULT;

	if (FAILED(pGameInstance->Add_Light(LightDesc, nullptr, true)))
		return E_FAIL;

	ENDINSTANCE;
	return S_OK;
}

HRESULT CLevel_Smith::Ready_UI(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	//if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	//{
	//	MSG_BOX("Failed Add Scene : (Scene_Main)");
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SMITH, TEXT("Prototype_GameObject_UI_Store"), pLayerTag, TEXT("GameObject_UI_Store"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Store)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Smith::Ready_Event(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Event_Smeade"), pLayerTag, TEXT("Event_Smeade"))))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Smeade)");
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Smith::Load_MapObject(const _tchar* pObjectFilePath)
{
	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject in Level_Smith");
		return E_FAIL;
	}

	_uint iObjectNum = 0;
	_uint iCatNum = 0;

	DWORD    dwByte = 0;

	while (true)
	{
		CMapObject::MAPOBJECTDESC MapObjectDesc;

		if (!ReadFile(hFile, &MapObjectDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos in Level_Smith");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen in Level_Smith");
		}

		if (!ReadFile(hFile, &MapObjectDesc.wszTag, MapObjectDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag in Level_Smith");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}
		BEGININSTANCE;

		wstring ws(MapObjectDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);

		// 비교해야되는 문자열
		wstring wsTreasureChestName(TEXT("Anim_TreasureChest"));
		wstring wsAshwinderEggs(TEXT("Anim_AshwinderEggs"));
		wstring wsLeapingToadStools(TEXT("Anim_LeapingToadStools"));
		wstring wsHorklump(TEXT("Anim_Horklump"));
		wstring wsLeech(TEXT("Anim_Leech"));
		wstring wsGreyCat(TEXT("Anim_GreyCat"));
		wstring wsPotionStation(TEXT("SM_HM_Potion_Table"));
		wstring wsShopDoor(TEXT("SM_HM_Shop_Door"));

		// 보물상자
		if (0 == lstrcmp(modelName.c_str(), wsTreasureChestName.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Treasure_Chest_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Treasure_Chest"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Treasure_Chest in Level_Smith");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 포션 제작소
		else if (0 == lstrcmp(modelName.c_str(), wsPotionStation.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Potion_Station_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Potion_Station"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Potion_Station in Level_Smith");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 채집물
		else if (0 == lstrcmp(modelName.c_str(), wsHorklump.c_str()) ||
				 0 == lstrcmp(modelName.c_str(), wsLeech.c_str()) || 
				 0 == lstrcmp(modelName.c_str(), wsAshwinderEggs.c_str()) ||
				 0 == lstrcmp(modelName.c_str(), wsLeapingToadStools.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Gatherer_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Gatherer"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Gatherer");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 문
		else if (0 == lstrcmp(modelName.c_str(), wsShopDoor.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Door_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Door"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Door in Level_Smith");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 고양이
		else if (0 == lstrcmp(modelName.c_str(), wsGreyCat.c_str()))
		{
			CCat::CATDESC CatDesc;
			CatDesc.iTagLen = MapObjectDesc.iTagLen;
			CatDesc.WorldMatrix = MapObjectDesc.WorldMatrix;
			lstrcpy(CatDesc.wszTag, MapObjectDesc.wszTag);
			CatDesc.iAnimIndex = iCatNum;

			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Cat_%d"), (iCatNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Cat"), TEXT("Layer_BackGround"),
				wszobjName, &CatDesc)))
			{
				MSG_BOX("Failed to Clone Cat in Level_Smith");
				ENDINSTANCE;
				return E_FAIL;
			}

			++iCatNum;
		}

		// 일반 맵오브젝트
		else
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone MapObject in Level_Smith");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Smith::Load_MapObject_Ins(const _tchar* pObjectFilePath)
{
	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins in Level_Smith");
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
			MSG_BOX("Failed to Read MapObject_Ins iInstanceCnt in Level_Smith");
			return E_FAIL;
		}

		if (0 != MapObjectInsDesc.iInstanceCnt)
		{
			_float4x4 Matrix;

			for (size_t i = 0; i < MapObjectInsDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &Matrix, sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read MapObject_Ins pMatTransform in Level_Smith");
					return E_FAIL;
				}
			}
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins matTransform in Level_Smith");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iTagLen in Level_Smith");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.wszTag, MapObjectInsDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins wszTag in Level_Smith");
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

		if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
			TEXT("Prototype_GameObject_MapObject_Ins"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectInsDesc)))
		{
			MSG_BOX("Failed to Install MapObject_Ins in Level_Smith");
			return E_FAIL;
		}

		++iCount; ENDINSTANCE;
	}

	CloseHandle(hFile);

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
