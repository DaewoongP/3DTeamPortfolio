#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "Client_Includes.h"
#include "Behavior_Includes.h"

#pragma region BackGrounds
#include "Sky.h"
#include "Terrain.h"
#include "Logo_BackGround.h"
#pragma endregion BackGrounds

#pragma region UI
#include "UI_Group_HP.h"
#include "UI_Group_Potion.h"
#include "UI_Group_Finisher.h"
#include "UI_Progress1.h"
#pragma endregion UI

#pragma region Monsters & NPC
#include "Armored_Troll.h"
#include "Forest_Troll.h"
#include "Golem_Combat.h"
#include "Golem_Merlin.h"
#include "Dugbog.h"
#pragma endregion Monsters & NPC

#pragma region Weapon
#include "Weapon_Armored_Troll.h"
#include "Weapon_Forest_Troll.h"
#include "Weapon_Golem_Combat.h"
#include "Weapon_Golem_Merlin.h"
#pragma endregion Weapon

#pragma region Effects
#include "ParticleSystem.h"
#include "MeshEffect.h"
#pragma endregion Effects

#pragma region Player
#include "Player.h"
#pragma endregion Player

#include "MapObject.h"

#ifdef _DEBUG
#include "Test_Player.h"
#include "Test_NPC.h"
#include "Test_Cloth.h"
#include "Camera_Debug.h"
#include "Dummy.h"
#include "Test_Stair.h"
#include "PhysXRender.h"
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
		return -1;

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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Default0.jpg")))))
	{
		MSG_BOX("Failed Add_Prototype : (Prototype_Component_Texture_Logo)");
		return E_FAIL;
	}

	lstrcpy(m_szLoading, TEXT("모델 로딩 중."));


	lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));


	lstrcpy(m_szLoading, TEXT("객체 로딩 중."));
	/* For.Prototype_GameObject_Logo_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_Logo_BackGround"),
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

	try /* Failed Check Add_Prototype*/
	{
		lstrcpy(m_szLoading, TEXT("텍스쳐 로딩 중."));
		/* For.Prototype_Component_Texture_SkyBox*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_SkyBox"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/SkyBox/Sky_%d.dds"), 4))))
			throw TEXT("Prototype_Component_Texture_SkyBox");

		/* Prototype_Component_Texture_Default_Particle*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Default_Particle"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Textures/Default_Particle.png")))))
			throw TEXT("Prototype_Component_Texture_Default_Particle");

		/* Prototype_Component_Texture_T_Default_Material_Grid_M*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_T_Default_Material_Grid_M"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/NonAnims/SM_SpherePrimitiveRegularNormals_01/T_Default_Material_Grid_M.png")))))
			throw TEXT("Prototype_Component_Texture_Default_Particle");

		/* Prototype_Component_Texture_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Texture_Terrain"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/Tile%d.dds"), 2))))
			throw TEXT("Prototype_Component_Texture_Terrain");

		lstrcpy(m_szLoading, TEXT("버퍼 로딩 중."));
		/* For.Prototype_Component_VIBuffer_Cube */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
			CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cube");

		/* Prototype_Component_VIBuffer_Terrain */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resources/Default/Textures/Terrain/Height.bmp")))))
		//	throw TEXT("Prototype_Component_VIBuffer_Terrain");
		
		/* For.Prototype_Component_VIBuffer_Cloth */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cloth"),
			CVIBuffer_Cloth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_VIBuffer_Cloth");

		lstrcpy(m_szLoading, TEXT("모델 로딩 중."));

		_float4x4 PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));

		/* For.Weapon Models */
		/* For.Prototype_Component_Model_Weopon_Armored_Troll */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Armored_Troll"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Armored_Club/Troll_Armored_Club.dat"), PivotMatrix))))
		//	throw TEXT("Prototype_Component_Model_Weopon_Armored_Troll");

		///* For.Prototype_Component_Model_Weopon_Forest_Troll */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Forest_Troll"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Troll_Forest_Club/Troll_Forest_Club.dat"), PivotMatrix))))
		//	throw TEXT("Prototype_Component_Model_Weopon_Forest_Troll");

		/* For.Prototype_Component_Model_Weopon_Golem_Combat */
		PivotMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f));
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Sword/Golem_Combat_Sword.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Weopon_Golem_Combat");

		///* For.Prototype_Component_Model_Weopon_Golem_Merlin */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Golem_Merlin"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Golem_Combat_Mace/Golem_Combat_Mace.dat"), PivotMatrix))))
		//	throw TEXT("Prototype_Component_Model_Weopon_Golem_Merlin");

		///* For.Prototype_Component_Model_Weopon_Pensive */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Weopon_Pensive"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, TEXT("../../Resources/Models/NonAnims/Pensive_Flail/Pensive_Flail.dat"), PivotMatrix))))
		//	throw TEXT("Prototype_Component_Model_Weopon_Pensive");

		/* For.Monster Models */
		/* For.Prototype_Component_Model_Armored_Troll */
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Armored_Troll"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Troll_Armored.gcm")))))
			throw TEXT("Prototype_Component_Model_Armored_Troll");*/

		/* For.Prototype_Component_Model_Forest_Troll */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Forest_Troll"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Troll_Forest/Troll_Forest.dat")))))
		//	throw TEXT("Prototype_Component_Model_Forest_Troll");

		///* For.Prototype_Component_Model_Dugbog */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Dugbog"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Dugbog/Dugbog.dat")))))
		//	throw TEXT("Prototype_Component_Model_Dugbog");

		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
		/* For.Prototype_Component_Model_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Combat"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_CombatGrunt/Golem_CombatGrunt.gcm"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Golem_Combat");

		///* For.Prototype_Component_Model_Golem_Merlin */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Merlin"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Golem_MerlinGrunt/Golem_MerlinGrunt.dat")))))
		//	throw TEXT("Prototype_Component_Model_Golem_Merlin");

		///* For.Prototype_Component_Model_Pensive */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Pensive"),
		//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/PensivePaladin/PensivePaladin.dat")))))
		//	throw TEXT("Prototype_Component_Model_Pensive");
		
		//-------------------------- LOD Models --------------------------//
		/*CModel_LOD::LODDESC LodDesc;
		LodDesc.iLevelIndex = LEVEL_MAINGAME;
		lstrcpy(LodDesc.szPrototypeName, TEXT("Prototype_Component_Model_Test_Robe_LOD"));
		LodDesc.eModelType = CModel::TYPE_NONANIM;
		LodDesc.PivotMatrix = XMMatrixIdentity();

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Test_Robe_LOD"),
			CModel_LOD::Create(m_pDevice, m_pContext, LodDesc, TEXT("../../Resources/Models/NonAnims/SM_DungeonEnterance_Cave_A/SM_DungeonEnterance_Cave_A_Lod%d.dat"), 3))))
			throw TEXT("Prototype_Component_Model_Test_Robe_LOD");*/

