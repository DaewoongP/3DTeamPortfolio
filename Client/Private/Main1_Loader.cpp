#include "..\Public\Main1_Loader.h"
#include "GameInstance.h"
#include "Client_Includes.h"
#include "Behavior_Includes.h"

/////////////////////////////////////////////
// Behavior / Effect / Magic / MapObject ////
/////////////////////////////////////////////

#pragma region Effects
#include "ParticleSystem.h"
#include "Trail.h"
#include "MeshEffect.h"
#include "Default_Magic_Effect.h"
#include "Protego_Effect.h"
#include "Default_MagicTraill_Effect.h"
#include "Wingardium_Effect.h"
#pragma endregion Effects

#pragma region Magic
#include "Magic.h"
#include "MagicBallPool.h"

#include "BasicCast.h"
#include "Protego.h"
#include "Revelio.h"
#include "Levioso.h"
#include "Wingardiumleviosa.h"
#pragma endregion Magic

#include "Sky.h"
#include "Terrain.h"
#include "MapObject.h"

CMain1_Loader::CMain1_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint WINAPI Thread_Main1(void* pArg)
{
	CMain1_Loader* pLoader = reinterpret_cast<CMain1_Loader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return -1;

	return 0;
}

HRESULT CMain1_Loader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	// 쓰레드 시작 함수 호출
	// 3번째 인자로 시작할 함수포인터 대입.
	// 4번째 인자로 시작할 함수의 매개변수로 넣어줄 값 대입.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main1, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMain1_Loader::Loading()
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	// 크리티컬 섹션 시작해서 다른 쓰레드가 이 안에 있는 동안 값을 변경하지 못하게 처리.
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

HRESULT CMain1_Loader::Loading_For_Logo()
{
	m_isFinished = true;

	return S_OK;
}

HRESULT CMain1_Loader::Loading_For_MainGame()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Add_Prototype*/
	{
		/* For.Prototype_GameObject_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Sky");

		/* For.Prototype_GameObject_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Terrain");

		if (FAILED(Loading_Map_Object(TEXT("../../Resources/GameData/MapData/MapData6.ddd"))))
			throw TEXT("Map Object");

		/* For.Prototype_Component_CharacterController*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
			CCharacterController::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_CharacterController");

		/* For.Prototype_Component_RigidBody */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			CRigidBody::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RigidBody");

		/* For.Prototype_Component_RootBehavior */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			CRootBehavior::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RootBehavior");

		/* For.Prototype_Component_Selector */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Selector"),
			CSelector::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Selector");

		/* For.Prototype_Component_Sequence */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sequence"),
			CSequence::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence");

		/* For.Prototype_Component_Sequence_Groggy */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Groggy"),
			CSequence_Groggy::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_Groggy");

		/* For.Prototype_Component_Random_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Random_Attack"),
			CRandom_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Random_Attack");

		/* For.Prototype_Component_Action_Deflect */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Action_Deflect"),
			CAction_Deflect::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Action_Deflect");

		/* For.Prototype_Component_Action */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Action"),
			CAction::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Action");

		/* For.Prototype_Component_Wait */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Wait"),
			CWait::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Wait");

		/* For.Prototype_Component_Turn */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Turn"),
			CTurn::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Turn");

		/* For.Prototype_Component_Check_Degree */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree"),
			CCheck_Degree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Check_Degree");

		/* For.Prototype_Component_Check_Distance */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance"),
			CCheck_Distance::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Check_Distance");

		/* For.Prototype_Component_Selector_Degree */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree"),
			CSelector_Degree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Selector_Degree");
		
		/* For.Prototype_Component_Sequence_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack"),
			CSequence_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_Attack");

		/* For.Prototype_Component_Sequence_MoveTarget */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget"),
			CSequence_MoveTarget::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_MoveTarget");

		/* For. Prototype_Component_Magic*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
			CMagic::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Magic");

#pragma region Magic_Effect

		/* For.Prototype_GameObject_Default_Magic_Effect*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Default_Magic_Effect"),
			CDefault_Magic_Effect::Create(m_pDevice, m_pContext, LEVEL_MAINGAME))))
			throw TEXT("Prototype_GameObject_Default_Magic_Effect");

		/* For.Prototype_GameObject_Wingardium_Effect*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Wingardium_Effect"),
			CWingardium_Effect::Create(m_pDevice, m_pContext, LEVEL_MAINGAME))))
			throw TEXT("Prototype_GameObject_Wingardium_Effect");

		/* For.Prototype_GameObject_Wingardium_Effect*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Default_MagicTraill_Effect"),
			CDefault_MagicTraill_Effect::Create(m_pDevice, m_pContext, LEVEL_MAINGAME))))
			throw TEXT("Prototype_GameObject_Default_MagicTraill_Effect");
		
#pragma endregion

#pragma region Magic
		/* For.Prototype_GameObject_BasicCast */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_BasicCast"),
			CBasicCast::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_BasicCast");

		/* For.Prototype_GameObject_Protego */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Protego"),
			CProtego::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Protego");

		/* For.Prototype_GameObject_Revelio */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Revelio"),
			CRevelio::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Revelio");

		/* For.Prototype_GameObject_Revelio */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Wingardiumleviosa"),
			CWingardiumleviosa::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Wingardiumleviosa");

		/* For.Prototype_GameObject_Revelio */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Levioso"),
			CLevioso::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Levioso");
#pragma endregion

		/* For.Prototype_GameObject_Protego_Effect*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Protego_Effect"),
			CProtego_Effect::Create(m_pDevice, m_pContext, LEVEL_MAINGAME))))
			throw TEXT("Prototype_GameObject_Protego_Effect");
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

HRESULT CMain1_Loader::Loading_Map_Object(const _tchar* pMapObjectPath)
{
	/* For.Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_MapObject");

	HANDLE hFile = CreateFile(pMapObjectPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

	// 맵 오브젝트 번호
	_uint iObjectNum = 0;

	DWORD    dwByte = 0;

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
		BEGININSTANCE;

		// 프로토타입 생성 부분
		wstring ws(LoadDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);

		wstring modelPath(TEXT("../../Resources/Models/MapObject/NonAnims/"));
		modelPath += modelName;
		modelPath += TEXT("/");
		modelPath += modelName;
		modelPath += TEXT(".dat");

		// 프로토타입 생성
		_float4x4 PivotMatrix = XMMatrixIdentity();
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_MAINGAME, LoadDesc.wszTag,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, modelPath.c_str(), PivotMatrix), true)))
		{
			MSG_BOX("Failed to Create New Model Prototype");
		}
		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);
	return S_OK;
}

CMain1_Loader* CMain1_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CMain1_Loader* pInstance = New CMain1_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created CMain1_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain1_Loader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
