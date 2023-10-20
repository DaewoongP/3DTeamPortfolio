#include "..\Public\Level_Smith.h"
#include "GameInstance.h"

#include "MapObject.h"
#include "MapObject_Ins.h"
#include "Level_Loading.h"
#include "Trigger.h"
#include "Dummy_NPC.h"
#include "House_Elf.h"

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
	//FAILED_CHECK_RETURN(Ready_Event(TEXT("Layer_Event")), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Shader(), E_FAIL);

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

	if (pGameInstance->Get_DIKeyState(DIK_F8, CInput_Device::KEY_DOWN))
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

	_float4x4 Matrix = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMMatrixTranslation(86.f, 10.f, 129.f);
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

	//Matrix = XMMatrixRotationY(XMConvertToRadians(55.f)) * XMMatrixTranslation(111.95f, 6.25f, 73.5f);
	Matrix = XMMatrixRotationY(XMConvertToRadians(55.f)) * XMMatrixTranslation(35.f, 3.f, 15.f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Oakes"), pLayerTag, TEXT("GameObject_Oakes"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Oakes)");
		ENDINSTANCE;
		return E_FAIL;
	}

	Matrix = XMMatrixRotationY(XMConvertToRadians(145.5f)) * XMMatrixTranslation(52.85f, 6.25f, 81.9f);
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Smith_Fig"), pLayerTag, TEXT("GameObject_Smith_Fig"), &Matrix)))
	{
		MSG_BOX("Failed Add_GameObject : (GameObject_Smith_Fig)");
		ENDINSTANCE;
		return E_FAIL;
	}

	if (FAILED(Load_Dummy_NPC(pLayerTag)))
		return E_FAIL;

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

HRESULT CLevel_Smith::Ready_Shader()
{
	BEGININSTANCE;

	CRenderer* pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer")));
	pRenderer->Defualt_Shading();



	Safe_Release(pRenderer);

	ENDINSTANCE;

	return S_OK;
}

HRESULT CLevel_Smith::Load_Dummy_NPC(const _tchar* pLayerTag)
{
	BEGININSTANCE;

	/* NPC Male */
	CDummy_NPC::NPCINITDESC InitDesc;
	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(51.f)) * XMMatrixTranslation(59.9f, 6.9f, 88.95f);
	InitDesc.wstrAnimationTag = TEXT("Table_Convor_2");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Arcane");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_M_C");
	InitDesc.MeshPartsTags[2] = TEXT("Prototype_Component_MeshPart_Head_NPC_M");
	InitDesc.MeshPartsTags[4] = TEXT("Prototype_Component_MeshPart_Player_Arm");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DarkArts");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Arcane_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	InitDesc.wstrCustomModelTag = TEXT("Prototype_Component_Model_CustomModel_NPC_M");
#ifdef _DEBUG
	InitDesc.isCheckPosition = false;
