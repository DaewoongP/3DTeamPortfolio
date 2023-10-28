#include "..\Public\Main0_Loader.h"
#include "GameInstance.h"

////////////////////////////////////////////////////////////
// UI / Texture / Buffer / Debug / Shader / Effect / Magic//
////////////////////////////////////////////////////////////
#include "Logo_BackGround.h"

#pragma region UI
#include "UI_Group_Logo.h"
#include "UI_Group_HP.h"
#include "UI_Group_Potion.h"
#include "UI_Group_Finisher.h"
#include "UI_Group_Finisher_Icon.h"
#include "UI_Group_Skill.h"
#include "UI_Group_MiniMap.h"
#include "UI_Group_Enemy_HP.h"
#include "UI_Group_Cursor.h"
#include "UI_Slot.h"
#include "UI_Progress1.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Font.h"
#include "Field_Guide.h"
#include "Main_Menu.h"
#include "Menu_Gear.h"
#include "Menu_Quest.h"
#include "Menu_Invectory.h"
#include "Menu_Setting.h"
#include "UI_Inventory.h"
#include "Inventory.h"
#include "UI_Potion_Tap.h"
#include "PotionTap.h"
#include "UI_Group_Brew.h"
#include "UI_Group_SKillTap.h"
#include "UI_Farming.h"
#include "UI_Dynamic_Back.h"
#include "UI_Damage.h"
#include "UI_Store.h"
#include "UI_Interaction.h"
#include "Script.h"
#include "UI_Group_Score.h"
#include "UI_Group_Timer.h"
#pragma endregion UI

#pragma region Effects
#include "ParticleSystem.h"
#include "ParticleSystemPool.h"
#include "Trail.h"
#include "MeshEffect.h"
#include "Wingardium_Effect.h"
#include "Blink_Effect.h"
#include "EnergyBall.h"
#include "Breath.h"
#include "Pulse.h"
#include "RadialBlur.h"
#include "Projectile_White_Effect.h"
#include"FireWorks.h"
#pragma endregion Effects

#pragma region Magic
#include "Magic.h"
#include "MagicBallPool.h"

#include "MagicSlot.h"
#include "BasicCast.h"
#include "Protego.h"
#include "Revelio.h"
#include "Levioso.h"
#include "Confringo.h"
#include "Finisher.h"
#include "Wingardiumleviosa.h"
#include "Ncendio.h"
#include "Lumos.h"
#include "Arrestomomentum.h"
#include "Descendo.h"
#include "Accio.h"
#include "Flipendo.h"
#include "Expelliarmus.h"
#include "Imperio.h"
#include "Crucio.h"
#include "Stupefy.h"
#include "Diffindo.h"
#include "Bombarda.h"

#include "Projectile_Black.h"
#include "Projectile_White.h"

#include "Pensive_Fail_Ball.h"
#include "Pensive_Ground_Ball.h"
#include "Pensive_Shouting.h"
#include "Pensive_Sword_Throw.h"
#include "Pensive_Mace_Attack.h"

#include "Magic_Sound_Manager.h"
#pragma endregion Magic

#include "Trigger_Vault.h"
#include "Phase.h"
#include "Cylinder.h"
#include "Water.h"

#pragma region Event
#include "Event_Spawn.h"
#include "Event_Enter_Sanctum.h"
#include "Event_Vault_Spawn.h"
#include "Event_Vault_Torch.h"
#include "Event_Enter_Vault.h"
#include "Event_Smeade.h"
#include "Event_Cliffside.h"
#include "Event_Spawn_Dragon.h"
#include "Event_Spawn_Dragon_2.h"
#include "Event_Cliffside_Next_Level.h"
#include "Event_Vault_Next_Level.h"
#include "Event_Smeade_Next_Level.h"
#pragma endregion

#pragma region Event
#include "Balloon_Coin.h"
#include "Racer.h"
#include "FlyGameManager.h"
#pragma endregion

#include "Guide_Book.h"
#include "RedSky.h"

#ifdef _DEBUG
#include "Test_Player.h"
#include "Camera_Debug.h"
#include "PhysXRender.h"
#include "Norm_Test.h"
#endif // _DEBUG

