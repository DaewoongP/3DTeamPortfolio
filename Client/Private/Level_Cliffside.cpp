#include "..\Public\Level_Cliffside.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "MapObject.h"
#include "MapObject_Ins.h"
#include "UI_Group_Enemy_HP.h"

CLevel_Cliffside::CLevel_Cliffside(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Cliffside::Initialize()
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
	/*if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
	{
		MSG_BOX("Failed Ready_Layer_NPC");

		return E_FAIL;
	}*/
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
	if (FAILED(Ready_Layer_FieldGuide_UI(TEXT("Layer_FieldGuide_UI"))))
	{
		MSG_BOX("Failed Ready_Layer_FieldGuide_UI");

		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Menu_UI(TEXT("Layer_Menu_UI"))))
	{
		MSG_BOX("Failed Ready_Layer_Menu_UI");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData0.ddd"))))
	{
		MSG_BOX("Failed Load Map Object");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins0.ddd"))))
	{
		MSG_BOX("Failed Load Map Object_Ins");

		return E_FAIL;
	}
	
#ifdef _DEBUG
	if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
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

void CLevel_Cliffside::Tick(_float fTimeDelta)
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

	// 스테이지 이동
	if (pGameInstance->Get_DIKeyState(DIK_BACKSPACE, CInput_Device::KEY_DOWN))
	{
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID::LEVEL_VAULT))))
		{
			MSG_BOX("Failed Open LEVEL_CLIFFSIDE to LEVEL_VAULT");
			Safe_Release(pGameInstance);
			return;
		}
	}
	
	ENDINSTANCE;

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("Cliffside"));
#endif //_DEBUG
}

HRESULT CLevel_Cliffside::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Lights()
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

HRESULT CLevel_Cliffside::Ready_Layer_BackGround(const _tchar* pLayerTag)
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

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Sky)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Terrain"), pLayerTag, TEXT("GameObject_Terrain"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Terrain)");
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Layer_Player(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Player)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Golem_CombatGrunt"), pLayerTag, TEXT("GameObject_Golem_Combat"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Combat)");
		ENDINSTANCE;
		return E_FAIL;
	}

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Armored_Troll"), pLayerTag, TEXT("GameObject_Armored_Troll"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Armored_Troll)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Golem_MerlinGrunt"), pLayerTag, TEXT("GameObject_Golem_Merlin"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Golem_Merlin)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	/* Add Scene : Main */
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Professor_Fig"), pLayerTag, TEXT("GameObject_Professor_Fig"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Professor_Fig)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Cliffside::Load_MapObject(const _tchar* pObjectFilePath)
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

		wstring ws(MapObjectDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);
		wstring wsMapEffectName(TEXT("Cylinder_Long"));

		if (0 == lstrcmp(modelName.c_str(), wsMapEffectName.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_MapEffect_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE,
				TEXT("Prototype_GameObject_MapEffect"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone MapEffect");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		else
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE,
				TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone MapObject");
				ENDINSTANCE;
				return E_FAIL;
			}
		}		

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Cliffside::Load_MapObject_Ins(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;
	_uint	iCount = 0; // �� �ν��Ͻ� �ѹ��� ����

	while (true)
	{
		CMapObject_Ins::MAPOJBECTINSDESC MapObjectInsDesc;
		ZEROMEM(&MapObjectInsDesc);

		if (!ReadFile(hFile, &MapObjectInsDesc.iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iInstanceCnt");
			return E_FAIL;
		}

		if (0 != MapObjectInsDesc.iInstanceCnt)
		{
			_float4x4 Matrix; // ���� �Ҵ� ���ϰ� ���� ������ ����

			for (size_t i = 0; i < MapObjectInsDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &Matrix, sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read MapObject_Ins pMatTransform");
					return E_FAIL;
				}
			}
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins matTransform");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iTagLen");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &MapObjectInsDesc.wszTag, MapObjectInsDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		BEGININSTANCE;

		// ���⼭ ������Ÿ�� �±� ���ڿ� �����������
		wstring ws = TEXT("Prototype_Component_Model_Instance_");
		wstring wsTag = TEXT("Prototype_Component_Model_");
		wstring wsSave(MapObjectInsDesc.wszTag);
		_uint iLength = wsTag.size();

		// �� �̸�
		wstring wsModelName = wsSave.substr(iLength);
		ws += wsModelName;

		ws += TEXT("_");

		_tchar wszNumber[MAX_PATH];
		_itow_s(iCount, wszNumber, 10);

		ws += wszNumber; // ������� ���� Prototype_Component_Model_Instance_�𵨸�_��ȣ �̷������� �̸��� ����	

		lstrcpy(MapObjectInsDesc.wszTag, ws.c_str()); // ������ �� �ν��Ͻ� �̸��� �־���

		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_InsMapObject_%d"), (iCount));

		if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE,
			TEXT("Prototype_GameObject_MapObject_Ins"), TEXT("Layer_BackGround"),
			wszobjName, &MapObjectInsDesc)))
		{
			MSG_BOX("Failed to Install MapObject_Ins");
			return E_FAIL;
		}

		++iCount; ENDINSTANCE;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_Cliffside::Load_Monsters(const wstring& wstrMonsterFilePath)
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
		if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE, wstrPrototypeTag.c_str(),
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

