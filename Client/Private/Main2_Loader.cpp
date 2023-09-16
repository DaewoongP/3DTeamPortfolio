#include "../Public/Main2_Loader.h"
#include "GameInstance.h"

/////////////////////////////////////////////
///////// Monster / Monster Weapon //////////
/////////////////////////////////////////////

#pragma region Monsters & NPC
#include "Armored_Troll.h"
#include "Forest_Troll.h"
#include "Golem_Combat.h"
#include "Golem_Merlin.h"
#include "Dugbog.h"

#include "Professor_FIg.h"
#pragma endregion Monsters & NPC

#pragma region Weapon
#include "Weapon_Armored_Troll.h"
#include "Weapon_Forest_Troll.h"
#include "Weapon_Golem_Combat.h"
#include "Weapon_Golem_Merlin.h"
#include "Weapon_Player_Wand.h"
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

_uint WINAPI Thread_TestMain(void* pArg)
{
	CMain2_Loader* pLoader = reinterpret_cast<CMain2_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain2_Loader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_TestMain, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain2_Loader::Loading()
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL_MAINGAME:
		hr = Loading_For_MainGame();
		break;
	default:
		MSG_BOX("Failed Load Next Level");
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain2_Loader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_isFinished = true;

	return S_OK;
}

HRESULT CMain2_Loader::Loading_For_MainGame()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	_float4x4 PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/SkySphere/SkySphere.dat")))))
		throw TEXT("Prototype_Component_Model_Sky");

	/* For.Weapon Models */
	/* For.Prototype_Component_Model_Weopon_Armored_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Armored_Troll"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Armored_Club/Troll_Armored_Club.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Armored_Troll");

	/* For.Prototype_Component_Model_Weopon_Forest_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Forest_Troll"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Forest_Club/Troll_Forest_Club.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Forest_Troll");

	/* For.Prototype_Component_Model_Weopon_Golem_Combat */
	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Golem_Combat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Sword/Golem_Combat_Sword.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Golem_Combat");

	/* For.Prototype_Component_Model_Weopon_Golem_Merlin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Golem_Merlin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Mace/Golem_Combat_Mace.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Golem_Merlin");

	///* For.Prototype_Component_Model_Weopon_Pensive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Pensive"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Pensive_Flail/Pensive_Flail.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Pensive");

	/* For.Prototype_Component_Model_Weopon_Player_Wand */
	PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixScaling(5, 5, 5);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Player_Wand"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/gaechul/gaechul.dat"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Weopon_Player_Wand");

	/* For.Monster Models */

	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	/* For.Prototype_Component_Model_Golem_Combat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Combat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_CombatGrunt/Golem_CombatGrunt.gcm"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Golem_Combat");

	/* For.Prototype_Component_Model_Golem_Merlin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Merlin"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_MerlinGrunt/Golem_MerlinGrunt.gcm"), PivotMatrix))))
		throw TEXT("Prototype_Component_Model_Golem_Merlin");

	/* For.Prototype_GameObject_Armored_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Armored_Troll"),
		CArmored_Troll::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Armored_Troll");

	/* For.Prototype_GameObject_Forest_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Forest_Troll"),
		CForest_Troll::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Forest_Troll");

	/* For.Prototype_GameObject_Dugbog */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Dugbog"),
		CDugbog::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Dugbog");

	/* For.Prototype_GameObject_Golem_CombatGrunt */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_CombatGrunt"),
		CGolem_Combat::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Golem_CombatGrunt");

	/* For.Prototype_GameObject_Golem_Merlin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Merlin"),
		CGolem_Merlin::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Golem_Merlin");

	/* For.Prototype_Component_Weapon_Armored_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Armored_Troll"),
		CWeapon_Armored_Troll::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_Component_Weapon_Armored_Troll");

	/* For.Prototype_Component_Weapon_Forest_Troll */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Forest_Troll"),
		CWeapon_Forest_Troll::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_Component_Weapon_Forest_Troll");

	/* For.Prototype_Component_Weapon_Golem_Combat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Combat"),
		CWeapon_Golem_Combat::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_Component_Weapon_Golem_Combat");

	/* For.Prototype_Component_Weapon_Golem_Merlin */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Merlin"),
		CWeapon_Golem_Merlin::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_Component_Weapon_Golem_Merlin");

	/* For.Prototype_Component_Weapon_Pensive */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Pensive"),
		CWeapon_Golem_Merlin::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_Component_Weapon_Pensive");

	m_isFinished = true;

	return S_OK;
}

CMain2_Loader* CMain2_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CMain2_Loader* pInstance = New CMain2_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
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