CMain0_Loader::CMain0_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint WINAPI Thread_Main0(void* pArg)
{
	CMain0_Loader* pLoader = reinterpret_cast<CMain0_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain0_Loader::Initialize(LEVELID eNextLevel, _bool isStaticLoaded)
{
	m_eNextLevelID = eNextLevel;
	m_isStaticLoaded = isStaticLoaded;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main0, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain0_Loader::Loading()
{
	FAILED_CHECK_RETURN(CoInitializeEx(nullptr, COINIT_MULTITHREADED), E_FAIL);

	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	if (false == m_isStaticLoaded)
	{
		hr = Loading_For_Static(LEVEL_STATIC);
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(&m_Critical_Section);
		return E_FAIL;
	}

	hr = 0;

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo(LEVEL_LOGO);
		break;
	case LEVEL_CLIFFSIDE:
		hr = Loading_For_Cliffside(LEVEL_CLIFFSIDE);
		break;
	case LEVEL_VAULT:
		hr = Loading_For_Vault(LEVEL_VAULT);
		break;
	case LEVEL_SMITH:
		hr = Loading_For_Hogsmeade(LEVEL_SMITH);
		break;
	case LEVEL_SKY:
		hr = Loading_For_Sky(LEVEL_SKY);
		break;
	case LEVEL_SANCTUM:
		hr = Loading_For_Sanctum(LEVEL_SANCTUM);
		break;
	default:
		MSG_BOX("Failed Load Next Level");
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	m_isFinished = true;

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Logo(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Add_Prototype*/
	{
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Logo"),
			CUI_Group_Logo::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Logo");

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Cliffside(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		/* For.Prototype_GameObject_Water */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Water"),
			CWater::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Water");

		/* For.Prototype_GameObject_Event_Cliffside */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Cliffside"),
			CEvent_Cliffside::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Event_Cliffside");

		/* For.Prototype_GameObject_Event_Cliffside_Next_Level */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Cliffside_Next_Level"),
			CEvent_Cliffside_Next_Level::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Event_Cliffside_Next_Level");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Vault(LEVELID eLevelID)
{
	/* For.Prototype_GameObject_MeshEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Cloister_MeshEffect"),
		CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Cloister/Cloister.ME")))))
		throw TEXT("Prototype_GameObject_Cloister_MeshEffect");

	/* For.Prototype_GameObject_Vault_Torch_P*/
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Vault_Torch_P"),
		CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Vault_Torch/")))))
		throw TEXT("Prototype_GameObject_Vault_Torch_P");

	/* For.Prototype_GameObject_Trigger_Vault */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Trigger_Vault"),
		CTrigger_Vault::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Trigger_Vault");

	/* For.Prototype_GameObject_Event_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Spawn"),
		CEvent_Vault_Spawn::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Spawn");

	/* For.Prototype_GameObject_Event_Torch */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Torch"),
		CEvent_Vault_Torch::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Torch");

	/* For.Prototype_GameObject_Event_Enter_Vault */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Enter_Vault"),
		CEvent_Enter_Vault::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Enter_Vault");

	/* For.Prototype_GameObject_Event_Vault_Next_Level */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Vault_Next_Level"),
		CEvent_Vault_Next_Level::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Vault_Next_Level");

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_GreatHall(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Hogsmeade(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	BEGININSTANCE;

	/* For.Prototype_GameObject_Event_Smeade */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Smeade"),
		CEvent_Smeade::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Smeade");

	/* For.Prototype_GameObject_Event_Smeade_Next_Level */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Smeade_Next_Level"),
		CEvent_Smeade_Next_Level::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Smeade_Next_Level");

	

	ENDINSTANCE;

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Sky(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		/* For.Prototype_GameObject_Racer */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Racer"),
			CRacer::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Racer");
		
		/* For.Prototype_GameObject_Balloon_Coin */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Balloon_Coin"),
			CBalloon_Coin::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Balloon_Coin");

		/* For.Prototype_GameObject_FlyGameManager */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_FlyGameManager"),
			CFlyGameManager::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_FlyGameManager");
	
	}


	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Sanctum(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Loading_For_Sanctum Add_Prototype*/
	{
		/* For.Prototype_Component_Particle_LandingFlame */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Particle_LandingFlame"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Landing/LandingFlame/")))))
			throw TEXT("Prototype_Component_Particle_LandingFlame");

		/* For.Prototype_Component_Particle_LandingFog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Particle_LandingFog"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Landing/LandingFog/")))))
			throw TEXT("Prototype_Component_Particle_LandingFog");

		/* For.Prototype_Component_Particle_LandingLight */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Particle_LandingLight"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Landing/LandingLight/")))))
			throw TEXT("Prototype_Component_Particle_LandingLight");

		/* Prototype_GameObject_EnergyBall */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_EnergyBall"),
			CEnergyBall::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_EnergyBall");

		/* Prototype_GameObject_Breath */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Breath"),
			CBreath::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Breath");

		/* Prototype_GameObject_Pulse */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pulse"),
			CPulse::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Pulse");

		/* For.Prototype_GameObject_Projectile_White*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Projectile_White_Effect"),
			CProjectile_White_Effect::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Projectile_White_Effect");

		/* For.Prototype_GameObject_Event_Spawn */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_RedSky"),
			CRedSky::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_RedSky");
		
		/* For.Prototype_GameObject_Event_Spawn */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Spawn"),
			CEvent_Spawn::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Event_Spawn");

		/* For.Prototype_GameObject_Event_Enter_Sanctum */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Enter_Sanctum"),
			CEvent_Enter_Sanctum::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Event_Enter_Sanctum");

		/* For.Prototype_GameObject_Event_Spawn_Dragon */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Spawn_Dragon"),
			CEvent_Spawn_Dragon::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Event_Spawn_Dragon");

		/* For.Prototype_GameObject_Event_Spawn_Dragon_2 */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Spawn_Dragon_2"),
			CEvent_Spawn_Dragon_2::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Event_Spawn_Dragon_2");

		/* For.Prototype_GameObject_Test_Particle */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MapParticle_Sanctum_CircularRocks01"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/MapParticle/Sanctum/CircularRocks01"), eLevelID))))
			throw TEXT("Prototype_GameObject_MapParticle_Sanctum_CircularRocks01");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg01"),
			CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/SummonRituals/DarkWizardSpawnToConjuredDragonEgg01.ME"), eLevelID))))
			throw TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg01");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg02"),
			CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/SummonRituals/DarkWizardSpawnToConjuredDragonEgg02.ME"), eLevelID))))
			throw TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg02");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg03"),
			CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/SummonRituals/DarkWizardSpawnToConjuredDragonEgg03.ME"), eLevelID))))
			throw TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg03");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg04"),
			CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/SummonRituals/DarkWizardSpawnToConjuredDragonEgg04.ME"), eLevelID))))
			throw TEXT("Prototype_GameObject_MapEffect_Sanctum_DarkWizardSpawnToConjuredDragonEgg04");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Static(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try
	{
#pragma region Load UI
		{
			std::lock_guard<std::mutex> lock(mtx);

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_HP"),
				CUI_Group_HP::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_HP");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_UI_Progress"),
				CUI_Progress::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_UI_Progress");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Potion"),
				CUI_Group_Potion::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Potion");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Finisher"),
				CUI_Group_Finisher::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Finisher");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Skill"),
				CUI_Group_Skill::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Skill");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
				CUI_Group_Finisher_Icon::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Finisher_Icon");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"),
				CUI_Group_Enemy_HP::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Enemy_HP");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Font"),
				CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")), true)))
				throw TEXT("Prototype_GameObject_UI_Font");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Cursor"),
				CUI_Group_Cursor::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Cursor");
			/*if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_MiniMap"),
				CUI_Group_MiniMap::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_MiniMap");*/
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_UI_Dynamic_Back"),
				CUI_Dynamic_Back::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_UI_Dynamic_Back");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_UI_Image"),
				CUI_Image::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_UI_Image");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_UI_Button"),
				CUI_Button::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_UI_Button");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Field_Guide"),
				CField_Guide::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Field_Guide");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Main_Menu"),
				CMain_Menu::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Main_Menu");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Menu_Gear"),
				CMenu_Gear::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Menu_Gear");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Menu_Inventory"),
				CMenu_Inventory::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Menu_Inventory");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Menu_Quest"),
				CMenu_Quest::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Menu_Quest");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Menu_Setting"),
				CMenu_Setting::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Menu_Setting");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Inventory"),
				CUI_Inventory::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Inventory");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Inventory"),
				CInventory::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Inventory");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Slot"),
				CUI_Slot::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Slot");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Potion_Tap"),
				CPotionTap::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Potion_Tap");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Potion_Tap"),
				CUI_Potion_Tap::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Potion_Tap");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_Brew"),
				CUI_Group_Brew::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_Brew");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Group_SkillTap"),
				CUI_Group_SkillTap::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Group_SkillTap");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Farming"),
				CUI_Farming::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Farming");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Damage"),
				CUI_Damage::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Damage");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Store"),
				CUI_Store::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("Prototype_GameObject_UI_Store");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Interaction"),
				CUI_Interaction::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Interaction");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Script"),
				CScript::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Script");


			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Timer"),
				CUI_Group_Timer::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Timer");
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_UI_Score"),
				CUI_Group_Score::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_UI_Score");
