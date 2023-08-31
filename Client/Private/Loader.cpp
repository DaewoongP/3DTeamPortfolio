#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "Client_Includes.h"

#include "Sky.h"
#include "Terrain.h"
#include "VIBuffer_Cube.h"
#include "Logo_BackGround.h"

#ifdef _DEBUG
#include "Test_Player.h"
#include "Camera_Debug.h"
#endif // _DEBUG


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint WINAPI Thread_Main(void* pArg)
{
	CLoader* pLoader = reinterpret_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 0;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	// 쓰레드 시작 함수 호출
	// 3번째 인자로 시작할 함수포인터 대입.
	// 4번째 인자로 시작할 함수의 매개변수로 넣어줄 값 대입.
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	if (0 == m_hThread)
	{
		MSG_BOX("Failed Begin Loading Thread");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLoader::Loading()
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

HRESULT CLoader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));
	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Default0.jpg")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Logo)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));
	

	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));
	/* For.Prototype_GameObject_Logo_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype_GameObject(TEXT("Prototype_GameObject_Logo_BackGround"),
		CLogo_BackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Logo_BackGround)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MainGame()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));
	/* For.Prototype_Component_Texture_SkyBox*/
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/SkyBox/Sky_%d.dds"), 4))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_SkyBox)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("버퍼 로딩 중."));
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Cube)");
		return E_FAIL;
	}

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 531, 531))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_VIBuffer_Terrain)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

#ifdef _DEBUG
	/* For.Prototype_Component_Model_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Fiona/Fiona.dat")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_Fiona)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Model_CustomModel */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel"),
		CCustomModel::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_CustomModel)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Model_TestModel */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_TestModel"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/test/test.dat")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Model_TestModel)");
		return E_FAIL;
	}

#endif // _DEBUG


	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));
	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXPOSCUBE_DECL::Elements, VTXPOSCUBE_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_SkyBox)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_VtxAnimMesh)");
		return E_FAIL;
	}

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Shader_Terrain)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("AI 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));
	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype_GameObject(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Sky)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype_GameObject(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Terrain)");
		return E_FAIL;
	}


#ifdef _DEBUG
	/* For.Prototype_GameObject_Camera_Debug*/
	if (FAILED(m_pGameInstance->Add_Prototype_GameObject(TEXT("Prototype_GameObject_Camera_Debug"),
		CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Camera_Debug)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Test_Player*/
	if (FAILED(m_pGameInstance->Add_Prototype_GameObject(TEXT("Prototype_GameObject_Test_Player"),
		CTest_Player::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Test_Player)");
		return E_FAIL;
	}
#endif // _DEBUG


	lstrcpy(m_szLoading, TEXT("로딩 완료."));

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	// 로딩이 끝날때까지 기다려야 하므로 infinite 옵션을 주어 로딩이 끝날때까지 쓰레드 대기.
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);
	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