#endif // _DEBUG
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_0"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_0)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(70.f)) * XMMatrixTranslation(56.05f, 7.05f, 91.85f);
	InitDesc.wstrAnimationTag = TEXT("Table_Convor_1");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_M_C");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_HerbologyCoat");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Arcane_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Merlin");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_1"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_1)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(91.f)) * XMMatrixTranslation(72.35f, 6.65f, 89.1f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Convor_1");
	InitDesc.MeshPartsTags[0] = TEXT("");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Player_Hair");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DragonGrab");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Celtic_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Merlin");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_2"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_2)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(101.5f)) * XMMatrixTranslation(36.6f, 3.8f, 67.6f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Convor_2");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_M_C");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_HerbologyCoat");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Arcane_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Merlin");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_3"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_3)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(40.5f)) * XMMatrixTranslation(55.45f, 4.65f, 60.95f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Convor_3");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Arcane");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_M_C");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Arcane");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_Arcane_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_4"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_4)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixTranslation(37.3f, 3.8f, 67.1f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Convor_4");
	InitDesc.MeshPartsTags[0] = TEXT("");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Player_Hair");
	InitDesc.MeshPartsTags[3] = TEXT("Prototype_Component_MeshPart_Mask_Guardian");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DarkArtsDelux_HoodDown");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_DarkArtsDelux_A");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_DarkArtsDelux");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_4.png"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_M_5"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_M_5)");
		ENDINSTANCE;
		return E_FAIL;
	}

	/* NPC Female*/
	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-131.f)) * XMMatrixTranslation(57.85f, 7.f, 92.6f);
	InitDesc.wstrAnimationTag = TEXT("Table_Drinking");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Pony");
	InitDesc.MeshPartsTags[2] = TEXT("Prototype_Component_MeshPart_Head_NPC_F");
	InitDesc.MeshPartsTags[3] = TEXT("");
	InitDesc.MeshPartsTags[4] = TEXT("Prototype_Component_MeshPart_Arm_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DruidCape_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.wstrCustomModelTag = TEXT("Prototype_Component_Model_CustomModel_NPC_F");
	InitDesc.isInteraction = false;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_0"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_0)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-75.f)) * XMMatrixTranslation(61.6f, 6.9f, 89.15f);
	InitDesc.wstrAnimationTag = TEXT("Table_Convor");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Long_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DarkArts_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_1"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_1)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-94.5f)) * XMMatrixTranslation(57.8f, 6.9f, 88.05f);
	InitDesc.wstrAnimationTag = TEXT("Table_Eating");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Short_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Beast_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_2"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_2)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-91.5f)) * XMMatrixTranslation(73.55f, 6.6f, 89.3f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Zonkos");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Pony");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Herbology_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacker_Celtic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_5.png"));

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_3"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_3)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-36.5f)) * XMMatrixTranslation(74.55f, 6.3f, 77.4f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Conversation_NewClothes_1");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Long_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_DarkArts_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_6.png"));

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_4"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_4)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(114.f)) * XMMatrixTranslation(73.3f, 6.35f, 77.75f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Note");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Short_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Beast_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = false;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_5"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_5)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-128.5f)) * XMMatrixTranslation(56.2f, 4.8f, 61.75f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Wand");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Witch_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Pony");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Herbology_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_2.png"));

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_6"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_6)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixTranslation(41.55f, 3.1f, 55.1f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Flower");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Short_F");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Herbology_1_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacket_BeastTunic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = false;

	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_7"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_7)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-186.5f)) * XMMatrixTranslation(60.25f, 6.85f, 90.4f);
	InitDesc.wstrAnimationTag = TEXT("Table_Nap");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Pony");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Herbology_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacker_Celtic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_3.png"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_8"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_8)");
		ENDINSTANCE;
		return E_FAIL;
	}

	InitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(113.5f)) * XMMatrixTranslation(39.15f, 1.55f, 29.3f);
	InitDesc.wstrAnimationTag = TEXT("Stand_Note");
	InitDesc.MeshPartsTags[0] = TEXT("Prototype_Component_MeshPart_Hat_Wizard_F");
	InitDesc.MeshPartsTags[1] = TEXT("Prototype_Component_MeshPart_Hair_Pony");
	InitDesc.MeshPartsTags[5] = TEXT("Prototype_Component_MeshPart_Robe_Herbology_F");
	InitDesc.MeshPartsTags[6] = TEXT("Prototype_Component_MeshPart_Jacker_Celtic_A_F");
	InitDesc.MeshPartsTags[7] = TEXT("Prototype_Component_MeshPart_Pants_Arcane_F");
	InitDesc.MeshPartsTags[9] = TEXT("Prototype_Component_MeshPart_Boots_Arcane_F");
	InitDesc.isInteraction = true;
	lstrcpy(InitDesc.wszScriptTag, TEXT("../../Resources/UI/Game/Script/Town/Town_1.png"));
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Dummy_NPC"), pLayerTag, TEXT("NPC_F_9"), &InitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (NPC_F_9)");
		ENDINSTANCE;
		return E_FAIL;
	}

	/* Doby*/
	CHouse_Elf::ELFINITDESC ElfInitDesc;
	ElfInitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(234.f)) * XMMatrixTranslation(59.1f, 6.6f, 93.96f);
	ElfInitDesc.wstrAnimationTag = TEXT("Choose");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Doby"), pLayerTag, TEXT("Doby_0"), &ElfInitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Doby_0)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ElfInitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(200.f)) * XMMatrixTranslation(34.f, 1.8f, 34.f);
	ElfInitDesc.wstrAnimationTag = TEXT("Walk");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Doby"), pLayerTag, TEXT("Doby_1"), &ElfInitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Doby_1)");
		ENDINSTANCE;
		return E_FAIL;
	}

	ElfInitDesc.WorldMatrix = XMMatrixRotationY(XMConvertToRadians(60.f)) * XMMatrixTranslation(77.f, 6.6f, 90.f);
	ElfInitDesc.wstrAnimationTag = TEXT("Walk");
	if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH, TEXT("Prototype_GameObject_Doby"), pLayerTag, TEXT("Doby_2"), &ElfInitDesc)))
	{
		MSG_BOX("Failed Add_GameObject : (Doby_2)");
		ENDINSTANCE;
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
		wstring wsOwl(TEXT("Anim_Owl"));
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

		// 부엉이
		else if (0 == lstrcmp(modelName.c_str(), wsOwl.c_str()))
		{
			_tchar wszobjName[MAX_PATH] = { 0 };
			_stprintf_s(wszobjName, TEXT("GameObject_Owl_%d"), (iObjectNum));

			if (FAILED(pGameInstance->Add_Component(LEVEL_SMITH, LEVEL_SMITH,
				TEXT("Prototype_GameObject_Owl"), TEXT("Layer_BackGround"),
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
