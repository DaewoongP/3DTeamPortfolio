#include "..\Public\Main0_Loader.h"
#include "GameInstance.h"

/////////////////////////////////////////////
// 로고화면 / UI / Texture / Buffer / Debug //
/////////////////////////////////////////////
#include "Logo_BackGround.h"

#pragma region UI
#include "UI_Group_HP.h"
#include "UI_Group_Potion.h"
#include "UI_Group_Finisher.h"
#include "UI_Group_Finisher_Icon.h"
#include "UI_Group_Skill.h"
#include "UI_Group_MiniMap.h"
#include "UI_Group_Enemy_HP.h"
#include "UI_Group_Cursor.h"
#include "UI_Progress1.h"
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Font.h"
#include "Field_Guide.h"
#include "Main_Menu.h"
#pragma endregion UI

#ifdef _DEBUG
#include "Test_Player.h"
#include "Camera_Debug.h"
#include "PhysXRender.h"
#include "LoadTrigger.h"
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

HRESULT CMain0_Loader::Initialize(LEVELID eNextLevel)
{
	m_eNextLevelID = eNextLevel;

	// 크리티컬 섹션 변수 초기화
	InitializeCriticalSection(&m_Critical_Section);

	// 쓰레드 시작 함수 호출
	// 3번째 인자로 시작할 함수포인터 대입.
	// 4번째 인자로 시작할 함수의 매개변수로 넣어줄 값 대입.
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

HRESULT CMain0_Loader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Default0.jpg")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Logo)");
		return E_FAIL;
	}

	/* For.Prototype_GameObject_Logo_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Logo_BackGround"),
		CLogo_BackGround::Create(m_pDevice, m_pContext))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_GameObject_Logo_BackGround)");
		return E_FAIL;
	}

	m_isFinished = true;

	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_MainGame()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		/* --------------Texture-------------- */
		/* Prototype_Component_Texture_Default_Particle*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Default_Particle"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Particle.png")))))
			throw TEXT("Prototype_Component_Texture_Default_Particle");

		/* Prototype_Component_Texture_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Terrain"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/Tile%d.dds"), 2))))
			throw TEXT("Prototype_Component_Texture_Terrain");

		/* --------------Buffer-------------- */
		/* For.Prototype_Component_VIBuffer_Cube */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
			CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cube");

		/* Prototype_Component_VIBuffer_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
			CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/DT.bmp")))))
			throw TEXT("Prototype_Component_VIBuffer_Terrain");

		/* For.Prototype_Component_VIBuffer_Cloth */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cloth"),
			CVIBuffer_Cloth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cloth");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ConvexMesh"),
			CConvexMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_ConvexMesh");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TriangleMesh"),
			CTriangleMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_TriangleMesh");

		/* --------------Shader-------------- */
		/* For.Prototype_Component_Shader_VtxCube */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXPOSCUBE_DECL::Elements, VTXPOSCUBE_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Texture_SkyBox");

		/* For.Prototype_Component_Shader_VtxAnimMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxAnimMesh");

		/* For.Prototype_Component_Shader_VtxMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_VtxMesh");

		/* For.Prototype_Component_Shader_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Terrain");

		/* For.Prototype_Component_Shader_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Debug"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Debug");

		/* Prototype_Component_Shader_DefaultEffect */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_DefaultEffect"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_DefaultEffect");

		/* --------------UI-------------- */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_HP"),
			CUI_Group_HP::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_HP");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_UI_Progress"),
			CUI_Progress::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Progress");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Potion"),
			CUI_Group_Potion::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Potion");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Finisher"),
			CUI_Group_Finisher::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Finisher");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Skill"),
			CUI_Group_Skill::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Skill");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
			CUI_Group_Finisher_Icon::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Finisher_Icon");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"),
			CUI_Group_Enemy_HP::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Enemy_HP");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Font"),
			CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")))))
			throw TEXT("Prototype_GameObject_UI_Font"); 
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_Cursor"),
			CUI_Group_Cursor::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Cursor");
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_UI_Group_MiniMap"),
			CUI_Group_MiniMap::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_MiniMap");*/

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_UI_Image"),
			CUI_Image::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Image");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_UI_Button"),
			CUI_Button::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Button");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Field_Guide"),
			CField_Guide::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Field_Guide");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Main_Menu"),
			CMain_Menu::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Main_Menu");

		/* --------------Debug-------------- */
#ifdef _DEBUG

		/* For.Prototype_GameObject_Camera_Debug*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Debug"),
			CCamera_Debug::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Camera_Debug");

		/* For.Prototype_GameObject_Test_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Player"),
			CTest_Player::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_Player");

		/* For.Prototype_GameObject_Test_Particle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Test/"), LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Test_Particle");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PhysxRenderer"),
			CPhysXRender::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_PhysxRenderer");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_LoadTrigger"),
			CLoadTrigger::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_LoadTrigger");

#endif // _DEBUG
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

CMain0_Loader* CMain0_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVELID eNextLevel)
{
	CMain0_Loader* pInstance = New CMain0_Loader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
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