#ifdef _DEBUG
		PivotMatrix = XMMatrixScaling(100.f, 100.f, 100.f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		/* For.Prototype_Component_Model_Fiona*/
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Fiona"),
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, TEXT("../../Resources/Models/Anims/Fiona/Fiona.dat"), PivotMatrix))))
			throw TEXT("Prototype_Component_Model_Fiona");*/

		///* For.Prototype_Component_Model_CustomModel */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
		//	CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, L"../../Resources/Models/Anims/test/test.dat"))))
		//	throw TEXT("Prototype_Component_Model_CustomModel");

		/* For.Prototype_Component_Model_CustomModel_Player */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_CustomModel_Player"),
			CCustomModel::Create(m_pDevice, m_pContext, CCustomModel::TYPE_ANIM, L"../../Resources/Models/Anims/Biped_Skeleton/Biped_Skeleton.dat"))))
			throw TEXT("Prototype_Component_Model_CustomModel_Player");

		///* MeshParts (테스트용) */
		///* For.Prototype_Component_MeshParts_Head */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Head"),
		//	CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/head/head.dat"), TEXT("HEAD_Default")))))
		//	throw TEXT("Prototype_Component_MeshParts_Head");

		/* For.Prototype_Component_MeshParts_Arm */
		/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Arm"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Arm/Arm.dat"), TEXT("ARM_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Arm");*/

		/* For.Prototype_Component_MeshParts_Up */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Up"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Up/Up.dat"), TEXT("UPPERBODY_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Up");

		///* For.Prototype_Component_MeshParts_Low */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Low"),
		//	CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Low/Low.dat"), TEXT("UNDERBODY_Default")))))
		//	throw TEXT("Prototype_Component_MeshParts_Low");

		/* For.Prototype_Component_MeshParts_Robe */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Robe"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Robe_Student_Test/Robe_Student_Test.dat"), TEXT("ROBE_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Robe");
		
		/* For.Prototype_Component_MeshParts_Robe_Student */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Robe_Student"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Robe_Student/Robe_Student.dat"), TEXT("ROBE_Student")))))
			throw TEXT("Prototype_Component_MeshParts_Robe_Student");

		/* For.Prototype_Component_MeshParts_Socks */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Socks"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Socks/Socks.dat"), TEXT("SOCKS_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Socks");

		/* For.Prototype_Component_MeshParts_Socks1 */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Socks1"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Socks1/Socks1.dat"), TEXT("SOCKS_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Socks1");

		/* For.Prototype_Component_MeshParts_Shoes */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_MeshParts_Shoes"),
			CMeshParts::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Shoes/Shoes.dat"), TEXT("SHOES_Default")))))
			throw TEXT("Prototype_Component_MeshParts_Shoes");

		