#pragma endregion

#pragma region Guide_Book
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Guide_Book"),
				CGuide_Book::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_GameObject_Guide_Book");

			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Guide_Book"),
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM,
					TEXT("../../Resources/Models/Anims/Anim_GuideBook/Anim_GuideBook.dat")))))
				throw TEXT("Prototype_Component_Model_Guide_Book");
#pragma endregion
		}

		/* Prototype_Component_Phase*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Phase"),
			CPhase::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Phase");

#pragma region Load Buffer
		/* --------------Buffer-------------- */
		/* For.Prototype_Component_VIBuffer_Cube */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Cube"),
			CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cube");

		/* Prototype_Component_VIBuffer_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Terrain"),
			CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/DT.bmp")))))
			throw TEXT("Prototype_Component_VIBuffer_Terrain");

		/* For.Prototype_Component_VIBuffer_Cloth */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Cloth"),
			CVIBuffer_Cloth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cloth");

		/* For.Prototype_Component_VIBuffer_Point_Instance */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
			CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Point_Instance");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_ConvexMesh"),
			CConvexMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_ConvexMesh");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_TriangleMesh"),
			CTriangleMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_TriangleMesh");
#pragma endregion

#pragma region Load Shader
		/* --------------Shader-------------- */
		/* For.Prototype_Component_Shader_VtxCube */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxCube"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXPOSCUBE_DECL::Elements, VTXPOSCUBE_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxCube");

		/* For.Prototype_Component_Shader_VtxAnimMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxAnimMesh");

		/* For.Prototype_Component_Shader_VtxMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxMesh");

		/* For.Prototype_Component_Shader_ShadowAnimMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_ShadowAnimMesh");

		/* For.Prototype_Component_Shader_ShadowMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_ShadowMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowMesh.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_ShadowMesh");

		/* For.Prototype_Component_Shader_VtxPointInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxPointInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"),
				VTXPOINTINSTANCE_DECL::Elements, VTXPOINTINSTANCE_DECL::iNumElements))))
			return E_FAIL;

		/* For.Prototype_Component_Shader_MeshInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"),
				VTXMESHINSTANCE_DECL::Elements, VTXMESHINSTANCE_DECL::iNumElements))))
			return E_FAIL;

		/* For.Prototype_Component_Shader_ShadowMeshInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_ShadowMeshInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowMeshInstance.hlsl"),
				VTXMESHINSTANCE_DECL::Elements, VTXMESHINSTANCE_DECL::iNumElements))))
			return E_FAIL;

		/* For.Prototype_Component_Shader_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_Terrain"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Terrain");

		/* For.Prototype_Component_Shader_Debug */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_Debug"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Debug");

		/* For.Prototype_Component_Shader_Water */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_Water"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Water.hlsl"), VTXPOSTEX_DECL::Elements, VTXPOSTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Water");

		/* Prototype_Component_Shader_DefaultEffect */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_DefaultEffect"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_DefaultEffect");
