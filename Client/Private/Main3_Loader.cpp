#include "..\Public\Main3_Loader.h"
#include "GameInstance.h"

/////////////////////////////////////////////
///////// Player / Player etc / NPC /////////
/////////////////////////////////////////////

#include "Player.h"
#include "Weapon_Player_Wand.h"

#include "Professor_FIg.h"

#pragma region Player State

#include "StateContext.h"
#include "IdleState.h"
#include "MoveTurnState.h"
#include "MoveStartState.h"
#include "MoveLoopState.h"
#include "RollState.h"
#include "JumpState.h"
#include "HardLandState.h"

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

_uint WINAPI Thread_BackGroundMain(void* pArg)
{
	CMain3_Loader* pLoader = reinterpret_cast<CMain3_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain3_Loader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_BackGroundMain, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain3_Loader::Loading()
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

HRESULT CMain3_Loader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_isFinished = true;

	return S_OK;
}

HRESULT CMain3_Loader::Loading_For_MainGame()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		

		/* For.Prototype_GameObject_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Player");

		_float4x4 PivotMatrix;
		PivotMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(180.0f));
		/* For.Prototype_Component_Model_CustomModel_Player */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
			CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, L"../../Resources/Models/Anims/Biped_Skeleton_jog_idle_face/Biped_Skeleton_jog_idle_face.gcm", PivotMatrix))))
			throw TEXT("Prototype_Component_Model_CustomModel_Player");

		/* For.Prototype_Component_Weapon_Player_Wand */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Player_Wand"),
			CWeapon_Player_Wand::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Player_Wand");
#pragma region Player Parts

		/* For.Prototype_Component_MeshPart_Player_Head */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Head"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Head/Player_Head.dat"), TEXT("Player_Head")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Head");

		/* For.Prototype_Component_MeshPart_Player_Arm */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Arm"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Arm/Player_Arm.dat"), TEXT("Player_Arm")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Arm");

		/* For.Prototype_Component_MeshPart_Robe01 */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Robe01"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Robe_Student/Robe_Student.dat"), TEXT("Robe01")))))
			throw TEXT("Prototype_Component_MeshPart_Robe01");

		/* For.Prototype_Component_MeshPart_Player_Top */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Top"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Top/Player_Top.dat"), TEXT("Player_Top")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Top");

		/* For.Prototype_Component_MeshPart_Player_Pants */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Pants"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Pants/Player_Pants.dat"), TEXT("Player_Pants")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Pants");

		/* For.Prototype_Component_MeshPart_Player_Socks */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Socks"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Socks/Player_Socks.dat"), TEXT("Player_Socks")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Socks");

		/* For.Prototype_Component_MeshPart_Player_Shoes */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshPart_Player_Shoes"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Player_Shoes/Player_Shoes.dat"), TEXT("Player_Shoes")))))
			throw TEXT("Prototype_Component_MeshPart_Player_Shoes");

#pragma endregion // Player Parts

#pragma region Player State
		/* For.Prototype_Component_StateContext */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_StateContext"),
			CStateContext::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_StateContext");

		/* For.Prototype_Component_State_Idle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Idle"),
			CIdleState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Idle");

		/* For.Prototype_Component_State_Move_Turn */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Move_Turn"),
			CMoveTurnState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Turn");

		/* For.Prototype_Component_State_Move_Start */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Move_Start"),
			CMoveStartState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Start");

		/* For.Prototype_Component_State_Move_Loop */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Move_Loop"),
			CMoveLoopState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Move_Loop");

		/* For.Prototype_Component_State_Roll */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Roll"),
			CRollState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Roll");

		/* For.Prototype_Component_State_Jump */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Jump"),
			CJumpState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Jump");

		/* For.Prototype_Component_State_Hard_Land */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_State_Hard_Land"),
			CHardLandState::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_State_Hard_Land");

#pragma endregion

		/* For.Prototype_Component_Model_Professor_Fig */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Professor_Fig"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Professor_Fig/Professor_Fig.dat")))))
			throw TEXT("Prototype_Component_Model_Professor_Fig");

		/* For.Prototype_GameObject_Professor_Fig */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Professor_Fig"),
			CProfessor_Fig::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Professor_Fig");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

	m_isFinished = true;

	return S_OK;
}

CMain3_Loader* CMain3_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CMain3_Loader* pInstance = New CMain3_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
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