#endif // _DEBUG

		lstrcpy(m_szLoading, TEXT("셰이더 로딩 중."));
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

		lstrcpy(m_szLoading, TEXT("피직스 로딩 중."));
		/* For.Prototype_Component_CharacterController*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_CharacterController"),
			CCharacterController::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_CharacterController");

		/* For.Prototype_Component_RigidBody */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			CRigidBody::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_RigidBody");

		/* For.Prototype_Component_Plane */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Plane"),
			CPlane::Create(m_pDevice, m_pContext, nullptr))))
			throw TEXT("Prototype_Component_Plane");

		lstrcpy(m_szLoading, TEXT("AI 로딩 중."));
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

		/* For.Prototype_Component_TargetDegree */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_TargetDegree"),
			CTargetDegree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_TargetDegree");

		/* For.Prototype_Component_Selector_Degree */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree"),
			CSelector_Degree::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Selector_Degree");

		lstrcpy(m_szLoading, TEXT("객체 로딩 중."));
		/* For.Prototype_GameObject_Sky */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Sky"),
			CSky::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Sky");

		/* For.Prototype_GameObject_Terrain */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Terrain");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ConvexMesh"),
			CConvexMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_ConvexMesh");

		/* For.Prototype_GameObject_ConvexMesh*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_TriangleMesh"),
			CTriangleMesh::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_TriangleMesh");

		/* For.Prototype_Component_Weapon_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Armored_Troll"),
			CWeapon_Armored_Troll::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Armored_Troll");

		/* For.Prototype_Component_Weapon_Forest_Troll */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Forest_Troll"),
		//	CWeapon_Forest_Troll::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_Weapon_Forest_Troll");

		/* For.Prototype_Component_Weapon_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			CWeapon_Golem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_Component_Weapon_Golem_Combat");

		///* For.Prototype_Component_Weapon_Golem_Merlin */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Merlin"),
		//	CWeapon_Golem_Merlin::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_Weapon_Golem_Merlin");

		///* For.Prototype_Component_Weapon_Pensive */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Pensive"),
		//	CWeapon_Golem_Merlin::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_Component_Weapon_Pensive");

		/* For.Prototype_GameObject_Armored_Troll */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Armored_Troll"),
			CArmored_Troll::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Armored_Troll");

		/* For.Prototype_GameObject_MapObject */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_MapObject"),
			CMapObject::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_MapObject");

		///* For.Prototype_GameObject_Forest_Troll */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Forest_Troll"),
		//	CForest_Troll::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_GameObject_Forest_Troll");

		///* For.Prototype_GameObject_Dugbog */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Dugbog"),
		//	CDugbog::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_GameObject_Dugbog");

		/* For.Prototype_GameObject_Golem_Combat */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Combat"),
			CGolem_Combat::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Golem_Combat");

		///* For.Prototype_GameObject_Golem_Merlin */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Golem_Merlin"),
		//	CGolem_Merlin::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_GameObject_Golem_Merlin");

		///* For.Prototype_GameObject_Pensive */
		//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Pensive"),
		//	CGolem_Merlin::Create(m_pDevice, m_pContext))))
		//	throw TEXT("Prototype_GameObject_Pensive");

		/* For.Prototype_GameObject_Dummy */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAINGAME, TEXT("Prototype_GameObject_Dummy"),
			CDummy::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Dummy");

#ifdef _DEBUG
		/* For.Prototype_GameObject_Camera_Debug*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Debug"),
			CCamera_Debug::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Camera_Debug");

		/* For.Prototype_GameObject_Test_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Player"),
			CTest_Player::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_Player");

		/* For.Prototype_GameObject_Test_NPC*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_NPC"),
			CTest_NPC::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_NPC");

		/* For.Prototype_GameObject_Test_Particle */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Particle"),
			CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Test/"), LEVEL_STATIC))))
			throw TEXT("Prototype_GameObject_Test_Particle");
			
		/* For.Prototype_GameObject_Test_Cloth*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Cloth"),
			CTest_Cloth::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_Cloth");
		
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Test_Stair"),
			CTest_Stair::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Test_Stair");

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_PhysxRenderer"),
			CPhysXRender::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_PhysxRenderer");
#endif // _DEBUG

		// For.UI
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


		/* For.Prototype_GameObject_Player*/
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Player"),
			CPlayer::Create(m_pDevice, m_pContext))))
			throw TEXT("Prototype_GameObject_Player");
		
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("Failed Add_Prototype : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		return E_FAIL;
	}

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