#pragma endregion

#pragma region Load ETC

		/* --------------ETC-------------- */
		/* For.Prototype_Component_Health*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Health"),
			CHealth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Health");

		/* For.Prototype_Component_RigidBody */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RigidBody"),
			CRigidBody::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RigidBody");

		/* For.Prototype_Component_RadialBlur */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RadialBlur"),
			CRadialBlur::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RadialBlur");

		/* Prototype_Component_Model_Shpere */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Shpere"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/SM_SpherePrimitiveRegularNormals_01.dat")))))
			throw TEXT("Prototype_Component_Model_Shpere");

		/* Prototype_Component_Model_Cylinder */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Cylinder"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/VFX_SM_Cylinder_1m/VFX_SM_Cylinder_1m.dat")))))
			throw TEXT("Prototype_Component_Model_Cylinder");

		/* Prototype_GameObject_Cylinder */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Cylinder"),
			CCylinder::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Cylinder");

		/* For.Prototype_GameObject_SmithToCliff_Gate_Portal */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_SmithToCliff_Gate_Portal"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Portal/Door"), eLevelID))))
			throw TEXT("Prototype_GameObject_SmithToCliff_Gate_Portal");

		/* For.Prototype_GameObject_Vault_Gate_Portal */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Vault_Gate_Portal"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Portal/Vault_Door"), eLevelID))))
			throw TEXT("Prototype_GameObject_Vault_Gate_Portal");

		/* For.Prototype_GameObject_LightStand_Fire */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_LightStand_Fire"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/LightStand_Fire"), eLevelID))))
			throw TEXT("Prototype_GameObject_LightStand_Fire");
		
