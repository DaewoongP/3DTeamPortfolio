#include "..\Public\Level_MainGame.h"
#include "GameInstance.h"
#include "Seamless_Loader.h"
#include "MapObject.h"
#include "UI_Group_Enemy_HP.h"

CLevel_MainGame::CLevel_MainGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MainGame::Initialize()
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
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
	{
		MSG_BOX("Failed Ready_Layer_Monster");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	{
		MSG_BOX("Failed Ready_Layer_NPC");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	{
		MSG_BOX("Failed Ready_Layer_BackGround");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	{
		MSG_BOX("Failed Ready_Layer_UI");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Info_UI(TEXT("Layer_Info_UI"))))
	{
		MSG_BOX("Failed Ready_Layer_UI");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		MSG_BOX("Failed Ready_Layer_Effect");

		return E_FAIL;
	}

	/*if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData6.ddd"))))
	{
		MSG_BOX("Failed Load Map Object");

		return E_FAIL;
	}*/
	
#ifdef _DEBUG
	if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
	{
		MSG_BOX("Failed Ready_Layer_Debug");

		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SceneTest(TEXT("Layer_SceneTest"))))
	{
		MSG_BOX("Failed Ready_Layer_Debug");

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

	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	ENDINSTANCE;

	return S_OK;
}

void CLevel_MainGame::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_T, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	}
	if (pGameInstance->Get_DIKeyState(DIK_Y, CInput_Device::KEY_DOWN))
	{
		pGameInstance->Set_CurrentScene(TEXT("Scene_Info"), false);
	}
	
	ENDINSTANCE;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임 레벨"));
#endif //_DEBUG
}

HRESULT CLevel_MainGame::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Lights()
{
	BEGININSTANCE;

	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vDir = _float4(0.33f, -0.99f, 0.33f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (nullptr == pGameInstance->Add_Lights(m_pDevice, m_pContext, LightDesc))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_BackGround(const _tchar* pLayerTag)
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

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Terrain)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_Player(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_CombatGrunt"), pLayerTag, TEXT("GameObject_Golem_Combat"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Combat)");
		ENDINSTANCE;
		return E_FAIL;
	}

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_MerlinGrunt"), pLayerTag, TEXT("GameObject_Golem_Merlin"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Merlin)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Professor_Fig"), pLayerTag, TEXT("GameObject_Professor_Fig"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Professor_Fig)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_MainGame::Load_MapObject(const _tchar* pObjectFilePath)
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

		if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectDesc)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return E_FAIL;
		}

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_MainGame::Load_Monsters(const wstring& wstrMonsterFilePath)
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
		_tchar szTag[MAX_PATH] = { TEXT("") };
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrMonsterTag = szTag;

		/* Read Monster PrototypeModelTag */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(szTag);
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrPrototypeModelTag = szTag;

		/* Read Monster ModelFilePath */
		iLength = { 0 };
		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iLength");
			CloseHandle(hFile);
		}
		ZEROMEM(szTag);
		if (!ReadFile(hFile, szTag, sizeof(_tchar) * iLength, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelFilePath = szTag;

		if (dwByte == 0)
		{
			break;
		}

		wstring wstrPrototypeTag = TEXT("Prototype_GameObject_");
		wstrPrototypeTag += wstrMonsterTag;
		wstring wstrComponentTag = TEXT("Monster_") + to_wstring(iMonsterNum++);
		BEGININSTANCE;
		if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, wstrPrototypeTag.c_str(),
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

HRESULT CLevel_MainGame::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#ifdef _DEBUG
	/*if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Particle"), pLayerTag, TEXT("GameObject_Test_Particle"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Particle)");
		return E_FAIL;
	}*/

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Default_Magic_Effect"), pLayerTag, TEXT("GameObject_Default_Magic_Effect"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Default_Magic_Effect)");
		return E_FAIL;
	}*/

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Wingardium_Effect"), pLayerTag, TEXT("GameObject_Wingardium_Effect"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Wingardium_Effect)");
		return E_FAIL;
	}*/
#endif _DEBUG
	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CLevel_MainGame::Ready_Layer_UI(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");

	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_HP.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_HP"),
		pLayerTag, TEXT("GameObject_UI_Group_HP"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_HP)");
		ENDINSTANCE;
		return E_FAIL;
	}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Potion.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Potion"),
		pLayerTag, TEXT("GameObject_UI_Group_Potion"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Potion)");
		ENDINSTANCE;
		return E_FAIL;
	}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Front.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Finisher"),
		pLayerTag, TEXT("GameObject_UI_Group_Finisher"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Finisher)");
		ENDINSTANCE;
		return E_FAIL;
	}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Skill_1.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Skill"),
		pLayerTag, TEXT("Prototype_GameObject_UI_Group_Skill"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_UI_Group_Skill)");
		return E_FAIL;
	}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Icon.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
		pLayerTag, TEXT("GameObject_UI_Group_Finisher_Icon"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Finisher_Icon)");
		return E_FAIL;
	}
	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_MiniMap.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_MiniMap"),
	//	pLayerTag, TEXT("GameObject_UI_Group_MiniMap"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_MiniMap)");
	//	return E_FAIL;
	//}

	CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;
	_tchar szLevel[MAX_PATH] = TEXT("77");
	_tchar szName[MAX_PATH] = TEXT("개철민");

	Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::MONSTER;
	lstrcpy(Desc.wszObjectLevel, szLevel);
	lstrcpy(Desc.wszObjectName, szName);

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"),
		pLayerTag, TEXT("GameObject_UI_Enemy_HP"), &Desc)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Enemy_HP)");
		return E_FAIL;
	}

	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Cursor"),
		pLayerTag, TEXT("GameObject_UI_Group_Cursor"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Cursor)");
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_Info_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Info"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Info)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");

	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Gear5.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Info_Main"),
		pLayerTag, TEXT("Prototype_GameObject_Info_Main"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Info_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_MainGame::Ready_Layer_Debug(const _tchar* pLayerTag)
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

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Debug"), pLayerTag, TEXT("GameObject_Camera_Debug"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Camera_Debug)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Player"), pLayerTag, TEXT("GameObject_Test_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Player)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_LoadTrigger"), pLayerTag, TEXT("GameObject_LoadTrigger"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_LoadTrigger)");
		return E_FAIL;
	}

	// 키지마세요 터집니다
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_PhysxRenderer"), pLayerTag, TEXT("GameObject_PhysxRenderer"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_PhysxRenderer)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_SceneTest(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Info"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}



	Safe_Release(pGameInstance);

	return S_OK;
}
#endif // _DEBUG

CLevel_MainGame* CLevel_MainGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MainGame* pInstance = New CLevel_MainGame(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_MainGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_MainGame::Free()
{
	__super::Free();
}
