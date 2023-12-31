#include "../Public/Main2_Loader.h"
#include "GameInstance.h"

/////////////////////////////////////////////
///////// Monster / Monster Weapon //////////
/////////////////////////////////////////////

#pragma region Monsters
#include "ConjuredDragon.h"
#include "Armored_Troll.h"
#include "DarkWizard_M.h"
#include "DarkWizard_F.h"
#include "DarkWizard_Fly.h"
#include "DarkWizard_Spawn.h"
#include "Forest_Troll.h"
#include "Golem_Combat.h"
#include "Dugbog.h"
#include "Wolf.h"
#pragma endregion Monsters

#pragma region Weapon
#include "Weapon_DarkWizard_Wand.h"
#include "Weapon_Armored_Troll.h"
#include "Weapon_Forest_Troll.h"
#include "Weapon_Golem_Combat.h"
#include "Weapon_Player_Wand.h"
#include "Weapon_Dragon_Head.h"
#include "Weapon_Dragon_Left_Wing.h"
#include "Weapon_Dragon_Right_Wing.h"
#include "Weapon_Dragon_Tail.h"
#include "Broom_Stick_DarkWizard.h"
#pragma endregion Weapon

#include "Camera_Shake.h"


#pragma region Weapon
#include "Pensive.h"
#include "Weapon_Pensive_Sword.h"
#include "Weapon_Pensive_Flail.h"

#include "StateContext_Enemy.h"
#include "Pensive_Appearence.h"
#include "Pensive_Groogy.h"
#include "Pensive_Charge_Attack.h"
#include "Pensive_Hit.h"
#include "Pensive_Death.h"
#include "Pensive_Idle.h"
#include "Pensive_Physical_Attack.h"
#include "Pensive_Orb_Attack.h"
#pragma endregion Weapon

CMain2_Loader::CMain2_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint WINAPI Thread_Main2(void* pArg)
{
	CMain2_Loader* pLoader = reinterpret_cast<CMain2_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain2_Loader::Initialize(LEVELID eNextLevel, _bool isStaticLoaded)
{
	m_eNextLevelID = eNextLevel;
	m_isStaticLoaded = isStaticLoaded;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main2, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain2_Loader::Loading()
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

HRESULT CMain2_Loader::Loading_For_Logo(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	return S_OK;
}

HRESULT CMain2_Loader::Loading_For_Cliffside(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		_float4x4 PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dugbog"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Dugbog/Dugbog.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Dugbog");

		/* For.Prototype_GameObject_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dugbog"),
			CDugbog::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dugbog");

		/* ========================== DEBUG ========================== */
		/* 밑에 코드는 몬스터의 간단한 행동을 테스트 하기 위해		   */
		/* 1스테이지에 직접 소환하는 임시 코드입니다.				    */
		/* =========================================================== */

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		PivotMatrix = XMMatrixScaling(0.45f, 0.45f, 0.45f);
		/* For.Prototype_Component_Model_Wolf */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Wolf"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Wolf/Wolf.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Wolf");

		/* For.Prototype_GameObject_Wolf */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Wolf"),
			CWolf::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Wolf");

		/* =========================================================== */
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

HRESULT CMain2_Loader::Loading_For_Vault(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		_float4x4 PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));

		/* ============ Enemy Weapon Models ============ */

		/* For.Prototype_Component_Model_Weopon_Golem_Combat */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Sword/Golem_Combat_Sword.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Golem_Combat");

		/* ============ Enemy Weapons ============ */

		/* For.Prototype_Component_Weapon_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			CWeapon_Golem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Golem_Combat");

		/* ============ Enemy Models ============ */

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_CombatGrunt/Golem_CombatGrunt.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Golem_Combat");

		/* ============ Enemies ============ */

		/* For.Prototype_GameObject_Golem_CombatGrunt */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Golem_CombatGrunt"),
			CGolem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Golem_CombatGrunt");

