#include "..\Public\Main3_Loader.h"
#include "GameInstance.h"

/////////////////////////////////////////////
///////// Player / Player etc / NPC /////////
/////////////////////////////////////////////

#include "Player.h"
#include "Weapon_Player_Wand.h"

#include "Professor_FIg.h"
#include "Weapon_Fig_Wand.h"

#pragma region Player State

#include "StateContext.h"
#include "IdleState.h"
#include "MoveTurnState.h"
#include "MoveStartState.h"
#include "MoveLoopState.h"
#include "RollState.h"
#include "JumpState.h"
#include "HardLandState.h"
#include "MagicCastingState.h"
#include "ProtegoState.h"
#include "HitState.h"
#include "StandingState.h"
#include "UseItemState.h"

//#include "Fly_Move.h"
//#include "Fly_Turn.h"
//#include "Hover_Idle.h"
//#include "Hover_Move.h"
//#include "Hover_Turn.h"
//#include "Broom_Begin.h"
//#include "Broom_End.h"
//#include "Broom_Break.h"
#pragma endregion


#include "Player_Information.h"
#include "CoolTime.h"
#include "Animation_Camera_Model.h"
#include "Defence.h"

#pragma region Items

#include "FocusPotion.h"
#include "MaximaPotion.h"
#include "EdurusPotion.h"
#include "InvisibilityPotion.h"
#include "WiggenweldPotion.h"
#include "ThunderBrewPotion.h"

#include "FocusPotion_Item.h"
#include "MaximaPotion_Item.h"
#include "EdurusPotion_Item.h"
#include "InvisibilityPotion_Item.h"
#include "ThunderBrewPotion_Item.h"
#include "WiggenweldPotion_Item.h"

#include "AshwinderEggs_Item.h"
#include "Dittany_Leaves_Item.h"
#include "Dugbog_Tongue_Item.h"
#include "Leaping_Toadstool_Caps_Item.h"
#include "Lacewing_Flies_Item.h"
#include "Knotgrass_Item.h"
#include "Horklump_Juice_Item.h"
#include "Leech_Juice_Item.h"
#include "MallowSweet_Item.h"
#include "MoonStone_Item.h"
#include "Fluxweed_Stem_Item.h"
#include "Spider_Fang_Item.h"
#include "Troll_Bogeys_Item.h"
#include "Shrivelfig_Item.h"
#include "Mongrel_Fur_Item.h"
#pragma endregion


CMain3_Loader::CMain3_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint WINAPI Thread_Main3(void* pArg)
{
	CMain3_Loader* pLoader = reinterpret_cast<CMain3_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain3_Loader::Initialize(LEVELID eNextLevel, _bool isStaticLoaded)
{
	m_eNextLevelID = eNextLevel;
	m_isStaticLoaded = isStaticLoaded;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main3, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain3_Loader::Loading()
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

HRESULT CMain3_Loader::Loading_For_Logo(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_Cliffside(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		
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

HRESULT CMain3_Loader::Loading_For_Vault(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_GreatHall(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_Hogsmeade(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_Sky(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_Sanctum(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_Static(LEVELID eLevelID)
{
	try
	{
#pragma region Player
		/* For.Prototype_GameObject_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Player");

		_float4x4 PivotMatrix;
		PivotMatrix = XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.f), 0.f));
		/* For.Prototype_Component_Model_CustomModel_Player */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_CustomModel_Player"),
			CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, L"../../Resources/Models/Anims/Biped_Skeleton_jog_Idle/Biped_Skeleton_jog_Idle.gcm", PivotMatrix))))
			throw TEXT("Prototype_Component_Model_CustomModel_Player");

		/* For.Prototype_Component_Weapon_Player_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Player_Wand"),
			CWeapon_Player_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Player_Wand");

		PivotMatrix *= XMMatrixScaling(0.5f, 0.5f, 0.5f);
		/* For.Prototype_Component_Model_Animation_Camera */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Animation_Camera"),
			CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, L"../../Resources/Models/Anims/CCL_CameraRig_Skeleton/CCL_CameraRig_Skeleton.gcm", PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Animation_Camera");

		/* For.Prototype_GameObject_Animation_Camera_Model */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Animation_Camera_Model"),
			CAnimation_Camera_Model::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Animation_Camera_Model");

#pragma endregion

#pragma region Player Parts
		/* For.Prototype_Component_MeshPart_Player_Hair */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Hair"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Hair/Player_Hair.dat"), TEXT("Player_Hair")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Hair");

		/* For.Prototype_Component_MeshPart_Player_Head */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Head"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Head/Player_Head.dat"), TEXT("Player_Head")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Head");

		/* For.Prototype_Component_MeshPart_Player_Arm */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Arm"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Arm/Player_Arm.dat"), TEXT("Player_Arm")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Arm");

		/* For.Prototype_Component_MeshPart_Robe01 */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Robe01"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Robe_Student/Robe_Student.dat"), TEXT("Robe01")))))
			throw TEXT("Prototype_Component_MeshPart_Robe01");

		/* For.Prototype_Component_MeshPart_Player_Top */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Top"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Top/Player_Top.dat"), TEXT("Player_Top")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Top");

		/* For.Prototype_Component_MeshPart_Player_Pants */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Pants"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Pants/Player_Pants.dat"), TEXT("Player_Pants")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Pants");

		/* For.Prototype_Component_MeshPart_Player_Socks */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Socks"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Socks/Player_Socks.dat"), TEXT("Player_Socks")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Socks");

		/* For.Prototype_Component_MeshPart_Player_Shoes */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MeshPart_Player_Shoes"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Shoes/Player_Shoes.dat"), TEXT("Player_Shoes")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Shoes");

