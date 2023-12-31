#include "..\Public\Level_Sky.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "MapObject.h"
#include "MapObject_Ins.h"
#include "ScoreBalloon.h"

CLevel_Sky::CLevel_Sky(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}
HRESULT CLevel_Sky::Initialize()
{
	FAILED_CHECK_RETURN(Ready_Lights(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_BackGround(TEXT("Layer_BackGround")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_Monster(TEXT("Layer_Monster")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_FlyGame(TEXT("Layer_Manager")), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Layer_NPC(TEXT("Layer_NPC")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Shader(), E_FAIL);
	//FAILED_CHECK_RETURN(Ready_Events(TEXT("Layer_Event")), E_FAIL);

	BEGININSTANCE;
	pGameInstance->Reset_World_TimeAcc();
	pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);
	pGameInstance->Stop_AllSound();
	//pGameInstance->Play_BGM(TEXT("Sky_Bgm.wav"), 0.6f);
	ENDINSTANCE;


	return S_OK;
}

void CLevel_Sky::Tick(_float fTimeDelta)
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
		pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, (LEVELID)m_iNextLevelIndex));
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
	SetWindowText(g_hWnd, TEXT("Sky"));
#endif //_DEBUG
}

HRESULT CLevel_Sky::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	_bool isNight = g_isNight;
	FAILED_CHECK_RETURN(pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_SKY,
		TEXT("Prototype_GameObject_Sky"), pLayerTag, TEXT("GameObject_Sky"), &isNight), E_FAIL)

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

	_float4x4 Matrix = XMMatrixTranslation(108.f, 35.f, 27.f);

	/*Matrix = XMMatrixRotationY(XMConvertToRadians(320.f)) * XMMatrixTranslation(59.f, 2.f, 42.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_1"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_1)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	/*Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(59.f, 0.6f, 42.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_Dugbog"), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Dugbog"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Dugbog)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	/*Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(48.f, 0.6f, 42.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_Golem_CombatGrunt"), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Golem_CombatGrunt"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Golem_CombatGrunt)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(48.f, 0.6f, 42.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_M"), TEXT("Layer_Monster"), TEXT("Prototype_GameObject_DarkWizard_M"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_DarkWizard_M)");
		ENDINSTANCE;
		return E_FAIL;
	}


	/*Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(181.f, 35.f, 200.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork2"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks2)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(188.f, 35.f, 154.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork3"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks3)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(174.f, 38.f, 120.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork4"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks4)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(168.f, 42.f, 89.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork5"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks5)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(40.f, 48.f, 124.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork6"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks6)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(80.f, 70.f, 110.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork7"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks7)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(167.f, 35.f, 246.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork8"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks8)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(147.f, 37.f, 268.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork9"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks9)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(113.f, 33.f, 286.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork10"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks10)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(69.f, 35.f, 273.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork11"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks11)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(52.f, 35.f, 242.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork12"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks12)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(36.f, 39.f, 202.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork13"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks13)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixTranslation(36.f, 39.f, 166.f);
	Matrix._41 *= 0.5f;
	Matrix._42 *= 0.5f;
	Matrix._43 *= 0.5f;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FireWorks"), pLayerTag, TEXT("FireWork14"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_FireWorks14)");
		ENDINSTANCE;
		return E_FAIL;
	}*/

	ENDINSTANCE;


	return S_OK;
}