#pragma region Pensive
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Particle_Pensive_Skill_Distotion"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Pensive_Skill_Distotion"), eLevelID))))
			throw TEXT("Particle_Pensive_Skill_Distotion");

		/* For.Prototype_GameObject_Dragon_Head */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dragon_Head"),
			CWeapon_Dragon_Head::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dragon_Head");

		/* ============ Enemy State Partten ============ */
		/* For.Prototype_Component_StateContext_Enemy */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_StateContext_Enemy"),
			CStateContext_Enemy::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_StateContext_Enemy");

		/* For.Prototype_Component_Pensive_Physical_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Physical_Attack"),
			CPensive_Physical_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Physical_Attack");

		/* For.Prototype_Component_Pensive_Orb_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Orb_Attack"),
			CPensive_Orb_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Orb_Attack");

		/* For.Prototype_Component_Pensive_Appearence */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Appearence"),
			CPensive_Appearence::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Appearence");

		/* For.Prototype_Component_Pensive_Death */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Death"),
			CPensive_Death::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Death");

		/* For.Prototype_Component_Pensive_Charge_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Charge_Attack"),
			CPensive_Charge_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Charge_Attack");

		/* For.Prototype_Component_Pensive_Groogy */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Groogy"),
			CPensive_Groogy::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Groogy");

		/* For.Prototype_Component_Pensive_Hit */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Hit"),
			CPensive_Hit::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Hit");

		/* For.Prototype_Component_Pensive_Idle */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Pensive_Idle"),
			CPensive_Idle::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Pensive_Idle");

		/* ============ Enemy Weapon Models ============ */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Pensive_Flail"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Pensive_Flail/Pensive_Flail.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Pensive_Flail");

		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Pensive_Sword"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Pensive_Sword/Pensive_Sword.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Pensive_Sword");

		/* ============ Enemy Weapons ============ */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Pensive_Flail"),
			CWeapon_Pensive_Flail::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Pensive_Flail");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Pensive_Sword"),
			CWeapon_Pensive_Sword::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Pensive_Sword");

		/* ============ Enemy Models ============ */
		PivotMatrix = XMMatrixScaling(2.5f, 2.5f, 2.5f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Pensive"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/PensivePaladin/PensivePaladin.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Pensive");

		/* ============ Enemies ============ */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_PensivePaladin"),
			CPensive::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_PensivePaladin");

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

HRESULT CMain2_Loader::Loading_For_GreatHall(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain2_Loader::Loading_For_Hogsmeade(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		_float4x4 PivotMatrix = PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dugbog"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Dugbog/Dugbog.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Dugbog");

		/* For.Prototype_GameObject_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dugbog"),
			CDugbog::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dugbog");

		/* ============ Enemy Weapon Models ============ */

		/* For.Prototype_Component_Model_Weopon_Golem_Combat */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Sword/Golem_Combat_Sword.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Golem_Combat");

		/* ============ Enemy Weapons ============ */

		/* For.Prototype_Component_Weapon_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			CWeapon_Golem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Golem_Combat");

		/* ============ Enemy Models ============ */

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_CombatGrunt/Golem_CombatGrunt.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Golem_Combat");

		/* ============ Enemies ============ */

		/* For.Prototype_GameObject_Golem_CombatGrunt */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Golem_CombatGrunt"),
			CGolem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Golem_CombatGrunt");

		/* ============ Enemy Weapon Models ============ */

		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		/* For.Prototype_Component_Model_Weopon_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Armored_Troll"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Armored_Club/Troll_Armored_Club.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Armored_Troll");

		/* ============ Enemy Weapons ============ */

		/* For.Prototype_Component_Weapon_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Armored_Troll"),
			CWeapon_Armored_Troll::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Armored_Troll");

		/* ============ Enemy Models ============ */

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Armored_Troll"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Armored_Troll/Armored_Troll.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Armored_Troll");

		/* ============ Enemies ============ */

		/* For.Prototype_GameObject_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Armored_Troll"),
			CArmored_Troll::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Armored_Troll");
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

HRESULT CMain2_Loader::Loading_For_Sanctum(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Loading_For_Sanctum Add_Prototype*/
	{
		_float4x4 PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		/* For.Prototype_Component_Model_ConjuredDragon */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_ConjuredDragon"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/SK_HebrideanBlack_Skeleton/SK_HebrideanBlack_Skeleton.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_ConjuredDragon");

		/* For.Prototype_GameObject_Dragon_Head */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dragon_Head"),
			CWeapon_Dragon_Head::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dragon_Head");

		/* For.Prototype_GameObject_Weapon_Dragon_Left_Wing */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Weapon_Dragon_Left_Wing"),
			CWeapon_Dragon_Left_Wing::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Weapon_Dragon_Left_Wing");

		///* For.Prototype_GameObject_Weapon_Dragon_Right_Wing */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Weapon_Dragon_Right_Wing"),
			CWeapon_Dragon_Right_Wing::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Weapon_Dragon_Right_Wing");

		/* For.Prototype_GameObject_Weapon_Dragon_Tail */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Weapon_Dragon_Tail"),
			CWeapon_Dragon_Tail::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Weapon_Dragon_Tail");

		/* For.Prototype_GameObject_ConjuredDragon */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_ConjuredDragon"),
			CConjuredDragon::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_ConjuredDragon");

		/* For.Prototype_Component_Model_Weopon_DarkWizard_Wand */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_DarkWizard_Wand"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/DarkWizard_Wand/DarkWizard_Wand.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_DarkWizard_Wand");

		/* For.Prototype_Component_Weapon_DarkWizard_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_DarkWizard_Wand"),
			CWeapon_DarkWizard_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_DarkWizard_Wand");

		/* For.Prototype_Component_Model_DarkWizard_M */
		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DarkWizard_M"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/DarkWizard_M/DarkWizard_M.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_DarkWizard_M");

		/* For.Prototype_GameObject_DarkWizard_M */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_DarkWizard_M"),
			CDarkWizard_M::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_DarkWizard_M");

		/* For.Prototype_Component_Model_DarkWizard_F */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DarkWizard_F"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/DarkWizard_F/DarkWizard_F.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_DarkWizard_F");

		/* For.Prototype_GameObject_DarkWizard_F */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_DarkWizard_F"),
			CDarkWizard_F::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_DarkWizard_F");

		/* For.Prototype_GameObject_DarkWizard_Spawn */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_DarkWizard_Spawn"),
			CDarkWizard_Spawn::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_DarkWizard_Spawn");

		/* For.Prototype_Component_Model_Wolf */
		PivotMatrix = _float4x4();
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Wolf"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Wolf/Wolf.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Wolf");

		/* For.Prototype_GameObject_Wolf */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Wolf"),
			CWolf::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Wolf");

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