#pragma endregion

#pragma region Load Player_Effect

		/*For_Player_WandHeadLight*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_WandHead_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/WandLight"), eLevelID))))
			throw TEXT("Prototype_GameObject_WandHead_Particle");

		/*For.Player_Hit*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Player_Hit_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PlayerHit/HitParticle"), eLevelID))))
			throw TEXT("Prototype_GameObject_Player_Hit_Particle");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Player_Hit_Fog")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PlayerHit/HitFog"), eLevelID))))
			throw TEXT("Prototype_GameObject_Player_Hit_Fog");
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Player_Hit_Circle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/PlayerHit/HitCircle"), eLevelID))))
			throw TEXT("Prototype_GameObject_Player_Hit_Circle");

#pragma endregion

		/*For.Test_LandingParticle*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Landing_Fog")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Landing/LandingFog"), eLevelID))))
			throw TEXT("Prototype_GameObject_Landing_Fog");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Landing_Light")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Landing/LandingLight"), eLevelID))))
			throw TEXT("Prototype_GameObject_Landing_Light");
		
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_House_FireEffect")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/House_Fire/MainFire"), eLevelID))))
			throw TEXT("Prototype_GameObject_House_FireEffect");
		
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_House_Fire_Distortion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/House_Fire/Distortion"), eLevelID))))
			throw TEXT("Prototype_GameObject_House_Fire_Distortion");
		
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_House_Fire_Smoke")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/House_Fire/Smoke"), eLevelID))))
			throw TEXT("Prototype_GameObject_House_Fire_Smoke");

#pragma region Potion_Effect

		/*For_Player_Potion*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Heal_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Potion/Wiggenweld"), eLevelID))))
			throw TEXT("Prototype_GameObject_Heal_Particle");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Heal_After_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Potion/Wig_After"), eLevelID))))
			throw TEXT("Prototype_GameObject_Heal_After_Particle");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Heal_After_Particle2")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Potion/Wig_After2"), eLevelID))))
			throw TEXT("Prototype_GameObject_Heal_After_Particle2");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Heal_Ground")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Potion/Wig_Ground"), eLevelID))))
			throw TEXT("Prototype_GameObject_Heal_Ground");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Maxima_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Potion/Maxima"), eLevelID))))
			throw TEXT("Prototype_GameObject_Maxima_Particle");
		
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Potion_Distortoin")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Potion/Potion_Distortion.ME"), eLevelID))))
			throw TEXT("Prototype_GameObject_Potion_Distortoin");

#pragma endregion

#pragma region Load Magic
		/* --------------Magic-------------- */
		/* For. Prototype_Component_Magic*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Magic"),
			CMagic::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Magic");

		/* For.Prototype_GameObject_BasicCast */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_BasicCast"),
			CBasicCast::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_BasicCast");

		/* For.Prototype_GameObject_Protego */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Protego"),
			CProtego::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Protego");

		/* For.Prototype_GameObject_Revelio */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Revelio"),
			CRevelio::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Revelio");

		/* For.Prototype_GameObject_Wingardiumleviosa */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Wingardiumleviosa"),
			CWingardiumleviosa::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Wingardiumleviosa");

		/* For.Prototype_GameObject_Levioso */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Levioso"),
			CLevioso::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Levioso");

		/* For.Prototype_GameObject_Confringo */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Confringo"),
			CConfringo::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Confringo");

		/* For.Prototype_GameObject_Finisher */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Finisher"),
			CFinisher::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Finisher");

		/* For.Prototype_GameObject_Ncendio */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Ncendio"),
			CNcendio::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Ncendio");

		/* For.Prototype_GameObject_Lumos */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Lumos"),
			CLumos::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Lumos");

		/* For.Prototype_GameObject_Arrestomomentum */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Arrestomomentum"),
			CArrestomomentum::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Arrestomomentum");

		/* For.Prototype_GameObject_Descendo */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Descendo"),
			CDescendo::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Descendo");

		/* For.Prototype_GameObject_Accio */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Accio"),
			CAccio::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Accio");

		/* For.Prototype_GameObject_Flipendo */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Flipendo"),
			CFlipendo::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Flipendo");

		/* For.Prototype_GameObject_Expelliarmus */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Expelliarmus"),
			CExpelliarmus::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Expelliarmus");

		/* For.Prototype_GameObject_Imperio */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Imperio"),
			CImperio::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Imperio");

		/* For.Prototype_GameObject_Crucio */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Crucio"),
			CCrucio::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Crucio");

		/* For.Prototype_GameObject_Stupefy */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Stupefy"),
			CStupefy::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Stupefy");

		/* For.Prototype_GameObject_Diffindo */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Diffindo"),
			CDiffindo::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Diffindo");

		/* For.Prototype_GameObject_Bombarda */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Bombarda"),
			CBombarda::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Bombarda");

		/* For.Prototype_GameObject_Blink_Trail */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Blink_Trail"),
			CBlink_Effect::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Blink_Trail");

		/* For.Prototype_GameObject_Projectile_Black */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Projectile_Black"),
			CProjectile_Black::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Projectile_Black");

		/* For.Prototype_GameObject_Projectile_White */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Projectile_White"),
			CProjectile_White::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Projectile_White");

		/* For.Prototype_GameObject_Pensive_Ground_Ball */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pensive_Ground_Ball"),
			CPensive_Ground_Ball::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Pensive_Ground_Ball");

		/* For.Prototype_GameObject_Pensive_Fail_Ball */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pensive_Fail_Ball"),
			CPensive_Fail_Ball::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Pensive_Fail_Ball");

		/* For.Prototype_GameObject_CPensive_Shouting */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pensive_Shouting"),
			CPensive_Shouting::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Pensive_Shouting");

		/* For.Prototype_GameObject_Pensive_Sword_Throw */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pensive_Sword_Throw"),
			CPensive_Sword_Throw::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Pensive_Sword_Throw");

		/* For.Prototype_GameObject_Pensive_Mace_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Pensive_Mace_Attack"),
			CPensive_Mace_Attack::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Pensive_Mace_Attack");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Monster_DarkFlare_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Monster_DarkFlare/"), eLevelID))))
			throw TEXT("Prototype_Monster_DarkFlare_Particle");

		{
			CMagicBallPool* pMagicBallPool = CMagicBallPool::GetInstance();
			Safe_AddRef(pMagicBallPool);
			if (FAILED(pMagicBallPool->Initialize()))
				throw TEXT("Magic Ball Pool");
			Safe_Release(pMagicBallPool);
		}

		/* For.Prototype_GameObject_MagicSlot */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MagicSlot"),
			CMagicSlot::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_MagicSlot");

		/* For.Prototype_GameObject_Magic_Sound_Manager */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Magic_Sound_Manager"),
			CMagic_Sound_Manager::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Magic_Sound_Manager");