HRESULT CLevel_Sky::Ready_Layer_Monster(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	_float4x4 Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(44.f, 3.5f, 40.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_1"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_1)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(62.f, 3.5f, 40.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_2"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_2)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(44.f, 3.5f, 60.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_3"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_3)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(62.f, 3.5f, 60.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_4"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_4)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(44.f, 3.5f, 80.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_5"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_5)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(62.f, 3.5f, 80.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_6"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_6)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(44.f, 3.5f, 100.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_7"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_7)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(62.f, 3.5f, 100.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_DarkWizard_Fly"), pLayerTag, TEXT("GameObject_DarkWizard_Fly_8"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_DarkWizard_Fly_8)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Layer_NPC(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	if (FAILED(Load_Crowds(TEXT("../../Resources/GameData/MonsterData/Crowds.mon"))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Instance_Crowd */
	_float4x4 WorldMatrix = XMMatrixIdentity();
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_SKY, TEXT("Prototype_Component_Model_Instance_Crowd"),
		CModel_Instance::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/crowd/crowd.dat"), m_pMatrices, m_iNumCrowds, XMMatrixIdentity()))))
		throw TEXT("Prototype_Component_Model_Instance_Crowd");

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_crowd"), pLayerTag, TEXT("GameObject_Crowds"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Crowds)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Lights()
{
	g_isNight = true;

	BEGININSTANCE;

	CLight::LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = CLight::TYPE_DIRECTIONAL;
	LightDesc.vPos = _float4(30.f, 63.f, 55.f, 1.f);
	LightDesc.vLookAt = _float4(56.f, 0.f, 69.f, 1.f);
	LightDesc.vDir = LightDesc.vLookAt - LightDesc.vPos;

	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 0.8f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(LightDesc, nullptr, true)))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_Shader()
{
	BEGININSTANCE;

	CRenderer* pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")));
	pRenderer->Defualt_Shading();
	pRenderer->Get_Dof()->Set_FocusRange(200.f);
	pRenderer->Get_Dof()->Set_FocusDistance(10.f);

	Safe_Release(pRenderer);

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Sky::Ready_FlyGame(const _tchar* pLayerTag)
{
	BEGININSTANCE;
	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_FlyGameManager"), pLayerTag, TEXT("GameObject_FlyGameManager"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_FlyGameManager)");
		ENDINSTANCE;
		return E_FAIL;
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CLevel_Sky::Ready_Events(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SKY, LEVEL_SKY, TEXT("Prototype_GameObject_Event_Sky_Enter"), pLayerTag, TEXT("Event_Sky_Enter"))))
	{
		MSG_BOX("Failed Add_GameObject : (Event_Sky_Enter)");
		return E_FAIL;
	}

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

		wstring ws(MapObjectDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);

		// 비교해야되는 문자열
		wstring wsGrass(TEXT("SM_HW_QuidditchPitch_Grass"));

		// 일반 맵오브젝트
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

		CGameObject* pGameObject = static_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_SKY, 
			TEXT("Layer_BackGround"), wszobjName));

		pGameObject->Get_Transform()->Set_Scale(pGameObject->Get_Transform()->Get_Scale() * _float3(0.5f, 0.5f, 0.5f));
		pGameObject->Get_Transform()->Set_Position(pGameObject->Get_Transform()->Get_Position() * _float3(0.5f, 0.5f, 0.5f));

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

HRESULT CLevel_Sky::Load_Crowds(const wstring& wstrMonsterFilePath)
{
	HANDLE hFile = CreateFile(wstrMonsterFilePath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	_uint iMonsterNum = 0;
	_ulong dwByte = { 0 };

	vector<_float4x4> Matrices;

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

		WorldMatrix._11 *= 0.5f;
		WorldMatrix._22 *= 0.5f;
		WorldMatrix._33 *= 0.5f;
		WorldMatrix._41 *= 0.5f;
		WorldMatrix._42 *= 0.5f;
		WorldMatrix._43 *= 0.5f;

		Matrices.push_back(WorldMatrix);
	}

	m_iNumCrowds = Matrices.size();
	m_pMatrices = New _float4x4[m_iNumCrowds];
	_uint i = { 0 };

	for (const auto& Matrix : Matrices)
		m_pMatrices[i++] = Matrix;

	CloseHandle(hFile);

	return S_OK;
}

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

	Safe_Delete_Array(m_pMatrices);
}