#pragma endregion

#pragma region Player State
		/* For.Prototype_Component_StateContext */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_StateContext"),
			CStateContext::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_StateContext");

		/* For.Prototype_Component_State_Idle */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Idle"),
			CIdleState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Idle");

		/* For.Prototype_Component_State_Move_Turn */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Move_Turn"),
			CMoveTurnState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Turn");

		/* For.Prototype_Component_State_Move_Start */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Move_Start"),
			CMoveStartState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Start");

		/* For.Prototype_Component_State_Move_Loop */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Move_Loop"),
			CMoveLoopState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Loop");

		/* For.Prototype_Component_State_Roll */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Roll"),
			CRollState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Roll");

		/* For.Prototype_Component_State_Jump */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Jump"),
			CJumpState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Jump");

		/* For.Prototype_Component_State_Hard_Land */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Hard_Land"),
			CHardLandState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Hard_Land");

		/* For.Prototype_Component_State_Magic_Casting */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Magic_Casting"),
			CMagicCastingState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Magic_Casting");

		/* For.Prototype_Component_State_ProtegoState */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_ProtegoState"),
			CProtegoState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_ProtegoState");

		/* For.Prototype_Component_State_Hit */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Hit"),
			CHitState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Hit");

		/* For.Prototype_Component_State_Standing */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Standing"),
			CStandingState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Standing");

		/* For.Prototype_Component_State_UseItem */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_UseItem"),
			CUseItemState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_UseItem");

		///* For.Prototype_Component_State_Fly_Turn */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Fly_Turn"),
		//	CFly_Turn::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Fly_Turn");

		///* For.Prototype_Component_State_Fly_Move */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Fly_Move"),
		//	CFly_Move::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Fly_Move");

		///* For.Prototype_Component_State_Hover_Idle */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Hover_Idle"),
		//	CHover_Idle::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Hover_Idle");

		///* For.Prototype_Component_State_Hover_Move */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Hover_Move"),
		//	CHover_Move::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Hover_Move");

		///* For.Prototype_Component_State_Hover_Turn */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Hover_Turn"),
		//	CHover_Turn::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Hover_Turn");

		///* For.Prototype_Component_State_Broom_Begin */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Broom_Begin"),
		//	CBroom_Begin::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Broom_Begin");

		///* For.Prototype_Component_State_Broom_End */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Broom_End"),
		//	CBroom_End::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Broom_End");

		///* For.Prototype_Component_State_Broom_Break */
		//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_State_Broom_Break"),
		//	CBroom_Break::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_State_Broom_Break");

#pragma endregion

#pragma region Player_Information


		/* For.Prototype_Component_Player_Health */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Player_Health"),
			CHealth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Player_Health");

		/* For.Prototype_Component_Player_Information */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Player_Information"),
			CPlayer_Information::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Player_Information");

#pragma endregion