HRESULT CMain2_Loader::Loading_For_Sky(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try
	{
		/* For.Prototype_Component_Model_Weopon_DarkWizard_Wand */
		_float4x4 PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Dugbog"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Dugbog/Dugbog.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Dugbog");

		/* For.Prototype_GameObject_Dugbog */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Dugbog"),
			CDugbog::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dugbog");

		/* ============ Enemy Weapon Models ============ */

		/* For.Prototype_Component_Model_Weopon_Golem_Combat */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Sword/Golem_Combat_Sword.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Golem_Combat");

		/* ============ Enemy Weapons ============ */

		/* For.Prototype_Component_Weapon_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			CWeapon_Golem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Golem_Combat");

		/* ============ Enemy Models ============ */

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

		/* For.Prototype_Component_Model_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_CombatGrunt/Golem_CombatGrunt.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Golem_Combat");

		/* ============ Enemies ============ */

		/* For.Prototype_GameObject_Golem_CombatGrunt */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Golem_CombatGrunt"),
			CGolem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Golem_CombatGrunt");

		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Weopon_DarkWizard_Wand"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/DarkWizard_Wand/DarkWizard_Wand.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_DarkWizard_Wand");

		/* For.Prototype_GameObject_Weapon_DarkWizard_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Weapon_DarkWizard_Wand"),
			CWeapon_DarkWizard_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Weapon_DarkWizard_Wand");

		/* For.Prototype_Component_Model_Broom_Stick_DarkWizard */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_Broom_Stick_DarkWizard"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/DarkWizardBroom/DarkWizardBroom.dat")))))
			throw TEXT("Prototype_Component_Model_Broom_Stick_DarkWizard");

		/* For.Prototype_GameObject_Broom_Stick_DarkWizard */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Broom_Stick_DarkWizard"),
			CBroom_Stick_DarkWizard::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Broom_Stick_DarkWizard");

		/* For.Prototype_Component_Model_DarkWizard_Fly */
		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DarkWizard_Fly"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/DarkWizard_Fly/DarkWizard_Fly.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_DarkWizard_Fly");

		/* For.Prototype_GameObject_DarkWizard_Fly */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_DarkWizard_Fly"),
			CDarkWizard_Fly::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_DarkWizard_Fly");

		/* For.Prototype_GameObject_Weapon_DarkWizard_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Weapon_DarkWizard_Wand"),
			CWeapon_DarkWizard_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Weapon_DarkWizard_Wand");

		/* For.Prototype_Component_Model_DarkWizard_M */
		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Model_DarkWizard_M"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/DarkWizard_M/DarkWizard_M.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_DarkWizard_M");

		/* For.Prototype_GameObject_DarkWizard_M */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_DarkWizard_M"),
			CDarkWizard_M::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_DarkWizard_M");

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

HRESULT CMain2_Loader::Loading_For_Static(LEVELID eLevelID)
{

	try 
	{
		/* For.Prototype_Component_Enemy_Camera_Shake*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			CCamera_Shake::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Enemy_Camera_Shake");
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

CMain2_Loader* CMain2_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded)
{
	CMain2_Loader* pInstance = New CMain2_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, isStaticLoaded)))
	{
		MSG_BOX("Failed to Created CMain2_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain2_Loader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
