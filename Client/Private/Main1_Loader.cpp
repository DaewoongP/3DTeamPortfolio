#include "..\Public\Main1_Loader.h"
#include "GameInstance.h"
#include "Client_Includes.h"
#include "Behavior_Includes.h"

//////////////////////////////
// Behavior / MapObject //////
//////////////////////////////

#include "Sky.h"
#include "Terrain.h"
#include "MapObject.h"
#include "MapObject_Ins.h"

#include "Treasure_Chest.h"

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

HRESULT CMain1_Loader::Initialize(LEVELID eNextLevel, _bool isStaticLoaded)
{
	m_eNextLevelID = eNextLevel;
	m_isStaticLoaded = isStaticLoaded;

	InitializeCriticalSection(&m_Critical_Section);

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

HRESULT CMain1_Loader::Loading_For_Logo(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain1_Loader::Loading_For_Cliffside(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Add_Prototype*/
	{
		if (FAILED(Loading_Map_Object(TEXT("../../Resources/GameData/MapData/MapData0.ddd"), eLevelID)))
			throw TEXT("Map Object");

		if (FAILED(Loading_Map_Object_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins0.ddd"), eLevelID)))
			throw TEXT("Map Object_Ins");

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

HRESULT CMain1_Loader::Loading_For_Vault(LEVELID eLevelID)
{
	try
	{
		if (FAILED(Loading_Map_Object(TEXT("../../Resources/GameData/MapData/MapData1.ddd"), eLevelID)))
			throw TEXT("Map Object");

		if (FAILED(Loading_Map_Object_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins1.ddd"), eLevelID)))
			throw TEXT("Map Object_Ins");
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

HRESULT CMain1_Loader::Loading_For_GreatHall(LEVELID eLevelID)
{
	try
	{
		if (FAILED(Loading_Map_Object(TEXT("../../Resources/GameData/MapData/MapData2.ddd"), eLevelID)))
			throw TEXT("Map Object");

		if (FAILED(Loading_Map_Object_Ins(TEXT("../../Resources/GameData/MapData/MapData_Ins2.ddd"), eLevelID)))
			throw TEXT("Map Object_Ins");
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

HRESULT CMain1_Loader::Loading_For_Static(LEVELID eLevelID)
{
	try
	{
#pragma region Load BackGround
		/* For.Prototype_GameObject_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Sky");

		/* For.Prototype_Component_Model_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sky"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/SkySphere/SkySphere.dat")))))
			throw TEXT("Prototype_Component_Model_Sky");

		/* For.Prototype_GameObject_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Terrain");
#pragma endregion

#pragma region Load Main Behavior Com
		/* For.Prototype_Component_RootBehavior */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RootBehavior"),
			CRootBehavior::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RootBehavior");

		/* For.Prototype_Component_Action */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Action"),
			CAction::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Action");

		/* For.Prototype_Component_RandomChoose */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RandomChoose"),
			CRandomChoose::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RandomChoose");
#pragma endregion

#pragma region Behavior_Selectors
		/* For.Prototype_Component_Selector */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Selector"),
			CSelector::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Selector");

		/* For.Prototype_Component_Selector_Degree */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Selector_Degree"),
			CSelector_Degree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Selector_Degree");
#pragma endregion Behavior_Selectors

#pragma region Behavior_Sequences
		/* For.Prototype_Component_Sequence */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Sequence"),
			CSequence::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence");

		/* For.Prototype_Component_Sequence_Groggy */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Sequence_Groggy"),
			CSequence_Groggy::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_Groggy");

		/* For.Prototype_Component_Sequence_Levitate */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Sequence_Levitate"),
			CSequence_Levitate::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_Levitate");

		/* For.Prototype_Component_Sequence_Attack */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Sequence_Attack"),
			CSequence_Attack::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_Attack");

		/* For.Prototype_Component_Sequence_MoveTarget */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Sequence_MoveTarget"),
			CSequence_MoveTarget::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Sequence_MoveTarget");
#pragma endregion Behavior_Sequences

#pragma region Behavior_Movements
		/* For.Prototype_Component_RigidMove */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RigidMove"),
			CRigidMove::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RigidMove");

		/* For.Prototype_Component_Wait */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Wait"),
			CWait::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Wait");

		/* For.Prototype_Component_Turn */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Turn"),
			CTurn::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Turn");

		/* For.Prototype_Component_LookAt */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_LookAt"),
			CLookAt::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_LookAt");

		/* For.Prototype_Component_Check_Degree */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Check_Degree"),
			CCheck_Degree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Check_Degree");

		/* For.Prototype_Component_Check_Distance */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Check_Distance"),
			CCheck_Distance::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Check_Distance");

		/* For.Prototype_Component_Death */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Death"),
			CDeath::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Death");
#pragma endregion Behavior_Movements
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

HRESULT CMain1_Loader::Loading_Map_Object(const _tchar* pMapObjectPath, LEVELID eID)
{
	std::lock_guard<std::mutex> lock(mtx);

	/* For.Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype(eID, TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_MapObject");

	/* For.Prototype_GameObject_Treasure_Chest */
	if (FAILED(m_pGameInstance->Add_Prototype(eID, TEXT("Prototype_GameObject_Treasure_Chest"),
		CTreasure_Chest::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Treasure_Chest");

	HANDLE hFile = CreateFile(pMapObjectPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject File for Load MapObject");
		return E_FAIL;
	}

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
			return E_FAIL;
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

		wstring ws(LoadDesc.wszTag);
		size_t findIndex = ws.find(TEXT("Model_")) + 6;

		wstring modelName = ws.substr(findIndex);

		wstring modelPath(TEXT("../../Resources/Models/MapObject/NonAnims/"));
		modelPath += modelName;
		modelPath += TEXT("/");

		wstring szDirectoryPath = modelPath;
		modelPath += modelName;
		modelPath += TEXT(".dat");
		
		if (FAILED(pGameInstance->Add_Prototype(eID, LoadDesc.wszTag,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, modelPath.c_str()), true)))
		{
			MSG_BOX("Failed to Create New Model Prototype");
		}

		++iObjectNum; ENDINSTANCE;
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CMain1_Loader::Loading_Map_Object_Ins(const _tchar* pMapObjectInsPath, LEVELID eID)
{
	std::lock_guard<std::mutex> lock(mtx);

	/* For.Prototype_GameObject_MapObject_Ins */
	if (FAILED(m_pGameInstance->Add_Prototype(eID, TEXT("Prototype_GameObject_MapObject_Ins"),
		CMapObject_Ins::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_MapObject_Ins");

	HANDLE hFile = CreateFile(pMapObjectInsPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MSG_BOX("Failed to Create MapObject_Ins File for Load MapObject_Ins");
		return E_FAIL;
	}

	DWORD	dwByte = 0;
	_uint	iCount = 0; // �� �ν��Ͻ� �ѹ��� ����

	while (true)
	{
		LOADOJBECTINSDESC LoadDesc;
		ZEROMEM(&LoadDesc);

		if (!ReadFile(hFile, &LoadDesc.iInstanceCnt, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iInstanceCnt");
			return E_FAIL;
		}

		// ����Ǿ��ִ� �ν��Ͻ� ������ŭ �����Ҵ�
		if (0 != LoadDesc.iInstanceCnt)
		{
			LoadDesc.pMatTransform = New _float4x4[LoadDesc.iInstanceCnt];

			for (size_t i = 0; i < LoadDesc.iInstanceCnt; i++)
			{
				if (!ReadFile(hFile, &LoadDesc.pMatTransform[i], sizeof(_float4x4), &dwByte, nullptr))
				{
					MSG_BOX("Failed to Read MapObject_Ins pMatTransform");
					return E_FAIL;
				}
			}

			m_vecFreeMatrix.push_back(LoadDesc.pMatTransform);
		}

		if (!ReadFile(hFile, &LoadDesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins matTransform");
			return E_FAIL;
		}

		if (!ReadFile(hFile, &LoadDesc.iTagLen, sizeof(_uint), &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins iTagLen");
		}

		if (!ReadFile(hFile, &LoadDesc.wszTag, LoadDesc.iTagLen, &dwByte, nullptr))
		{
			MSG_BOX("Failed to Read MapObject_Ins wszTag");
			return E_FAIL;
		}

		if (dwByte == 0)
		{
			break;
		}

		// ���⼭ ������Ÿ�� �±� ���ڿ� �����������
		wstring ws = TEXT("Prototype_Component_Model_Instance_");
		wstring wsTag = TEXT("Prototype_Component_Model_");
		wstring wsSave(LoadDesc.wszTag);
		_uint iLength = wsTag.size();

		// �� �̸�
		wstring wsModelName = wsSave.substr(iLength);
		ws += wsModelName;

		ws += TEXT("_");

		_tchar wszNumber[MAX_PATH];
		_itow_s(iCount, wszNumber, 10);

		ws += wszNumber; // ������� ���� Prototype_Component_Model_Instance_�𵨸�_��ȣ �̷������� �̸��� ����	

		++iCount;

		wstring wsPath = TEXT("../../Resources/Models/MapObject/NonAnims/");
		wsPath += wsModelName;
		wsPath += TEXT("/");
		wsPath += wsModelName;
		wsPath += TEXT(".dat");

		// �ν��Ͻ� �� ������Ÿ�� ����
		_float4x4 PivotMatrix = XMMatrixIdentity();
		if (FAILED(m_pGameInstance->Add_Prototype(eID, ws.c_str(),
			CModel_Instance::Create(m_pDevice, m_pContext, CModel_Instance::TYPE_NONANIM, wsPath.c_str(),
				LoadDesc.pMatTransform, LoadDesc.iInstanceCnt, PivotMatrix), true)))
		{
			MSG_BOX("Failed to Create New CModel_Instance Prototype");
		}
	}

	CloseHandle(hFile);

	return S_OK;
}

CMain1_Loader* CMain1_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel, _bool isStaticLoaded)
{
	CMain1_Loader* pInstance = New CMain1_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, isStaticLoaded)))
	{
		MSG_BOX("Failed to Created CMain1_Loader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMain1_Loader::Free()
{
	// �ε��� ���������� ��ٷ��� �ϹǷ� infinite �ɼ��� �־� �ε��� ���������� ������ ���.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	// MapObject_Ins�� �ҷ����� ���� ���� �����Ҵ� ���� ó��
	for (auto& iter : m_vecFreeMatrix)
	{
		Safe_Delete_Array(iter);
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