#pragma region Load Fig
		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		/* For.Prototype_Component_Model_Professor_Fig */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Professor_Fig"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Professor_Fig/Professor_Fig.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Professor_Fig");

		/* For.Prototype_GameObject_Professor_Fig */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Professor_Fig"),
			CProfessor_Fig::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Professor_Fig");

		/* For.Prototype_Component_Model_Weopon_Fig_Wand */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Fig_Wand"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Wand_Fig/Wand_Fig.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Fig_Wand");

		/* For.Prototype_Component_Weapon_Fig_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Fig_Wand"),
			CWeapon_Fig_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Fig_Wand");
#pragma endregion

#pragma region Load Potion
		{
			std::lock_guard<std::mutex> lock(mtx);

			/* For.Prototype_GameObject_WiggenweldPotion*/
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_WiggenweldPotion"),
				CWiggenweldPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("Prototype_GameObject_WiggenweldPotion");

			/* For.Prototype_GameObject_FocusPotion*/
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_FocusPotion"),
				CFocusPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("Prototype_GameObject_FocusPotion");

			/* For.Prototype_GameObject_MaximaPotion */
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MaximaPotion"),
				CMaximaPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("ProtoType_GameObject_MaximaPotion");

			/* For.Prototype_GameObject_EdurusPotion */
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_EdurusPotion"),
				CEdurusPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("ProtoType_GameObject_EdurusPotion");

			/* For.Prototype_GameObject_InvisibilityPotion */
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_InvisibilityPotion"),
				CInvisibilityPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("Prototype_GameObject_InvisibilityPotion");

			/* For.Prototype_GameObject_ThunderBrewPotion */
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_ThunderBrewPotion"),
				CThunderBrewPotion::Create(m_pDevice, m_pContext, eLevelID))))
				throw TEXT("Prototype_GameObject_ThunderBrewPotion");
		}
#pragma endregion

#pragma region Load Item
		/* For.Prototype_GameObject_WiggenweldPotion_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_WiggenweldPotion_Item"),
			CWiggenweldPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_WiggenweldPotion_Item");

		/* For.Prototype_GameObject_FocusPotion_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_FocusPotion_Item"),
			CFocusPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_FocusPotion_Item");

		/* For.Prototype_GameObject_MaximaPotion_Item */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MaximaPotion_Item"),
			CMaximaPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_MaximaPotion_Item");

		/* For.Prototype_GameObject_EdurusPotion_Item */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_EdurusPotion_Item"),
			CEdurusPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_EdurusPotion_Item");

		/* For.Prototype_GameObject_InvisibilityPotion_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_InvisibilityPotion_Item"),
			CInvisibilityPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_InvisibilityPotion_Item");

		/* For.Prototype_GameObject_ThunderBrewPotion_Item */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_ThunderBrewPotion_Item"),
			CThunderBrewPotion_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_ThunderBrewPotion_Item");

		/* For.Prototype_GameObject_AshwinderEggs_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_AshwinderEggs_Item"),
			CAshwinderEggs_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_AshwinderEggs_Item");

		/* For.Prototype_GameObject_Dittany_Leaves_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dittany_Leaves_Item"),
			CDittany_Leaves_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Dittany_Leaves_Item");

		/* For.Prototype_GameObject_Dugbog_Tongue_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dugbog_Tongue_Item"),
			CDugbog_Tongue_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Dugbog_Tongue_Item");

		/* For.Prototype_GameObject_Leaping_Toadstool_Caps_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Leaping_Toadstool_Caps_Item"),
			CLeaping_Toadstool_Caps_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Leaping_Toadstool_Caps_Item");

		/* For.Prototype_GameObject_AshwinderEggs_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Lacewing_Flies_Item"),
			CLacewing_Flies_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Lacewing_Flies_Item");

		/* For.Prototype_GameObject_Knotgrass_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Knotgrass_Item"),
			CKnotgrass_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Knotgrass_Item");

		/* For.Prototype_GameObject_Horklump_Juice_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Horklump_Juice_Item"),
			CHorklump_Juice_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Horklump_Juice_Item");

		/* For.Prototype_GameObject_Leech_Juice_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Leech_Juice_Item"),
			CLeech_Juice_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Leech_Juice_Item");

		/* For.Prototype_GameObject_MallowSweet_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MallowSweet_Item"),
			CMallowSweet_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_MallowSweet_Item");

		/* For.Prototype_GameObject_MoonStone_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_MoonStone_Item"),
			CMoonStone_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_MoonStone_Item");

		/* For.Prototype_GameObject_Fluxweed_Stem_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Fluxweed_Stem_Item"),
			CFluxweed_Stem_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Fluxweed_Stem_Item");

		/* For.Prototype_GameObject_Spider_Fang_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Spider_Fang_Item"),
			CSpider_Fang_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Spider_Fang_Item");

		/* For.Prototype_GameObject_Troll_Bogeys_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Troll_Bogeys_Item"),
			CTroll_Bogeys_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Troll_Bogeys_Item");

		/* For.Prototype_GameObject_Shrivelfig_Item*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Shrivelfig_Item"),
			CShrivelfig_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Shrivelfig_Item");

		/* For.Prototype_GameObject_Mongrel_Fur*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Mongrel_Fur"),
			CMongrel_Fur_Item::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Mongrel_Fur");
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

CMain3_Loader* CMain3_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded)
{
	CMain3_Loader* pInstance = New CMain3_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, isStaticLoaded)))
	{
		MSG_BOX("Failed to Created CMain3_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain3_Loader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
