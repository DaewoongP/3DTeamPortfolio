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

#pragma endregion UI

#pragma region Effects
#include "ParticleSystem.h"
#include "ParticleSystemPool.h"
#include "Trail.h"
#include "MeshEffect.h"
#include "Wingardium_Effect.h"
#include "EnergyBall.h"
#include "Breath.h"
#include "Pulse.h"
#include "RadialBlur.h"
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
#pragma endregion Magic

#include "Trigger_Vault.h"
#include "Phase.h"
#include "Cylinder.h"
#include "Event_Vault_Spawn.h"

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
	std::lock_guard<std::mutex> lock(mtx);

	/* For.Prototype_GameObject_MeshEffect*/
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Cloister_MeshEffect"),
		CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Cloister/Cloister.ME")))))
		throw TEXT("Prototype_GameObject_Cloister_MeshEffect");

	/* For.Prototype_GameObject_Trigger_Vault */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Trigger_Vault"),
		CTrigger_Vault::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Trigger_Vault");

	/* For.Prototype_GameObject_Event_Spawn */
	if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Event_Spawn"),
		CEvent_Vault_Spawn::Create(m_pDevice, m_pContext))))
		throw TEXT("Prototype_GameObject_Event_Spawn");

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


	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Sky(LEVELID eLevelID)
{
	return S_OK;
}

HRESULT CMain0_Loader::Loading_For_Sanctum(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	try /* Failed Check Loading_For_Sanctum Add_Prototype*/
	{
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
				CUI_Font::Create(m_pDevice, m_pContext, TEXT("../../Resources/Fonts/NexonGothic.spritefont")))))
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
		}
#pragma endregion

#pragma region Load Texture
		/* --------------Texture-------------- */
		/* Prototype_Component_Texture_Default_Particle*/
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Default_Particle"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Particle.png")))))
			throw TEXT("Prototype_Component_Texture_Default_Particle");

		/* Prototype_Component_Texture_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Texture_Terrain"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/Tile%d.dds"), 2))))
			throw TEXT("Prototype_Component_Texture_Terrain");

#pragma endregion

		/* Prototype_Component_Texture_Default_Particle*/
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
			throw TEXT("Prototype_Component_Texture_SkyBox");

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

		/* For.Prototype_Component_Shader_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_Debug"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Debug.hlsl"), VTXPOSNORTEX_DECL::Elements, VTXPOSNORTEX_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_Debug");

		/* Prototype_Component_Shader_DefaultEffect */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Shader_DefaultEffect"),
			CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_DefaultEffect.hlsl"), VTXMESH_DECL::Elements, VTXMESH_DECL::iNumElements))))
			throw TEXT("Prototype_Component_Shader_DefaultEffect");
#pragma endregion

#pragma region Load ETC

		{
			std::lock_guard<std::mutex> lock(mtx);

			/* --------------ETC-------------- */
		/* For.Prototype_Component_Health*/
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_Health"),
				CHealth::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_Health");

			/* For.Prototype_Component_RigidBody */
			if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RigidBody"),
				CRigidBody::Create(m_pDevice, m_pContext))))
				throw TEXT("Prototype_Component_RigidBody");

			///* For.Prototype_Component_RadialBlur */
			//if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_RadialBlur"),
			//	CRadialBlur::Create(m_pDevice, m_pContext))))
			//	throw TEXT("Prototype_Component_RadialBlur");
		}

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
#pragma endregion

#pragma region Load Magic
		{
			std::lock_guard<std::mutex> lock(mtx);

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
		}

		/* For.Prototype_GameObject_Projectile_Black */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Projectile_Black"),
			CProjectile_Black::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Projectile_Black");

		/* For.Prototype_GameObject_Projectile_White */
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_GameObject_Projectile_White"),
			CProjectile_White::Create(m_pDevice, m_pContext, eLevelID))))
			throw TEXT("Prototype_GameObject_Projectile_White");

		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Monster_DarkFlare_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Monster_DarkFlare/"), eLevelID))))
			throw TEXT("Prototype_Monster_DarkFlare_Particle");
		

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
		if (FAILED(m_pGameInstance->Add_Prototype(eLevelID, TEXT("Prototype_Component_MagicSlot"),
			CMagicSlot::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_MagicSlot");
#pragma endregion

#pragma region Load Particle
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