#pragma endregion

#pragma region Load Particle
		{
			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust01"),
				TEXT("../../Resources/GameData/ParticleData/Misc/Dust01/"), 3)))
				throw TEXT("Reserve Particle : Particle_Dust01");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust02"),
				TEXT("../../Resources/GameData/ParticleData/Misc/Dust02/"), 3)))
				throw TEXT("Reserve Particle : Particle_Dust02");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_RockChunksRough"),
				TEXT("../../Resources/GameData/ParticleData/Misc/RockChunksRough/"), 3)))
				throw TEXT("Reserve Particle : Particle_RockChunksRough");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Dash_Gas_Splash"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Dash/Gas_Splash/"), 3)))
				throw TEXT("Reserve Particle : Particle_Golem_Dash_Gas_Splash");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Dash_Gas_Twinkle"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Dash/Gas_Twinkle/"), 3)))
				throw TEXT("Reserve Particle : Particle_Golem_Dash_Gas_Twinkle");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Hit"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Hit/"), 3)))
				throw TEXT("Reserve Particle : Particle_Golem_Hit");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Golem_Slash_Trace_Twinkle"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Golem_Slash/Trace_Twinkle/"), 3)))
				throw TEXT("Reserve Particle : Particle_Golem_Slash_Trace_Twinkle");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Hammer_Trace_Rock"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Hammer_Trace_Rock/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Hammer_Trace_Rock");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Hand_Black"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Hand_Black/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Hand_Black");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Dust"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Dust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Stone"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Stone/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Stone");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Dust_Mini"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Dust_Mini/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Dust_Mini");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_Stone_Mini"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_Stone_Mini/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_TwoHand_Stone_Mini");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_TwoHand_StoneRain"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_TwoHand_StoneRain/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_TwoHand_StoneRain");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Foot_Circle_Dust"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Foot_Circle_Dust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Foot_Circle_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Landing_Dust"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Landing_Dust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Landing_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Toe_Landing_Dust"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Toe_Landing_Dust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Toe_Landing_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Weapon_Trace"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Troll_Weapon_Trace/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Weapon_Trace");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DugBog_Chim"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DugBog_Chim/"), 3)))
				throw TEXT("Reserve Particle : Particle_DugBog_Chim");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Trace_Rock"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Trace_Rock/"), 3)))
				throw TEXT("Reserve Particle : Particle_DogBog_Trace_Rock");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Chim_Red"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Chim_Red/"), 3)))
				throw TEXT("Reserve Particle : Particle_DogBog_Chim_Red");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Chim_Blue"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Chim_Blue/"), 3)))
				throw TEXT("Reserve Particle : Particle_DogBog_Chim_Blue");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Trace_Water"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Trace_Water/"), 3)))
				throw TEXT("Reserve Particle : Particle_DogBog_Trace_Water");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DogBog_Water_Foot"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/DogBog_Water_Foot/"), 3)))
				throw TEXT("Reserve Particle : Particle_DogBog_Water_Foot");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Dust_Hit"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Monster_Hit/Troll_Dust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Dust_Hit");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Troll_Stone_Hit"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Monster_Hit/Troll_Stone/"), 3)))
				throw TEXT("Reserve Particle : Particle_Troll_Stone_Hit");
			/*For.Player_Effect*/
			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Effect"),
				TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFog/"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Effect");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Effect1"),
				TEXT("../../Resources/GameData/ParticleData/Blink/BlinkFog1/"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Effect1");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Distortion"),
				TEXT("../../Resources/GameData/ParticleData/Blink/BlinkDistortion/"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Distortion");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Particle"),
				TEXT("../../Resources/GameData/ParticleData/Blink/BlinkParticle/"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Particle");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Blink_Ring"),
				TEXT("../../Resources/GameData/ParticleData/Blink/BlinkRing"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Blink_Ring");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Player_Hit_Dust"),
				TEXT("../../Resources/GameData/ParticleData/PlayerHit/HitDust/"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Player_Hit_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_WandLine"),
				TEXT("../../Resources/GameData/ParticleData/BasicCast/WandLine"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_WandLine");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Prototype_GameObject_Finisher_WandLine"),
				TEXT("../../Resources/GameData/ParticleData/Lightning/WandLine"))))
				throw TEXT("Reserve Particle : Prototype_GameObject_Finisher_WandLine");

			/*For.Pensive_Effect*/
			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Circle"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Circle/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Hit_Circle");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Distotion"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Distotion/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Hit_Distotion");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Spread"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Spread/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Hit_Spread");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Hit_Stick_Glow"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Hit/Stick_Glow/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Hit_Stick_Glow");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Black_Water"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Black_Water/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Black_Water");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Bubble"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Bubble/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Bubble");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Main"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Main/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Main");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Splash_01"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Splash_01/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Splash_01");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Water_Splash_02"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Water_Splash_02/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Water_Splash_02");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Pensive_Appear_Flare"),
				TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Appear/Flare/"), 3)))
				throw TEXT("Reserve Particle : Particle_Pensive_Appear_Flare");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Conjured_Step_Dust"),
				TEXT("../../Resources/GameData/ParticleData/BoneDragon/StepDust/"), 3)))
				throw TEXT("Reserve Particle : Particle_Conjured_Step_Dust");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Conjured_FallDown_Dust_01"),
				TEXT("../../Resources/GameData/ParticleData/Misc/FallDownDust01/"), 1)))
				throw TEXT("Reserve Particle : Particle_Conjured_FallDown_Dust_01");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Conjured_FallDown_Dust_02"),
				TEXT("../../Resources/GameData/ParticleData/Misc/FallDownDust02/"), 1)))
				throw TEXT("Reserve Particle : Particle_Conjured_FallDown_Dust_02");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Blue"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Blue/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Blue");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Purple"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Purple/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Purple");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Red"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Red/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Red");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Yellow"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Yellow/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Yellow");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Blue2"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Blue2/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Blue2");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Purple2"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Purple2/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Purple2");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Red2"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Red2/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Red2");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_DestroyProtego_Yellow2"),
				TEXT("../../Resources/GameData/ParticleData/Portego/DestroyPortego_Yellow2/"), 2)))
				throw TEXT("Reserve Particle : Particle_DestroyProtego_Yellow2");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_CircularRocks02"),
				TEXT("../../Resources/GameData/ParticleData/MapParticle/Sanctum/CircularRocks02/"), 1)))
				throw TEXT("Reserve Particle : Particle_CircularRocks02");
				
			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Balloon_Spread"),
				TEXT("../../Resources/GameData/ParticleData/Balloon/Spread/"), 30)))
				throw TEXT("Reserve Particle : Particle_Balloon_Spread");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Balloon_Line"),
				TEXT("../../Resources/GameData/ParticleData/Balloon/Line/"), 30)))
				throw TEXT("Reserve Particle : Particle_Balloon_Line");
		}
#pragma endregion

#pragma region Load Debug

#ifdef _DEBUG
		/* --------------Debug-------------- */
		/* For.Prototype_GameObject_Camera_Debug*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Camera_Debug"),
			CCamera_Debug::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Camera_Debug");

		/* For.Prototype_GameObject_Test_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Test_Player"),
			CTest_Player::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_Player");

		/* For.Prototype_GameObject_Test_Particle */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Test_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Test/"), eLevelID))))
			throw TEXT("Prototype_GameObject_Test_Particle");

		/* For.Prototype_GameObject_PhysxRenderer */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_PhysxRenderer"),
			CPhysXRender::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_PhysxRenderer");

		/* For.Prototype_GameObject_Norm_Test */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Norm_Test"),
			CNorm_Test::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Norm_Test");

#endif // _DEBUG

#pragma endregion
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}


CMain0_Loader* CMain0_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded)
{
	CMain0_Loader* pInstance = New CMain0_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, isStaticLoaded)))
	{
		MSG_BOX("Failed to Created CMain0_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain0_Loader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
