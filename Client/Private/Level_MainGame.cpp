#include "..\Public\Level_MainGame.h"
#include "GameInstance.h"

#include "MapObject.h"

CLevel_MainGame::CLevel_MainGame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_MainGame::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
#ifdef _DEBUG
	if (FAILED(Ready_Layer_Debug(TEXT("Layer_Debug"))))
		return E_FAIL;
#endif // _DEBUG

	BEGININSTANCE;
	/* 게임 초기화와 함께 월드시간 초기화 */
	pGameInstance->Reset_World_TimeAcc();
	ENDINSTANCE;

	return S_OK;
}

void CLevel_MainGame::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("메인게임레벨입니다."));
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

	Load_MapObject();

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_Player(const _tchar* pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_MainGame::Load_MapObject()
{
	_tchar dataFile[MAX_PATH] = TEXT("../../Resources/GameData/MapData/MapObject.ddd");

	HANDLE hFile = CreateFile(dataFile, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	// 맵 오브젝트 번호
	_uint iObjectNum = 0;

	DWORD	dwByte = 0;

	while (true)
	{
		LOADOBJECTDESC LoadDesc;
		ZEROMEM(&LoadDesc);		

		if (!ReadFile(hFile, &LoadDesc.matTransform, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.vPos");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &LoadDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.iTagLen");
		}

		if (!ReadFile(hFile, &LoadDesc.wszTag, LoadDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read m_vecSaveObject.wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		// 맵 오브젝트에 번호 붙여줌
		_tchar wszobjName[MAX_PATH] = { 0 };
		_stprintf_s(wszobjName, TEXT("GameObject_MapObject_%d"), (iObjectNum));

		// 번호를 붙인 태그로 MapObject 등록
		BEGININSTANCE if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME,
			TEXT("Prototype_GameObject_MapObject"), TEXT("Layer_BackGround"),
			wszobjName, &LoadDesc.matTransform)))
		{
			MSG_BOX("Failed to Install MapObject");
			ENDINSTANCE;
			return E_FAIL;
		} ENDINSTANCE;

		// 마지막에 설치한 맵 오브젝트 주소 가져옴
		CMapObject* pObject = static_cast<CMapObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME,
			TEXT("Layer_BackGround"), wszobjName));

		pObject->Add_Model_Component(TEXT("Prototype_Component_Model__Intro_RuinsFloorBroken"));
		pObject->Add_Shader_Component(TEXT("Prototype_Component_Shader_VtxMesh"));
		pObject->Set_Color(iObjectNum); // 고유한 색깔 값을 넣어줌

		++iObjectNum;	
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_MainGame::Ready_Layer_Effect(const _tchar* pLayerTag)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

#ifdef _DEBUG
 	if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Particle"), pLayerTag, TEXT("GameObject_Test_Particle"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Particle)");
		return E_FAIL;
	}
#endif _DEBUG
	Safe_Release(pGameInstance);

	return S_OK;
}
#ifdef _DEBUG
HRESULT CLevel_MainGame::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	//
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
	
	/*if (FAILED(pGameInstance->Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Cloth"), pLayerTag, TEXT("GameObject_Test_Cloth"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Test_Cloth)");
		return E_FAIL;
	}*/

	/*if (FAILED(pGameInstance->Add_Component(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Armored_Troll"), pLayerTag, TEXT("GameObject_Armored_Troll"))))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Armored_Troll)");
		return E_FAIL;
	}*/

	Safe_Release(pGameInstance);

	return S_OK;
}
#endif // _DEBUG
CLevel_MainGame* CLevel_MainGame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_MainGame* pInstance = new CLevel_MainGame(pDevice, pContext);

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
