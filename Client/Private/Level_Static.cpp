#include "..\Public\Level_Static.h"
#include "GameInstance.h"
#include "Camera_Includes.h"
#include "Quest_Manager.h"
#include "UI.h"
#include "UI_Group_Score.h"

IMPLEMENT_SINGLETON(CLevel_Static)

HRESULT CLevel_Static::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	FAILED_CHECK_RETURN(Ready_Layer_Player(TEXT("Layer_Player")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(TEXT("Layer_UI")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Inventory(TEXT("Layer_Inventory")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_FieldGuide_UI(TEXT("Layer_FieldGuide_UI")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Menu_UI(TEXT("Layer_Menu_UI")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Sounds(), E_FAIL);
	
#ifdef _DEBUG
	FAILED_CHECK_RETURN(Ready_Layer_Debug(TEXT("Layer_Debug")), E_FAIL);
#endif // _DEBUG

	FAILED_CHECK_RETURN(Add_Sound(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Scene(), E_FAIL);
	FAILED_CHECK_RETURN(Add_Cameras(pDevice, pContext), E_FAIL);
	FAILED_CHECK_RETURN(Add_Quests(pDevice, pContext), E_FAIL);

	m_pGameInstance->Reset_World_TimeAcc();
	m_pGameInstance->Set_CurrentScene(TEXT("Scene_Main"), true);

	Safe_Release(m_pGameInstance);

	this->DestroyInstance();

	return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_Player(const _tchar* pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_Player"), pLayerTag, TEXT("GameObject_Player")), E_FAIL)

		return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_Inventory(const _tchar* pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC
		, TEXT("Prototype_GameObject_Inventory"), pLayerTag, TEXT("GameObject_Inventory")), E_FAIL)

		return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CUI::UIDESC UIDesc;
	ZEROMEM(&UIDesc);
	UIDesc.vCombinedXY = _float2(0.f, 0.f);
	UIDesc.fX = 640.f;
	UIDesc.fY = 360.f;
	UIDesc.fZ = 0.3f;
	UIDesc.fSizeX = 64.f;
	UIDesc.fSizeY = 64.f;
	lstrcpy(UIDesc.szTexturePath, TEXT("../../Resources/UI/Game/UI/HUD/Reticles/UI_T_Aim.png"));

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Back"), pLayerTag, TEXT("GameObject_UI_Aim"), &UIDesc), E_FAIL);

	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_UI_Farming"), pLayerTag, TEXT("GameObject_UI_Farming")), E_FAIL);
	
	BEGININSTANCE;
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/UI_Sound/"));
	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_FieldGuide_UI(const _tchar* pLayerTag)
{
	_tchar szDataPath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Gear_Alpha3.uidata");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_Field_Guide"), pLayerTag, TEXT("GameObject_UI_Field_Guide"), szDataPath), E_FAIL)

		return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_Menu_UI(const _tchar* pLayerTag)
{
	_tchar szDataPath[MAX_PATH] = TEXT("../../Resources/GameData/UIData/UI_Group_Menu_Frame.uidata");
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_Main_Menu"), pLayerTag, TEXT("GameObject_UI_Main_Menu"), szDataPath), E_FAIL)

	// 가이드 북
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_Guide_Book"), pLayerTag, TEXT("GameObject_Guide_Book")), E_FAIL)

	return S_OK;
}

HRESULT CLevel_Static::Ready_Sounds()
{
	BEGININSTANCE;
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Pensive/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Golem/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Dugbog/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Monster/"));
	ENDINSTANCE;
}
HRESULT CLevel_Static::Add_Sound()
{
	BEGININSTANCE;
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Animal/BigBird/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Animal/Cat/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Animal/Owl/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Animal/Gull/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/Door/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/Gatherer/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/LightStand/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/Portal/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/PotionStation/"));
	pGameInstance->Add_Sounds(TEXT("../../Resources/Sound/Object/TreasureChest/"));
	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Static::Add_Scene()
{
	/* Add Scene : Main */
	vector<const _tchar*>		MainSceneLayerTags;
	MainSceneLayerTags.push_back(TEXT("Layer_Player"));
	MainSceneLayerTags.push_back(TEXT("Layer_UI"));
	MainSceneLayerTags.push_back(TEXT("Layer_BackGround"));
	MainSceneLayerTags.push_back(TEXT("Layer_Monster"));
	MainSceneLayerTags.push_back(TEXT("Layer_NPC"));
	MainSceneLayerTags.push_back(TEXT("Layer_Magic"));
	MainSceneLayerTags.push_back(TEXT("Layer_Particle"));
	MainSceneLayerTags.push_back(TEXT("Layer_Item"));
	MainSceneLayerTags.push_back(TEXT("Layer_Event"));
	MainSceneLayerTags.push_back(TEXT("Layer_Balloon"));
	MainSceneLayerTags.push_back(TEXT("Layer_Manager"));
#ifdef _DEBUG
	MainSceneLayerTags.push_back(TEXT("Layer_Debug"));
#endif // _DEBUG

	/* Add Scene : FieldGuide */
	vector<const _tchar*>		FieldGuideSceneLayerTags;
	FieldGuideSceneLayerTags.push_back(TEXT("Layer_FieldGuide_UI"));

	/* Add Scene : Menu */
	vector<const _tchar*>		MenuSceneLayerTags;
	MenuSceneLayerTags.push_back(TEXT("Layer_Menu_UI"));
	MenuSceneLayerTags.push_back(TEXT("Layer_Inventory"));

	try
	{
		for (auto& szLayerTag : MainSceneLayerTags)
		{
			/* Add Scene : Main */
			if (FAILED(m_pGameInstance->Add_Scene(TEXT("Scene_Main"), szLayerTag)))
				throw szLayerTag;
		}

		for (auto& szLayerTag : FieldGuideSceneLayerTags)
		{
			/* Add Scene : FieldGuide */
			if (FAILED(m_pGameInstance->Add_Scene(TEXT("Scene_FieldGuide"), szLayerTag)))
				throw szLayerTag;
		}

		for (auto& szLayerTag : MenuSceneLayerTags)
		{
			/* Add Scene : Menu */
			if (FAILED(m_pGameInstance->Add_Scene(TEXT("Scene_Menu"), szLayerTag)))
				throw szLayerTag;
		}
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add Layer to Scene : ");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Static::Add_Cameras(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//카메라 추가
	CCamera::CAMERADESC CameraDesc;
	CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
	CameraDesc.m_fFovY = XMConvertToRadians(90.f);
	CameraDesc.m_fNear = 0.1f;
	CameraDesc.m_fFar = 1000.f;

	// PotionStation
	CPotionStationCamera::POTIONSTATION_CAMERA_DESC Potionstation_Camera_Desc;
	Potionstation_Camera_Desc.vAt = { 98.124f, 8.180f, 77.079f };
	Potionstation_Camera_Desc.pSuperDesc = CameraDesc;

	m_pGameInstance->Add_Camera(TEXT("Potion_Station_Camera"),
		CPotionStationCamera::Create(pDevice, pContext, &Potionstation_Camera_Desc));

	return S_OK;
}

HRESULT CLevel_Static::Add_Quests(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
	Safe_AddRef(pQuest_Manager);

	FAILED_CHECK(pQuest_Manager->Add_Quest(TEXT("Quest_Save_Fig"), 
		CQuest_Save_Fig::Create(pDevice, pContext)));

	FAILED_CHECK(pQuest_Manager->Add_Quest(TEXT("Quest_Potion"),
		CQuest_Potion::Create(pDevice, pContext)));

	FAILED_CHECK(pQuest_Manager->Add_Quest(TEXT("Quest_Town"),
		CQuest_Town::Create(pDevice, pContext)));

	FAILED_CHECK(pQuest_Manager->Add_Quest(TEXT("Quest_Secret"),
		CQuest_Secret::Create(pDevice, pContext)));

	FAILED_CHECK(pQuest_Manager->Add_Quest(TEXT("Quest_Bone"),
		CQuest_Bone::Create(pDevice, pContext)));

	Safe_Release(pQuest_Manager);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CLevel_Static::Ready_Layer_Debug(const _tchar* pLayerTag)
{
	FAILED_CHECK_RETURN(m_pGameInstance->Add_Component(LEVEL_STATIC, LEVEL_STATIC,
		TEXT("Prototype_GameObject_Camera_Debug"), pLayerTag, TEXT("GameObject_Camera_Debug")), E_FAIL);

	return S_OK;
}

#endif // _DEBUG

void CLevel_Static::Free()
{
}
