#include "..\Public\Level_Vault.h"
#include "GameInstance.h"

#include "MapObject.h"
#include "MapObject_Ins.h"
#include "Trigger_Vault.h"
#include "Player.h"
#include "Level_Loading.h"
#include "Event_Enter_Vault.h"

CLevel_Vault::CLevel_Vault(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Vault::Initialize()
{
	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Monster(TEXT("Layer_Monster")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Events(TEXT("Layer_Event")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Shader(), E_FAIL);

	BEGININSTANCE;
	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	pGameInstance->Stop_AllSound();
	//pGameInstance->Play_BGM(TEXT("Vault_Bgm.wav"), 0.6f);
	ENDINSTANCE;

    return S_OK;
}

void CLevel_Vault::Tick(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_ESCAPE, CInput_Device::KEY_DOWN))
	{
		if (!lstrcmp(TEXT("Scene_Main"), pGameInstance->Get_CurrentSceneTag()))
		{
			BEGININSTANCE;
			pGameInstance->Play_Sound(TEXT("Field.wav"), 1.0f);
			ENDINSTANCE
			pGameInstance->Set_CurrentScene(TEXT("Scene_FieldGuide"), false);
		}
		else
		{
			BEGININSTANCE;
			pGameInstance->Play_Sound(TEXT("Field.wav"), 1.0f);
			ENDINSTANCE
			pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
		}
	}

	if (true == m_isNextLevel)
	{
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVELID(m_iNextLevelIndex)));
	}

#ifdef _DEBUG
	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT))
	{
		if (pGameInstance->Get_DIKeyState(DIK_BACKSPACE, CInput_Device::KEY_DOWN))
		{
			pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_SMITH));
		}
	}
#endif // _DEBUG

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

HRESULT CLevel_Vault::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_bool isNight = false;
	FAILED_CHECK_RETURN(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_VAULT,
		TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"), &isNight), E_FAIL)

	if (FAILED(Load_MapObject(TEXT("../../Resources/GameData/MapData/MapData1.ddd"))))
	{
		MSG_BOX("Failed Load Map Object");

		return E_FAIL;
	}

	if (FAILED(Load_MapObject_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins1.ddd"))))
	{
		MSG_BOX("Failed Load Map Object_Ins");

		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Vault::Ready_Events(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT, TEXT("Prototype_GameObject_Event_Spawn"), pLayerTag, TEXT("Event_Spawn"))))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Spawn)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT, TEXT("Prototype_GameObject_Event_Torch"), pLayerTag, TEXT("Event_Torch"))))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Torch)");
		return E_FAIL;
	}
	
	CEvent_Enter_Vault::INITEVENTENTERVAULT InitEnterVaultDesc;
	InitEnterVaultDesc.vTriggerWorldPosition = _float3(15.f, 0.f, 15.f);
	InitEnterVaultDesc.vTriggerSize = _float3(5.f, 5.f, 5.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT, TEXT("Prototype_GameObject_Event_Enter_Vault"), pLayerTag, TEXT("Event_Enter_Vault"), &InitEnterVaultDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Enter_Vault)");
		return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
		TEXT("Prototype_GameObject_Event_Vault_Next_Level"), pLayerTag, TEXT("Event_Vault_Next_Level"))))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Vault_Next_Level)");
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Vault::Ready_Lights()
{
	g_isNight = true;

	BEGININSTANCE;

	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPos = _float4(12.5f, 40.5f, 44.f, 1.f);
	LightDesc.vLookAt = _float4(51.7f, 0.f, 52.4f, 1.f);
	LightDesc.vDir = LightDesc.vLookAt - LightDesc.vPos;

	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 0.1f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(LightDesc, nullptr, true, 0, _float(g_iWinSizeX) / g_iWinSizeY)))
		return E_FAIL;

	ENDINSTANCE;
	
	return S_OK;
}

HRESULT CLevel_Vault::Ready_Shader()
{
	BEGININSTANCE;

	CRenderer* pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")));
	pRenderer->Defualt_Shading();

	pRenderer->Set_SSAO(false);
	pRenderer->Get_Dof()->Set_FocusRange(500.f);

	Safe_Release(pRenderer);

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Vault::Load_MapObject(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	_uint iObjectNum = 0;
	_uint iTorchNum = 0;

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

		// 읽어온 태그에서 모델이름 추출
		wstring ws(MapObjectDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);

		// 비교해야되는 문자열
		wstring wsHorklump(TEXT("Anim_Horklump"));
		wstring wsLeech(TEXT("Anim_Leech"));
		wstring wsVaultGate(TEXT("Anim_Gate_Vault"));
		wstring wsVaultTorch(TEXT("SM_Intro_Vault_Torch"));
		wstring wsLightStand(TEXT("SM_SanctumDun_LightStand_A"));

		// 채집물
		if (0 == lstrcmp(modelName.c_str(), wsHorklump.c_str()) ||
			0 == lstrcmp(modelName.c_str(), wsLeech.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Gatherer_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
				TEXT("Prototype_GameObject_Gatherer"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Gatherer");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 볼트 게이트
		else if (0 == lstrcmp(modelName.c_str(), wsVaultGate.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Vault_Gate_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
				TEXT("Prototype_GameObject_Vault_Gate"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Vault_Gate");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 볼트 횃불
		else if (0 == lstrcmp(modelName.c_str(), wsVaultTorch.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Vault_Torch_%d"), (iTorchNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
				TEXT("Prototype_GameObject_Vault_Torch"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone Vault_Torch");
				ENDINSTANCE;
				return E_FAIL;
			}

			++iTorchNum;
		}

		// 화로
		else if (0 == lstrcmp(modelName.c_str(), wsLightStand.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_LightStand_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
				TEXT("Prototype_GameObject_LightStand"), TEXT("Layer_BackGround"),
				wszobjName, &MapObjectDesc)))
			{
				MSG_BOX("Failed to Clone LightStand");
				ENDINSTANCE;
				return E_FAIL;
			}
		}

		// 일반 맵 오브젝트
		else
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
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

HRESULT CLevel_Vault::Load_MapObject_Ins(const _tchar* pObjectFilePath)
{
	std::lock_guard<std::mutex> lock(mtx);

	HANDLE hFile = CreateFile(pObjectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins");
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
			MSG_BOX("Failed to Read MapObject_Ins iInstanceCnt");
			return E_FAIL;
		}

		if (0 != MapObjectInsDesc.iInstanceCnt)
		{
			_float4x4 Matrix;

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

		if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT,
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

HRESULT CLevel_Vault::Load_Monsters(const wstring& wstrMonsterFilePath)
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
		if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT, wstrPrototypeTag.c_str(),
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

HRESULT CLevel_Vault::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	Load_Monsters(TEXT("../../Resources/GameData/MonsterData/Vault.mon"));
	Load_Monsters(TEXT("../../Resources/GameData/MonsterData/Pensive.mon"));
	
	_float4x4 Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(165.f, 0.02f, 92.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_VAULT, LEVEL_VAULT, TEXT("Prototype_GameObject_Golem_CombatGrunt"), pLayerTag, TEXT("Prototype_GameObject_Golem_CombatGrunt"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Golem_CombatGrunt)");
		ENDINSTANCE;
		return E_FAIL;
	}

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
