#include "..\Public\Main0_Loader.h"
#include "GameInstance.h"

////////////////////////////////////////////////////////////
// UI / Texture / Buffer / Debug / Shader / Effect / Magic//
////////////////////////////////////////////////////////////
#include "Logo_BackGround.h"

#pragma region UI
#include "UI_Group_Logo.h"
#include "UI_Group_Loading.h"
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
#include "Menu_Gear.h"
#include "Menu_Quest.h"
#include "Menu_Invectory.h"
#include "UI_Inventory.h"
#include "Inventory.h"
#pragma endregion UI

#pragma region Effects
#include "ParticleSystem.h"
#include "ParticleSystemPool.h"
#include "Trail.h"
#include "MeshEffect.h"
#include "Wingardium_Effect.h"
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
#pragma endregion Magic

#include "Trigger_Vault.h"

#ifdef _DEBUG
#include "Test_Player.h"
#include "Camera_Debug.h"
#include "PhysXRender.h"
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

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL_CLIFFSIDE:
		hr = Loading_For_Cliffside();
		break;
	case LEVEL_VAULT:
		hr = Loading_For_Vault();
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

HRESULT CMain0_Loader::Loading_For_Logo()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_HP"),
			CUI_Group_HP::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_HP");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Progress"),
			CUI_Progress::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Progress");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Potion"),
			CUI_Group_Potion::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Potion");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Finisher"),
			CUI_Group_Finisher::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Finisher");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Skill"),
			CUI_Group_Skill::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Skill");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Finisher_Icon"),
			CUI_Group_Finisher_Icon::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Finisher_Icon");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"),
			CUI_Group_Enemy_HP::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Enemy_HP");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Font"),
			CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")))))
			throw TEXT("Prototype_GameObject_UI_Font");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Cursor"),
			CUI_Group_Cursor::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Cursor");
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_MiniMap"),
			CUI_Group_MiniMap::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_MiniMap");*/

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Image"),
			CUI_Image::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Image");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_UI_Button"),
			CUI_Button::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_UI_Button");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Field_Guide"),
			CField_Guide::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Field_Guide");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Main_Menu"),
			CMain_Menu::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Main_Menu");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Menu_Gear"),
			CMenu_Gear::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Menu_Gear");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Menu_Inventory"),
			CMenu_Inventory::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Menu_Inventory");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Menu_Quest"),
			CMenu_Quest::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Menu_Quest");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Logo"),
			CUI_Group_Logo::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Logo");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Loading"),
			CUI_Group_Loading::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Group_Loading");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Inventory"),
			CUI_Inventory::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_UI_Inventory");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Inventory"),
			CInventory::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Inventory");
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

HRESULT CMain0_Loader::Loading_For_Cliffside()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	try /* Failed Check Add_Prototype*/
	{
		/* --------------Texture-------------- */
		/* Prototype_Component_Texture_Default_Particle*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Default_Particle"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Particle.png")))))
			throw TEXT("Prototype_Component_Texture_Default_Particle");

		/* Prototype_Component_Texture_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
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
		
		/* For.Prototype_Component_Shader_ShadowAnimMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowAnimMesh.hlsl"), VTXANIMMESH_DECL::Elements, VTXANIMMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_ShadowAnimMesh");

		/* For.Prototype_Component_Shader_ShadowMesh */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMesh"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowMesh.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_ShadowMesh");

		/* For.Prototype_Component_Shader_MeshInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMeshInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMeshInstance.hlsl"),
				VTXMESHINSTANCE_DECL::Elements, VTXMESHINSTANCE_DECL::iNumElements))))
			return E_FAIL;
		
		/* For.Prototype_Component_Shader_ShadowMeshInstance */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowMeshInstance"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ShadowMeshInstance.hlsl"),
				VTXMESHINSTANCE_DECL::Elements, VTXMESHINSTANCE_DECL::iNumElements))))
			return E_FAIL;

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

	
		/* --------------ETC-------------- */
		/* For.Prototype_Component_Health*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			CHealth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Health");

		/* For.Prototype_Component_RigidBody */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			CRigidBody::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RigidBody");

		/* --------------Magic-------------- */
		/* For. Prototype_Component_Magic*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Magic"),
			CMagic::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Magic");



#pragma region Magic
		/* For.Prototype_GameObject_BasicCast */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast"),
			CBasicCast::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_BasicCast");

		/* For.Prototype_GameObject_Protego */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Protego"),
			CProtego::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Protego");

		/* For.Prototype_GameObject_Revelio */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Revelio"),
			CRevelio::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Revelio");

		/* For.Prototype_GameObject_Wingardiumleviosa */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Wingardiumleviosa"),
			CWingardiumleviosa::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Wingardiumleviosa");

		/* For.Prototype_GameObject_Levioso */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Levioso"),
			CLevioso::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Levioso");

		/* For.Prototype_GameObject_Confringo */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Confringo"),
			CConfringo::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Confringo");

		/* For.Prototype_GameObject_Finisher */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Finisher"),
			CFinisher::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Finisher");

		/* For.Prototype_GameObject_Ncendio */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Ncendio"),
			CNcendio::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Ncendio");

		/* For.Prototype_GameObject_Lumos */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Lumos"),
			CLumos::Create(m_pDevice, m_pContext, LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Lumos");

#pragma endregion
		{
			std::lock_guard<std::mutex> lock(mtx);

			CMagicBallPool* pMagicBallPool = CMagicBallPool::GetInstance();
			Safe_AddRef(pMagicBallPool);
			if (FAILED(pMagicBallPool->Initialize()))
				throw TEXT("Magic Ball Pool");
			Safe_Release(pMagicBallPool);
		}

		/* For.Prototype_GameObject_MagicSlot */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			CMagicSlot::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_MagicSlot");

		{
			std::lock_guard<std::mutex> lock(mtx);

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust01"),
				TEXT("../../Resources/GameData/ParticleData/Misc/Dust01/"), 3)))
				throw TEXT("Reserve Particle : Particle_Dust01");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_Dust02"),
				TEXT("../../Resources/GameData/ParticleData/Misc/Dust02/"), 3)))
				throw TEXT("Reserve Particle : Particle_Dust02");

			if (FAILED(m_pGameInstance->Reserve_Particle(m_pDevice, m_pContext, TEXT("Particle_RockChunksRough"),
				TEXT("../../Resources/GameData/ParticleData/Misc/RockChunksRough/"), 3)))
				throw TEXT("Reserve Particle : Particle_RockChunksRough");
		}

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

#endif // _DEBUG
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

HRESULT CMain0_Loader::Loading_For_Vault()
{
	/* For.Prototype_GameObject_MeshEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VAULT, TEXT("Prototype_GameObject_Cloister_MeshEffect"),
		CMeshEffect::Create(m_pDevice, m_pContext,TEXT("../../Resources/GameData/MeshEffectData/Cloister/Cloister.ME")))))
		throw TEXT("Prototype_GameObject_Cloister_MeshEffect");

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_VAULT, TEXT("Prototype_GameObject_Trigger_Vault"),
		CTrigger_Vault::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Trigger_Vault");


	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_GreatHall()
{
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