HRESULT CLevel_Cliffside::Ready_Layer_UI(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Main"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Main)");
		ENDINSTANCE;
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");

	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_HP.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_HP"),
	//	pLayerTag, TEXT("GameObject_UI_Group_HP"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_HP)");
	//	ENDINSTANCE;
	//	return E_FAIL;
	//}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Potion.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Potion"),
		pLayerTag, TEXT("GameObject_UI_Group_Potion"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Potion)");
		ENDINSTANCE;
		return E_FAIL;
	}
	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Front.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Finisher"),
	//	pLayerTag, TEXT("GameObject_UI_Group_Finisher"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Finisher)");
	//	ENDINSTANCE;
	//	return E_FAIL;
	//}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Skill_1.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Skill"),
		pLayerTag, TEXT("Prototype_GameObject_UI_Group_Skill"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_UI_Group_Skill)");
		return E_FAIL;
	}
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Finisher_Icon.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
		pLayerTag, TEXT("GameObject_UI_Group_Finisher_Icon"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Finisher_Icon)");
		return E_FAIL;
	}
	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_MiniMap.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_MiniMap"),
	//	pLayerTag, TEXT("GameObject_UI_Group_MiniMap"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_MiniMap)");
	//	return E_FAIL;
	//}

	//CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;
	//
	//Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::BOSS;
	//lstrcpy(Desc.wszObjectLevel, TEXT("77"));
	//lstrcpy(Desc.wszObjectName, TEXT("개철민"));

	//if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"),
	//	pLayerTag, TEXT("GameObject_UI_Enemy_HP"), &Desc)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Enemy_HP)");
	//	return E_FAIL;
	//}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Layer_FieldGuide_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_FieldGuide"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_FieldGuide)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");
	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Cursor"),
	//	pLayerTag, TEXT("GameObject_UI_Group_Info_Cursor"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Cursor)");
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}	
	
	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Alpha3.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Field_Guide"),
		pLayerTag, TEXT("GameObject_UI_Field_Guide"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Field_Guide)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Cliffside::Ready_Layer_Menu_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(pGameInstance->Add_Scene(TEXT("Scene_Menu"), pLayerTag)))
	{
		MSG_BOX("Failed Add Scene : (Scene_Menu)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	_tchar szFilePath[MAX_PATH] = TEXT("");
	//lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Cursor.uidata"));
	//if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_UI_Group_Cursor"),
	//	pLayerTag, TEXT("GameObject_UI_Group_Info_Cursor"), szFilePath)))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_UI_Group_Cursor)");
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}


	lstrcpy(szFilePath, TEXT("../../Resources/GameData/UIData/UI_Group_Menu_Frame.uidata"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_Main_Menu"),
		pLayerTag, TEXT("GameObject_UI_Main_Menu"), szFilePath)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Main_Menu)");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	//if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Menu_Gear"),
	//	pLayerTag, TEXT("GameObject_UI_Menu_Gear"))))
	//{
	//	MSG_BOX("Failed Add_GameObject : (GameObject_Menu_Gear)");
	//	Safe_Release(pGameInstance);
	//	return E_FAIL;
	//}
	



	Safe_Release(pGameInstance);
	return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Cliffside::Ready_Layer_Debug(const _tchar* pLayerTag)
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

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_CLIFFSIDE, LEVEL_CLIFFSIDE, TEXT("Prototype_GameObject_LoadTrigger"), pLayerTag, TEXT("GameObject_LoadTrigger"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_LoadTrigger)");
		return E_FAIL;
	}*/

	Safe_Release(pGameInstance);

	return S_OK;
}

#endif // _DEBUG

CLevel_Cliffside* CLevel_Cliffside::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Cliffside* pInstance = New CLevel_Cliffside(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created CLevel_Cliffside");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Cliffside::Free()
{
	__super::Free();
}
