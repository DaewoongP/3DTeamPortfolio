#include "Armored_Troll.h"

#include "Client_GameInstance_Functions.h"
#include "Weapon_Armored_Troll.h"

#include "Camera_Shake.h"

#include "Turn.h"
#include "Wait.h"
#include "Death.h"
#include "LookAt.h"
#include "Action.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "RandomChoose.h"
#include "Check_Distance.h"
#include "Selector_Degree.h"
#include "Sequence_Groggy.h"
#include "Sequence_Attack.h"
#include "UI_Group_Enemy_HP.h"
#include "Sequence_Levitate.h"
#include "Sequence_MoveTarget.h"

#include "Quest_Manager.h"
#include "UI_Damage.h"

CArmored_Troll::CArmored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CArmored_Troll::CArmored_Troll(const CArmored_Troll& rhs)
	: CEnemy(rhs)
{
}

void CArmored_Troll::Set_Weapon_Render(_bool tf)
{
	m_pWeapon->Set_Render(tf); 
}

HRESULT CArmored_Troll::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmored_Troll::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Components_for_Shake()))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmored_Troll::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(Make_Notifies_for_Shake()))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	m_DarkAuraBoneMatrix[0] = m_pModelCom->Get_Bone_Index(32)->Get_CombinedTransformationMatrixPtr();
	m_DarkAuraBoneMatrix[1] = m_pModelCom->Get_Bone_Index(36)->Get_CombinedTransformationMatrixPtr();
	m_DarkAuraBoneMatrix[2] = m_pModelCom->Get_Bone_Index(96)->Get_CombinedTransformationMatrixPtr();
	m_DarkAuraBoneMatrix[3] = m_pModelCom->Get_Bone_Index(38)->Get_CombinedTransformationMatrixPtr();
	m_DarkAuraBoneMatrix[4] = m_pModelCom->Get_Bone_Index(97)->Get_CombinedTransformationMatrixPtr();

	m_pTarget = m_pPlayer;

	m_pModelCom->Change_Animation(TEXT("Idle_Combat"));

	return S_OK;
}

void CArmored_Troll::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[rand() % 3];

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);

	for (_uint i = 0; i < m_DarkAura.size(); i++)
	{
		m_DarkAura[i]->Get_Transform()->Set_Position(m_pTransform->Get_Position() + m_DarkAuraBoneMatrix[i]->Translation());
	}
}

void CArmored_Troll::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CArmored_Troll::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		Print_Damage_Font(iDamage);

		m_pHealth->Damaged(iDamage);

		_float3 vDir = CollisionEventDesc.pOtherTransform->Get_Position() - m_pTransform->Get_Position();
		vDir.Normalize();
		BEGININSTANCE;
		pGameInstance->Play_Particle(TEXT("Particle_Troll_Stone_Hit"), CollisionEventDesc.pOtherTransform->Get_Position(), vDir);
		pGameInstance->Play_Particle(TEXT("Particle_Troll_Dust_Hit"), CollisionEventDesc.pOtherTransform->Get_Position(), vDir);
		ENDINSTANCE;

		m_iCurrentSpell |= eBuff;
	}
}

HRESULT CArmored_Troll::Render()
{
	// 심씨 추가. 스폰되어야 랜더링 되게 넣어둠
	if (false == m_isSpawn)
		return S_OK;

	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
				throw TEXT("Bind_Material Normal");

			if (true == m_isDissolve)
			{
				if (FAILED(m_pShaderCom->Begin("AnimMesh_Dissolve")))
					return E_FAIL;
			}
			else if (1 == i)
			{
				if (FAILED(m_pEyeEmissiveTexture->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin("AnimMesh_Troll")))
					throw TEXT("Shader Begin AnimMesh_Troll");
			}
			else if (4 == i || 7 == i)
			{
				if (FAILED(m_pEmissiveTexture->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin("AnimMesh_Troll")))
					throw TEXT("Shader Begin AnimMesh_Troll");
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin("AnimMesh")))
					throw TEXT("Shader Begin AnimMesh");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CArmored_Troll::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	// 심씨 추가. 스폰되어야 랜더링 되게 넣어둠
	if (false == m_isSpawn)
		return S_OK;

	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShadowShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pShadowShaderCom->Begin("Shadow")))
				throw TEXT("Shader Begin Shadow");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CArmored_Troll::Add_Components_for_Shake()
{
	try
	{
		CCamera_Shake::CAMERA_SHAKE_DESC Camera_Shake_Desc = { CCamera_Shake::CAMERA_SHAKE_DESC() };

		_float fMaxDistance = { 30.0f };
		_float fMinDistance = { 2.0f };


		Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
		Camera_Shake_Desc.isDistanceOption = true;
		Camera_Shake_Desc.pTransform = m_pTransform;
		Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 0.2f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.05f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 10.0f;
		Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			TEXT("Com_Step_Shake"), reinterpret_cast<CComponent**>(&m_pStep_Shake), &Camera_Shake_Desc)))
			throw TEXT("Com_Step_Shake");

		m_pStep_Shake->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

		Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
		Camera_Shake_Desc.isDistanceOption = true;
		Camera_Shake_Desc.pTransform = m_pTransform;
		Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::OUT_SINE;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 1.5f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.1f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 10.0f;
		Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			TEXT("Com_Hit_Shake"), reinterpret_cast<CComponent**>(&m_pHit_Shake), &Camera_Shake_Desc)))
			throw TEXT("Com_Hit_Shake");

		m_pHit_Shake->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);

		Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
		Camera_Shake_Desc.isDistanceOption = false;
		Camera_Shake_Desc.pTransform = m_pTransform;
		Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::OUT_SINE;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 2.0f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.1f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 8.0f;
		Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			TEXT("Com_Hit_Shake"), reinterpret_cast<CComponent**>(&m_pDeath_Shake), &Camera_Shake_Desc)))
			throw TEXT("Com_Hit_Shake");

		m_pDeath_Shake->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmored_Troll::Make_Notifies_for_Shake()
{
	if (nullptr == m_pStep_Shake || nullptr == m_pHit_Shake || nullptr == m_pDeath_Shake)
	{
		MSG_BOX("Failed Make_Notifies_for_Shake"); 
		return E_FAIL;
	}

	function<void()> func = [&] { this->Play_FootStep_Sound(); };

	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Step_1"), func);
	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Step_2"), func);
	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Step_3"), func);
	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Step_4"), func);

	func = [&] { this->Play_Hit_Sound(); };
	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Hit"), func);

	func = [&] { this->Play_Dead_Sound(); };
	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Death_Land"), func);

	return S_OK;
}

HRESULT CArmored_Troll::Make_AI()
{
	BEGININSTANCE;

	try
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		if (FAILED(m_pRootBehavior->Add_Type("isOverheadAction", &m_isOverheadAction)))
			throw TEXT("Failed Add_Type isOverheadAction");
		if (FAILED(m_pRootBehavior->Add_Type("isPlaySound", &m_isPlaySound)))
			throw TEXT("Failed Add_Type isPlaySound");
		if (FAILED(m_pRootBehavior->Add_Type("isFirstFlip", _bool())))
			throw TEXT("Failed Add_Type isFirstFlip");

		/* Make Childs */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CSequence* pSequence_Death = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death)))
			throw TEXT("Failed Create_Behavior pSequence_Death");
		CSelector* pSelector_Alive = nullptr;
		if (FAILED(Create_Behavior(pSelector_Alive)))
			throw TEXT("Failed Create_Behavior pSelector_Alive");

		/* Set Decorator */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return *pIsSpawn;
			});

		/* Set Options */

		/* Bind Childs */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("RootBehavior Assemble pSelector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Selector Assemble Sequence_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Selector Assemble Selector_Alive");

		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Alive(pSelector_Alive)))
			throw TEXT("Failed Make_Alive");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_AI : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Notifies()
{
	function<void()> Func = [&] {(*this).Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Light_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Heavy_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Body_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Body_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Exit_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Exit_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).On_Overhead_Event(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Overhead_Event"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Off_Overhead_Event(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Overhead_Event"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmored_Troll::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 4000;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* For.EmissiveTexture */
		m_pEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Armored_Troll/T_Troll_ArmoredTroll_Metal_E.dds"));
		if (nullptr == m_pEmissiveTexture)
			throw TEXT("EmissiveTexture");

		/* For.EyeEmissiveTexture */
		m_pEyeEmissiveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/Armored_Troll/T_CorruptedSpider_Eye_D.dds"));
		if (nullptr == m_pEyeEmissiveTexture)
			throw TEXT("EyeEmissiveTexture");

		/* For.Com_AuraEffect*/
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Monster_DarkFlare_Particle")
			, TEXT("Com_DarkFlare_Particle01"), (CComponent**)&m_DarkAura[0])))
			throw TEXT("Com_DarkFlare_Particle01");
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Monster_DarkFlare_Particle")
			, TEXT("Com_DarkFlare_Particle02"), (CComponent**)&m_DarkAura[1])))
			throw TEXT("Com_DarkFlare_Particle02");
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Monster_DarkFlare_Particle")
			, TEXT("Com_DarkFlare_Particle03"), (CComponent**)&m_DarkAura[2])))
			throw TEXT("Com_DarkFlare_Particle03");
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Monster_DarkFlare_Particle")
			, TEXT("Com_DarkFlare_Particle04"), (CComponent**)&m_DarkAura[3])))
			throw TEXT("Com_DarkFlare_Particle04");
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Monster_DarkFlare_Particle")
			, TEXT("Com_DarkFlare_Particle05"), (CComponent**)&m_DarkAura[4])))
			throw TEXT("Com_DarkFlare_Particle05");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.7f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(1.2f, 1.5f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_MAGIC | COL_STATIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Body_Attack */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_SHIELD;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body_Attack");
		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::BOSS;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("10"));
		lstrcpy(Desc.wszObjectName, TEXT("무장한 트롤"));

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmored_Troll::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Armored_Troll"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Armored_Troll::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = _float4x4();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_Armored_Troll"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Add_Components_Level : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmored_Troll::Bind_HitMatrices()
{
	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Head"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[0] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("Hips"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[1] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("Spine2"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[2] = pBone->Get_CombinedTransformationMatrixPtr();

	return S_OK;
}

void CArmored_Troll::DeathBehavior(const _float& fTimeDelta)
{
	BEGININSTANCE;

	if (false == m_isEnterDeath)
	{
		pGameInstance->Play_Sound(TEXT("Troll_Dead.wav"), 0.8f);
		CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
		Safe_AddRef(pQuest_Manager);
		pQuest_Manager->Clear_Quest(TEXT("Quest_Town"));
		Safe_Release(pQuest_Manager);
		m_isEnterDeath = true;
	}

	m_fDeadTimeAcc += fTimeDelta;

	_float fSound = { 0.6f };

	if (2.f > m_fDeadTimeAcc)
	{
		// 보스 사운드 줄이고
		fSound -= m_fDeadTimeAcc * 0.4f;
		fSound = fSound < 0.f ? 0.f : fSound;
		pGameInstance->Set_ChannelVolume(m_iChennelNumber, fSound);
	}
	else
	{
		if (false == m_isEnter_Play_BGM)
		{
			// 마을 bgm으로 변경 후
			pGameInstance->Stop_Sound(m_iChennelNumber);
			//m_iChennelNumber = pGameInstance->Play_BGM(TEXT("HogSmead_Night_Bgm.wav"), 0.01f);
			m_isEnter_Play_BGM = true;
		}
		
		// bgm 소리 키움
		fSound = (m_fDeadTimeAcc - 2.f) * 0.5f;
		fSound = fSound > 0.6f ? 0.6f : fSound;
		pGameInstance->Set_ChannelVolume(m_iChennelNumber, fSound);
	}

	ENDINSTANCE;

	if (3.5f < m_fDeadTimeAcc)
	{
		m_isDissolve = true;
		m_fDissolveAmount += fTimeDelta / 1.5f; // 디졸브 값 증가
		m_pWeapon->On_Dissolve();
		m_isDead = true;
	}

	if (5.5f < m_fDeadTimeAcc && m_fDissolveAmount >= 1.f)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CArmored_Troll::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Ground)))
			throw TEXT("Failed Create_Behavior pAction_Death_Ground");

		CDeath* pTsk_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Ground)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Ground");

		// Set Decorators
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});

		// Set Options 
		pAction_Death_Ground->Set_Options(TEXT("Death"), m_pModelCom, true);
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) {this->DeathBehavior(fTimeDelta); };
		pTsk_Death_Ground->Set_DeathFunction(Func);

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Death_Ground"), pAction_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Ground");

		if (FAILED(pAction_Death_Ground->Assemble_Behavior(TEXT("Tsk_Death_Ground"), pTsk_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Ground");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Alive(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behavior */
		CSequence* pSequence_Intro = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Intro)))
			throw TEXT("Failed Create_Behavior pSequence_Intro");
		CSequence* pSequence_Flipendo = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Flipendo)))
			throw TEXT("Failed Create_Behavior pSequence_Flipendo");
		CSelector* pSelector_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Attacks)))
			throw TEXT("Failed Create_Behavior pSelector_Attacks");

		CSequence* pSequence_Attacks_Degree = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attacks_Degree)))
			throw TEXT("Failed Create_Behavior pSequence_Attacks_Degree");
		CSequence* pSequence_Attacks_Far = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attacks_Far)))
			throw TEXT("Failed Create_Behavior pSequence_Attacks_Far");
		CSequence* pSequence_Taunts = nullptr;
		if (FAILED(Create_Behavior(pSequence_Taunts)))
			throw TEXT("Failed Create_Behavior pSequence_Taunts");

		/* Set Decorator */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});
		pSelector_Attacks->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsOverheadAction = { nullptr };
				_uint* pCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isOverheadAction", pIsOverheadAction)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pCurrentSpell)))
					return false;

				if (true == *pIsOverheadAction && (BUFF_FLIPENDO & *pCurrentSpell))
					return false;

				if (BUFF_FLIPENDO & *pCurrentSpell)
					*pCurrentSpell ^= BUFF_FLIPENDO;

				return true;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Intro"), pSequence_Intro)))
			throw TEXT("Failed Assemble_Behavior Sequence_Intro");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Flipendo"), pSequence_Flipendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Flipendo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Attacks"), pSelector_Attacks)))
			throw TEXT("Failed Assemble_Behavior Selector_Attacks");

		if (FAILED(pSelector_Attacks->Assemble_Behavior(TEXT("Sequence_Attacks_Degree"), pSequence_Attacks_Degree)))
			throw TEXT("Failed Selector Assemble_Behavior Sequence_Attacks_Degree");
		if (FAILED(pSelector_Attacks->Assemble_Behavior(TEXT("Sequence_Attacks_Far"), pSequence_Attacks_Far)))
			throw TEXT("Failed Selector Assemble Sequence_Attacks_Far");
		if (FAILED(pSelector_Attacks->Assemble_Behavior(TEXT("Sequence_Taunts"), pSequence_Taunts)))
			throw TEXT("Failed Selector Assemble_Behavior Sequence_Taunts");

		if (FAILED(Make_Intro(pSequence_Intro)))
			throw TEXT("Failed Make_Intro");
		if (FAILED(Make_Flipendo(pSequence_Flipendo)))
			throw TEXT("Failed Make_Flipendo");
		if (FAILED(Make_Attack_Degree(pSequence_Attacks_Degree)))
			throw TEXT("Failed Make_Attack_Degree");
		if (FAILED(Make_Pattern_Attack_Far(pSequence_Attacks_Far)))
			throw TEXT("Failed Make_Pattern_Attack_Far");
		if (FAILED(Make_Taunt_Degree(pSequence_Taunts)))
			throw TEXT("Failed Make_Taunt_Degree");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Alive : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Intro(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Intro_1 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_1)))
			throw TEXT("Failed Create_Behavior pAction_Intro_1");
		CAction* pAction_Intro_2 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_2)))
			throw TEXT("Failed Create_Behavior pAction_Intro_2");
		CAction* pAction_Intro_3 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_3)))
			throw TEXT("Failed Create_Behavior pAction_Intro_3");
		CAction* pAction_Intro_4 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_4)))
			throw TEXT("Failed Create_Behavior pAction_Intro_4");
		CAction* pAction_Intro_5 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_5)))
			throw TEXT("Failed Create_Behavior pAction_Intro_5");

		CWait* pTsk_Wait = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorators */

		/* Set Options */
		pAction_Intro_1->Set_Options(TEXT("Intro_2"), m_pModelCom, false, 0.f, true);
		pAction_Intro_2->Set_Options(TEXT("Attack_Tantrum_1"), m_pModelCom, false, 0.f, true);
		pAction_Intro_3->Set_Options(TEXT("Attack_Tantrum_2"), m_pModelCom, false, 0.f, true);
		pAction_Intro_4->Set_Options(TEXT("Taunt_Front_4"), m_pModelCom, false, 0.f, true);
		pAction_Intro_5->Set_Options(TEXT("Idle_Combat"), m_pModelCom, true, 0.f, true);
		pTsk_Wait->Set_Timer(2.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Intro_1"), pAction_Intro_1)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Intro_2"), pAction_Intro_2)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_2");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Intro_3"), pAction_Intro_3)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_3");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Intro_4"), pAction_Intro_4)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_4");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Intro_5"), pAction_Intro_5)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_5");

		if (FAILED(pAction_Intro_5->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Intro : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Flipendo(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Club_Face_Enter = { nullptr };
		if (FAILED(Create_Behavior(pAction_Club_Face_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Club_Face_Enter");
		CAction* pAction_Stun_Loop = { nullptr };
		if (FAILED(Create_Behavior(pAction_Stun_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Stun_Loop");
		CAction* pAction_Club_Face_End = { nullptr };
		if (FAILED(Create_Behavior(pAction_Club_Face_End)))
			throw TEXT("Failed Create_Behavior pAction_Club_Face_End");

		CWait* pTsk_Wait = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsOverheadAction = { nullptr };
				_uint* pCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isOverheadAction", pIsOverheadAction)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pCurrentSpell)))
					return false;

				return true == *pIsOverheadAction && (BUFF_FLIPENDO & *pCurrentSpell);
			});
		pSequence->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pCurrentSpell)))
					return false;

				if (BUFF_FLIPENDO & *pCurrentSpell)
					*pCurrentSpell ^= BUFF_FLIPENDO;

				return true;
			});
		pAction_Club_Face_Enter->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool isFirstFlip = { false };
				if (FAILED(pBlackBoard->Get_Type("isFirstFlip", isFirstFlip)))
					return false;

				if (true == isFirstFlip)
				{
					CHealth* pHealth = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
						return false;

					Print_Damage_Font(200);

					pHealth->Damaged(200);

					if (FAILED(pBlackBoard->Set_Type("isFirstFlip", _bool(false))))
						return false;
				}

				return true;
			});
		pAction_Club_Face_Enter->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				if (FAILED(pBlackBoard->Set_Type("isFirstFlip", _bool(true))))
					return false;

				return true;
			});

		/* Set Options */
		pAction_Club_Face_Enter->Set_Options(TEXT("Club_Face_Enter"), m_pModelCom);
		pAction_Stun_Loop->Set_Options(TEXT("Stun_Knee_Loop"), m_pModelCom, true);
		pAction_Club_Face_End->Set_Options(TEXT("Club_Face_End"), m_pModelCom);
		pTsk_Wait->Set_Timer(3.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Club_Face_Enter"), pAction_Club_Face_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Club_Face_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Stun_Loop"), pAction_Stun_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Stun_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Club_Face_End"), pAction_Club_Face_End)))
			throw TEXT("Failed Assemble_Behavior Action_Club_Face_End");

		if (FAILED(pAction_Stun_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Flipendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Degree(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Degree */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pTsk_Check_Distance = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Distance)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Distance");
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = nullptr;
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CSequence* pSequence_Attack_Left_Front = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Left_Front)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Left_Front");
		CSequence* pSequence_Attack_Right_Front = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Right_Front)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Right_Front");
		CSequence* pSequence_Attack_Left_45 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Left_45)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Left_45");
		CSequence* pSequence_Attack_Left_90 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Left_90)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Left_90");
		CSequence* pSequence_Attack_Left_135 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Left_135)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Left_135");
		CSequence* pSequence_Attack_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Left_180)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Left_180");
		CSequence* pSequence_Attack_Right_45 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Right_45)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Right_45");
		CSequence* pSequence_Attack_Right_90 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Right_90)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Right_90");
		CSequence* pSequence_Attack_Right_135 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Right_135)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Right_135");
		CSequence* pSequence_Attack_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Right_180)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Right_180");

		/* Set Decorations */
		pSelector_Degree->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				if (7.f >= fTargetDistance)
				{
					_bool* pIsPlaySound = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("isPlaySound", pIsPlaySound)))
						return false;

					if (false == *pIsPlaySound)
					{
						Play_Random_Attack_Sound();
						*pIsPlaySound = true;
					}
					return true;
				}

				return false;
			});
		pSelector_Degree->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPlaySound = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPlaySound", pIsPlaySound)))
					return false;

				*pIsPlaySound = false;

				return true;
			});

		/* Set Options */
		pSelector_Degree->Set_Option(1.5f);
		pTsk_Check_Distance->Set_Option(m_pTransform);
		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Distance"), pTsk_Check_Distance)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_FRONT, pSequence_Attack_Left_Front)))
			throw TEXT("Failed Assemble_Behavior LEFT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_45, pSequence_Attack_Left_45)))
			throw TEXT("Failed Assemble_Behavior LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pSequence_Attack_Left_90)))
			throw TEXT("Failed Assemble_Behavior LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pSequence_Attack_Left_135)))
			throw TEXT("Failed Assemble_Behavior LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pSequence_Attack_Left_180)))
			throw TEXT("Failed Assemble_Behavior LEFT_BACK");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_FRONT, pSequence_Attack_Right_Front)))
			throw TEXT("Failed Assemble_Behavior RIGHT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_45, pSequence_Attack_Right_45)))
			throw TEXT("Failed Assemble_Behavior RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pSequence_Attack_Right_90)))
			throw TEXT("Failed Assemble_Behavior RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pSequence_Attack_Right_135)))
			throw TEXT("Failed Assemble_Behavior RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pSequence_Attack_Right_180)))
			throw TEXT("Failed Assemble_Behavior RIGHT_BACK");

		if (FAILED(Make_Attack_Left_Front(pSequence_Attack_Left_Front)))
			throw TEXT("Failed Make_Attack_Left_Front");
		if (FAILED(Make_Attack_Left_45(pSequence_Attack_Left_45)))
			throw TEXT("Failed Make_Attack_Left_45");
		if (FAILED(Make_Attack_Left_90(pSequence_Attack_Left_90)))
			throw TEXT("Failed Make_Attack_Left_90");
		if (FAILED(Make_Attack_Left_90(pSequence_Attack_Left_135)))
			throw TEXT("Failed Make_Attack_Left_90");
		if (FAILED(Make_Attack_Left_180(pSequence_Attack_Left_180)))
			throw TEXT("Failed Make_Attack_Left_180");

		if (FAILED(Make_Attack_Right_Front(pSequence_Attack_Right_Front)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Attack_Right_45(pSequence_Attack_Right_45)))
			throw TEXT("Failed Make_Attack_Right_45");
		if (FAILED(Make_Attack_Right_90(pSequence_Attack_Right_90)))
			throw TEXT("Failed Make_Attack_Right_90");
		if (FAILED(Make_Attack_Right_90(pSequence_Attack_Right_135)))
			throw TEXT("Failed Make_Attack_Right_90");
		if (FAILED(Make_Attack_Right_180(pSequence_Attack_Right_180)))
			throw TEXT("Failed Make_Attack_Right_180");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Degree : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Pattern_Attack_Far(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_Far */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pCheck_Distance = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CRandomChoose* pRandom_Attack = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attack)))
			throw TEXT("Failed Create_Behavior pRandom_Attack");

		CSelector* pSelector_Attack_Charge = nullptr;
		if (FAILED(Create_Behavior(pSelector_Attack_Charge)))
			throw TEXT("Failed Create_Behavior pSelector_Attack_Charge");
		CSequence* pSequence_Attack_Run = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Run)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Run");

		/* Set Decorations */
		pRandom_Attack->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attack"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior Random_Attack");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSelector_Attack_Charge, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Sequence_Attack_Run"), pSequence_Attack_Run, 0.7f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Run");

		if (FAILED(Make_Pattern_Attack_Run(pSequence_Attack_Run)))
			throw TEXT("Failed Make_Pattern_Attack_Run");
		if (FAILED(Make_Pattern_Attack_Charge(pSelector_Attack_Charge)))
			throw TEXT("Failed Make_Charge");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Pattern_Attack_Far : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Taunt_Degree(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turn_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = nullptr;
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");
		CRandomChoose* pRandom_Taunt_Left_Front = nullptr;
		if (FAILED(Create_Behavior(pRandom_Taunt_Left_Front)))
			throw TEXT("Failed Create_Behavior pRandom_Taunt_Left_Front");
		CRandomChoose* pRandom_Taunt_Right_Front = nullptr;
		if (FAILED(Create_Behavior(pRandom_Taunt_Right_Front)))
			throw TEXT("Failed Create_Behavior pRandom_Taunt_Right_Front");
		CAction* pAction_Idle = nullptr;
		if (FAILED(Create_Behavior(pAction_Idle)))
			throw TEXT("Failed Create_Behavior pAction_Idle");

		CAction* pAction_Left_Front_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front_1)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front_1");
		CAction* pAction_Left_Front_2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front_2)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front_2");
		CAction* pAction_Left_Front_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front_3)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front_3");
		CAction* pAction_Left_Front_4 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front_4)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front_4");
		CAction* pAction_Left_Front_5 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front_5)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front_5");

		CAction* pAction_Right_Front_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front_1)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front_1");
		CAction* pAction_Right_Front_2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front_2)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front_2");
		CAction* pAction_Right_Front_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front_3)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front_3");
		CAction* pAction_Right_Front_4 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front_4)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front_4");
		CAction* pAction_Right_Front_5 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front_5)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front_5");

		CAction* pAction_Left_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_45)))
			throw TEXT("Failed Create_Behavior pAction_Left_45");
		CAction* pAction_Right_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_45)))
			throw TEXT("Failed Create_Behavior pAction_Right_45");
		CAction* pAction_Left_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_90)))
			throw TEXT("Failed Create_Behavior pAction_Left_90");
		CAction* pAction_Right_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_90)))
			throw TEXT("Failed Create_Behavior pAction_Right_90");
		CAction* pAction_Left_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_135)))
			throw TEXT("Failed Create_Behavior pAction_Left_135");
		CAction* pAction_Right_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_135)))
			throw TEXT("Failed Create_Behavior pAction_Right_135");
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");

		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorations */
		pRandom_Taunt_Left_Front->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});
		pRandom_Taunt_Right_Front->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Idle->Set_Options(TEXT("Idle_Combat"), m_pModelCom, true);
		pTsk_Wait->Set_Timer(0.5f);

		pAction_Left_Front_1->Set_Options(TEXT("Taunt_Front_1"), m_pModelCom);
		pAction_Left_Front_2->Set_Options(TEXT("Taunt_Front_2"), m_pModelCom);
		pAction_Left_Front_3->Set_Options(TEXT("Taunt_Front_3"), m_pModelCom);
		pAction_Left_Front_4->Set_Options(TEXT("Taunt_Front_4"), m_pModelCom);
		pAction_Left_Front_5->Set_Options(TEXT("Taunt_Front_5"), m_pModelCom);
		pAction_Right_Front_1->Set_Options(TEXT("Taunt_Front_1"), m_pModelCom);
		pAction_Right_Front_2->Set_Options(TEXT("Taunt_Front_2"), m_pModelCom);
		pAction_Right_Front_3->Set_Options(TEXT("Taunt_Front_3"), m_pModelCom);
		pAction_Right_Front_4->Set_Options(TEXT("Taunt_Front_4"), m_pModelCom);
		pAction_Right_Front_5->Set_Options(TEXT("Taunt_Front_5"), m_pModelCom);

		pAction_Left_45->Set_Options(TEXT("Taunt_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Taunt_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Taunt_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Taunt_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Taunt_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Taunt_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Taunt_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Taunt_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Idle"), pAction_Idle)))
			throw TEXT("Failed Assemble_Behavior Action_Idle");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_FRONT, pRandom_Taunt_Left_Front)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_FRONT, pRandom_Taunt_Right_Front)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");

		if (FAILED(pRandom_Taunt_Left_Front->Assemble_Behavior(TEXT("Action_Left_Front_1"), pAction_Left_Front_1, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Left_Front_1");
		if (FAILED(pRandom_Taunt_Left_Front->Assemble_Behavior(TEXT("Action_Left_Front_2"), pAction_Left_Front_2, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Left_Front_2");
		if (FAILED(pRandom_Taunt_Left_Front->Assemble_Behavior(TEXT("Action_Left_Front_3"), pAction_Left_Front_3, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Left_Front_3");
		if (FAILED(pRandom_Taunt_Left_Front->Assemble_Behavior(TEXT("Action_Left_Front_4"), pAction_Left_Front_4, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Left_Front_4");
		if (FAILED(pRandom_Taunt_Left_Front->Assemble_Behavior(TEXT("Action_Left_Front_5"), pAction_Left_Front_5, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Left_Front_5");

		if (FAILED(pRandom_Taunt_Right_Front->Assemble_Behavior(TEXT("Action_Right_Front_1"), pAction_Right_Front_1, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Right_Front_1");
		if (FAILED(pRandom_Taunt_Right_Front->Assemble_Behavior(TEXT("Action_Right_Front_2"), pAction_Right_Front_2, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Right_Front_2");
		if (FAILED(pRandom_Taunt_Right_Front->Assemble_Behavior(TEXT("Action_Right_Front_3"), pAction_Right_Front_3, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Right_Front_3");
		if (FAILED(pRandom_Taunt_Right_Front->Assemble_Behavior(TEXT("Action_Right_Front_4"), pAction_Right_Front_4, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Right_Front_4");
		if (FAILED(pRandom_Taunt_Right_Front->Assemble_Behavior(TEXT("Action_Right_Front_5"), pAction_Right_Front_5, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Right_Front_5");

		if (FAILED(pAction_Idle->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Turn_Run : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Left_Front(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behaviors */
		CSequence* pSequence_Attack_ForHnd_1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd_1");
		CSequence* pSequence_Attack_BackHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd");
		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* Set Decorators */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_RightHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd_1"), pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pSequence_Attack_BackHnd"), pSequence_Attack_BackHnd)))
			throw TEXT("Failed Assemble_Behavior pSequence_Attack_BackHnd");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_Overhead"), pAction_Attack_Overhead)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead");

		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_Front : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Left_45(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Left_45 */
	{
		if (nullptr == pSequence)
			throw TEXT("pSequence pSequence is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Attack_ForHnd1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd1");
		CSequence* pSequence_Attack_BackHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd");
		CSequence* pSequence_Attack_ForHnd2 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd2");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd1"), pSequence_Attack_ForHnd1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd"), pSequence_Attack_BackHnd)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd2"), pSequence_Attack_ForHnd2)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd2");

		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd1)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd2)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_45 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Left_90(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Left_90 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CRandomChoose* pRandom_Select = nullptr;
		if (FAILED(Create_Behavior(pRandom_Select)))
			throw TEXT("Failed Create_Behavior pRandom_Select");

		CSequence* pSequence_BackHnd_Combo = nullptr;
		if (FAILED(Create_Behavior(pSequence_BackHnd_Combo)))
			throw TEXT("Failed Create_Behavior pSequence_BackHnd_Combo");
		CSequence* pSequence_ForHnd_Combo = nullptr;
		if (FAILED(Create_Behavior(pSequence_ForHnd_Combo)))
			throw TEXT("Failed Create_Behavior pSequence_ForHnd_Combo");
		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* BackHnd Combo */
		CSequence_Attack* pSequence_Attack_BackHnd_1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd_1");
		CSequence* pSequence_Attack_ForHnd_1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd_1");
		CAction* pAction_Attack_Overhead_Right = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead_Right)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead_Right");

		/* ForHnd Combo */
		CSequence_Attack* pSequence_Attack_ForHnd_2 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd_2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd_2");
		CSequence* pSequence_Attack_BackHnd_2 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd_2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd_2");
		CAction* pAction_Attack_Overhead_Left = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead_Left)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead_Left");

		/* Set Decorations */
		pRandom_Select->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pSequence_Attack_BackHnd_1->Set_Attack_Action_Options(TEXT("Attack_Step_Swing_Left_90_BackHnd"), m_pModelCom);
		pSequence_Attack_BackHnd_1->Set_Attack_Option(7.f);
		pSequence_Attack_ForHnd_2->Set_Attack_Action_Options(TEXT("Attack_Swing_Left_90_ForHnd"), m_pModelCom);
		pSequence_Attack_ForHnd_2->Set_Attack_Option(7.f);
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Left_90"), m_pModelCom);
		pAction_Attack_Overhead_Right->Set_Options(TEXT("Attack_Overhead_Slam_RightHnd"), m_pModelCom);
		pAction_Attack_Overhead_Left->Set_Options(TEXT("Attack_Overhead_Slam_LeftHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_BackHnd_Combo"), pSequence_BackHnd_Combo, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_BackHnd_Combo");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_ForHnd_Combo"), pSequence_ForHnd_Combo, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_ForHnd_Combo");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Action_Attack_Overhead"), pAction_Attack_Overhead, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead");

		/* Assemble BackHnd Combo */
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd_1"), pSequence_Attack_BackHnd_1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd_1");
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd_1"), pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd_1");
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Action_Attack_Overhead_Right"), pAction_Attack_Overhead_Right)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead_Right");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd_1)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");

		/* Assemble ForHnd Combo */
		if (FAILED(pSequence_ForHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd_2"), pSequence_Attack_ForHnd_2)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd_2");
		if (FAILED(pSequence_ForHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd_2"), pSequence_Attack_BackHnd_2)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd_2");
		if (FAILED(pSequence_ForHnd_Combo->Assemble_Behavior(TEXT("Action_Attack_Overhead_Left"), pAction_Attack_Overhead_Left)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead_Left");
		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd_2)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_90 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Left_180(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Left_180 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* Set Decorations */

		/* Set Options */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Left_180"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_Left_180"), pAction_Attack_Overhead)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Overhead");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_180 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Right_Front(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behaviors */
		CSequence* pSequence_Attack_BackHnd_1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd_1");
		CSequence* pSequence_Attack_ForHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd");

		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* Set Decorators */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_LeftHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd_1"), pSequence_Attack_BackHnd_1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd"), pSequence_Attack_ForHnd)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_Overhead"), pAction_Attack_Overhead)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead");

		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd_1)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_Front : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Right_45(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Right_45 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Attack_BackHnd1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd1");
		CSequence* pSequence_Attack_ForHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd");
		CSequence* pSequence_Attack_BackHnd2 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd2");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd1"), pSequence_Attack_BackHnd1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd"), pSequence_Attack_ForHnd)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd2"), pSequence_Attack_BackHnd2)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd2");

		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd1)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd2)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Right_45 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Right_90(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Right_90 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CRandomChoose* pRandom_Select = nullptr;
		if (FAILED(Create_Behavior(pRandom_Select)))
			throw TEXT("Failed Create_Behavior pRandom_Select");

		CSequence* pSequence_BackHnd_Combo = nullptr;
		if (FAILED(Create_Behavior(pSequence_BackHnd_Combo)))
			throw TEXT("Failed Create_Behavior pSequence_BackHnd_Combo");
		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* BackHnd Combo */
		CSequence_Attack* pSequence_Attack_BackHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_BackHnd");
		CSequence* pSequence_Attack_ForHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_ForHnd");
		CAction* pAction_Attack_Overhead_Right = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead_Right)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead_Right");

		/* Set Decorations */
		pRandom_Select->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pSequence_Attack_BackHnd->Set_Attack_Action_Options(TEXT("Attack_Swing_Right_90_BackHnd"), m_pModelCom);
		pSequence_Attack_BackHnd->Set_Attack_Option(7.f);
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Right_90"), m_pModelCom);
		pAction_Attack_Overhead_Right->Set_Options(TEXT("Attack_Overhead_Slam_RightHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_BackHnd_Combo"), pSequence_BackHnd_Combo, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Sequence_BackHnd_Combo");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Action_Attack_Overhead"), pAction_Attack_Overhead, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead");

		/* Assemble BackHnd Combo */
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_BackHnd"), pSequence_Attack_BackHnd)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd");
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd"), pSequence_Attack_ForHnd)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");
		if (FAILED(pSequence_BackHnd_Combo->Assemble_Behavior(TEXT("Action_Attack_Overhead_Right"), pAction_Attack_Overhead_Right)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead_Right");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Right_90 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Attack_Right_180(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Right_180 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Attack_Overhead = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Overhead)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Overhead");

		/* Set Decorations */

		/* Set Options */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Right_180"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_Right_180"), pAction_Attack_Overhead)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Overhead");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Right_180 : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Pattern_Attack_BackHnd(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_BackHnd */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pCheck_Distance = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CRandomChoose* pRandom_Attack = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attack)))
			throw TEXT("Failed Create_Behavior pRandom_Attack");

		CAction* pAttack_Swing = nullptr;
		if (FAILED(Create_Behavior(pAttack_Swing)))
			throw TEXT("Failed Create_Behavior pAttack_Swing");
		CAction* pAttack_1Step_Swing = nullptr;
		if (FAILED(Create_Behavior(pAttack_1Step_Swing)))
			throw TEXT("Failed Create_Behavior pAttack_1Step_Swing");
		CAction* pAttack_2Step_Swing = nullptr;
		if (FAILED(Create_Behavior(pAttack_2Step_Swing)))
			throw TEXT("Failed Create_Behavior pAttack_2Step_Swing");

		CLookAt* pTsk_LookAt_1 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_1)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_1");
		CLookAt* pTsk_LookAt_2 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_2)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_2");
		CLookAt* pTsk_LookAt_3 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_3)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_3");

		/* Set Decorations */
		pRandom_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return 7.f >= fTargetDistance;
			});
		pRandom_Attack->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);
		pAttack_Swing->Set_Options(TEXT("Attack_Swing_Front_BackHnd"), m_pModelCom);
		pAttack_1Step_Swing->Set_Options(TEXT("Attack_1Step_Swing_Front_BackHnd"), m_pModelCom);
		pAttack_2Step_Swing->Set_Options(TEXT("Attack_2Step_Swing_Front_BackHnd"), m_pModelCom);

		pTsk_LookAt_1->Set_Option(m_pTransform);
		pTsk_LookAt_2->Set_Option(m_pTransform);
		pTsk_LookAt_3->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attack"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior Random_Attack");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Attack_Swing"), pAttack_Swing, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Attack_Swing");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Attack_1Step_Swing"), pAttack_1Step_Swing, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Attack_1Step_Swing");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Attack_2Step_Swing"), pAttack_2Step_Swing, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Attack_2Step_Swing");

		if (FAILED(pAttack_Swing->Assemble_Behavior(TEXT("Tsk_LookAt_1"), pTsk_LookAt_1)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_1");
		if (FAILED(pAttack_1Step_Swing->Assemble_Behavior(TEXT("Tsk_LookAt_2"), pTsk_LookAt_2)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_2");
		if (FAILED(pAttack_2Step_Swing->Assemble_Behavior(TEXT("Tsk_LookAt_3"), pTsk_LookAt_3)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_3");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Pattern_Attack_BackHnd : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Pattern_Attack_ForHnd(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_ForHnd */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pCheck_Distance = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CRandomChoose* pRandom_Attack = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attack)))
			throw TEXT("Failed Create_Behavior pRandom_Attack");

		CAction* pAttack_Swing = nullptr;
		if (FAILED(Create_Behavior(pAttack_Swing)))
			throw TEXT("Failed Create_Behavior pAttack_Swing");
		CAction* pAttack_Step_Swing = nullptr;
		if (FAILED(Create_Behavior(pAttack_Step_Swing)))
			throw TEXT("Failed Create_Behavior pAttack_Step_Swing");

		CLookAt* pTsk_LookAt_1 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_1)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_1");
		CLookAt* pTsk_LookAt_2 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_2)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_2");

		/* Set Decorations */
		pRandom_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return 7.f >= fTargetDistance;
			});
		pRandom_Attack->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);
		pAttack_Swing->Set_Options(TEXT("Attack_Swing_Front_ForHnd"), m_pModelCom);
		pAttack_Step_Swing->Set_Options(TEXT("Attack_Step_Swing_Front_ForHnd"), m_pModelCom);

		pTsk_LookAt_1->Set_Option(m_pTransform);
		pTsk_LookAt_2->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior Random_Select");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Attack_Swing"), pAttack_Swing, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Attack_Swing");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Attack_Step_Swing"), pAttack_Step_Swing, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Attack_Step_Swing");

		if (FAILED(pAttack_Swing->Assemble_Behavior(TEXT("Tsk_LookAt_1"), pTsk_LookAt_1)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_1");
		if (FAILED(pAttack_Step_Swing->Assemble_Behavior(TEXT("Tsk_LookAt_2"), pTsk_LookAt_2)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_2");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Pattern_Attack_ForHnd : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Pattern_Attack_Run(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Turn_Run = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turn_Run)))
			throw TEXT("Failed Create_Behavior pSequence_Turn_Run");
		CAction* pAction_Run_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Run_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Run_Loop");
		CRandomChoose* pRandom_Attacks = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");

		CSequence_Attack* pSequence_Attack_Swing_BackHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Swing_BackHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Swing_BackHnd");
		CSequence_Attack* pSequence_Attack_Swing_ForHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Swing_ForHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Swing_ForHnd");
		CSequence_Attack* pSequence_Attack_Hop_ForHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Hop_ForHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Hop_ForHnd");
		CSequence_Attack* pSequence_Attack_Hop_BackHnd = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Hop_BackHnd)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Hop_BackHnd");

		CCheck_Distance* pTsk_Check_Distance = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Distance)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Distance");
		CLookAt* pTsk_LookAt = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt");

		/* Set Decorations */
		pSequence_Turn_Run->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fAttackRange = { 0.f };
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fAttackRange", fAttackRange)))
				{
					MSG_BOX("Failed Get_Type fAttackRange");
					return false;
				}
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
				{
					MSG_BOX("Failed Get_Type fTargetDistance");
					return false;
				}

				return 7.f < fTargetDistance;
			});
		pRandom_Attacks->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});
		pRandom_Attacks->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPlaySound = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPlaySound", pIsPlaySound)))
					return false;

				if (false == *pIsPlaySound)
				{
					Play_Random_Attack_Sound();
					*pIsPlaySound = true;
				}
				return true;
			});
		pRandom_Attacks->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPlaySound = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPlaySound", pIsPlaySound)))
					return false;

				*pIsPlaySound = false;

				return true;
			});

		/* Set Options */
		pAction_Run_Loop->Set_Options(TEXT("Run_Loop"), m_pModelCom, true, 0.f, false, false);
		pTsk_LookAt->Set_Option(m_pTransform);
		pTsk_Check_Distance->Set_Option(m_pTransform, true, 7.f, true);
		pSequence_Attack_Swing_BackHnd->Set_Attack_Action_Options(TEXT("Attack_Run_Swing_Front_BackHnd"), m_pModelCom);
		pSequence_Attack_Swing_BackHnd->Set_Attack_Option(7.f);
		pSequence_Attack_Swing_ForHnd->Set_Attack_Action_Options(TEXT("Attack_Run_Swing_Front_ForHnd"), m_pModelCom);
		pSequence_Attack_Swing_ForHnd->Set_Attack_Option(7.f);
		pSequence_Attack_Hop_ForHnd->Set_Attack_Action_Options(TEXT("Attack_Hop_Backhand_BackHnd"), m_pModelCom);
		pSequence_Attack_Hop_ForHnd->Set_Attack_Option(7.f);
		pSequence_Attack_Hop_BackHnd->Set_Attack_Action_Options(TEXT("Attack_Hop_Backhand_ForHnd"), m_pModelCom);
		pSequence_Attack_Hop_BackHnd->Set_Attack_Option(7.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Turn_Run"), pSequence_Turn_Run)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turn_Run");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Run_Loop"), pAction_Run_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Run_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Select_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swing_BackHnd"), pSequence_Attack_Swing_BackHnd, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swing_BackHnd");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swing_ForHnd"), pSequence_Attack_Swing_ForHnd, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swing_ForHnd");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Hop_ForHnd"), pSequence_Attack_Hop_ForHnd, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Hop_ForHnd");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Hop_BackHnd"), pSequence_Attack_Hop_BackHnd, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Hop_BackHnd");

		if (FAILED(pAction_Run_Loop->Assemble_Behavior(TEXT("Check_Distance"), pTsk_Check_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pAction_Run_Loop->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");

		if (FAILED(Make_Turn_Run(pSequence_Turn_Run)))
			throw TEXT("Failed Make_Turn_Run");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Pattern_Attack_Run : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Pattern_Attack_Charge(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Turns = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");
		CSequence* pSequence_Attack_Charge = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Charge)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Charge");

		CAction* pAction_Charge_Enter = nullptr;
		if (FAILED(Create_Behavior(pAction_Charge_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Charge_Enter");
		CAction* pAction_Charge_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Charge_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Charge_Loop");
		CAction* pAction_Charge_End = nullptr;
		if (FAILED(Create_Behavior(pAction_Charge_End)))
			throw TEXT("Failed Create_Behavior pAction_Charge_End");

		CLookAt* pTsk_LookAt_1 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_1)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_1");
		CLookAt* pTsk_LookAt_2 = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_2)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_2");
		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorations */

		/* Set Options */
		pTsk_LookAt_1->Set_Option(m_pTransform);
		pTsk_LookAt_2->Set_Option(m_pTransform);
		pAction_Charge_Enter->Set_Options(TEXT("Attack_Charge_Enter"), m_pModelCom);
		pAction_Charge_Loop->Set_Options(TEXT("Attack_Charge_Loop"), m_pModelCom, true, 0.f, false, false);
		pAction_Charge_End->Set_Options(TEXT("Attack_Charge_End_Turn_Left_180"), m_pModelCom);

		pTsk_Wait->Set_Timer(1.f);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSequence_Attack_Charge)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
		if (FAILED(pSequence_Attack_Charge->Assemble_Behavior(TEXT("Action_Charge_Enter"), pAction_Charge_Enter)))
			throw TEXT("Failed Assemble_Childs Action_Charge_Enter");
		if (FAILED(pSequence_Attack_Charge->Assemble_Behavior(TEXT("Action_Charge_Loop"), pAction_Charge_Loop)))
			throw TEXT("Failed Assemble_Childs Action_Charge_Loop");
		if (FAILED(pSequence_Attack_Charge->Assemble_Behavior(TEXT("Action_Charge_End"), pAction_Charge_End)))
			throw TEXT("Failed Assemble_Childs Action_Charge_End");

		if (FAILED(pAction_Charge_Enter->Assemble_Behavior(TEXT("Tsk_LookAt_1"), pTsk_LookAt_1)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_1");
		if (FAILED(pAction_Charge_Loop->Assemble_Behavior(TEXT("Tsk_LookAt_2"), pTsk_LookAt_2)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_2");
		if (FAILED(pAction_Charge_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Charge : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = nullptr;
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CAction* pAction_Left_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_45)))
			throw TEXT("Failed Create_Behavior pAction_Left_45");
		CAction* pAction_Right_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_45)))
			throw TEXT("Failed Create_Behavior pAction_Right_45");
		CAction* pAction_Left_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_90)))
			throw TEXT("Failed Create_Behavior pAction_Left_90");
		CAction* pAction_Right_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_90)))
			throw TEXT("Failed Create_Behavior pAction_Right_90");
		CAction* pAction_Left_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_135)))
			throw TEXT("Failed Create_Behavior pAction_Left_135");
		CAction* pAction_Right_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_135)))
			throw TEXT("Failed Create_Behavior pAction_Right_135");
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Idle_Turn_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Idle_Turn_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Idle_Turn_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Idle_Turn_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Idle_Turn_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Idle_Turn_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Idle_Turn_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Idle_Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CArmored_Troll::Make_Turn_Run(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turn_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = nullptr;
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CAction* pAction_Left_Front = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Front)))
			throw TEXT("Failed Create_Behavior pAction_Left_Front");
		CAction* pAction_Right_Front = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Front)))
			throw TEXT("Failed Create_Behavior pAction_Right_Front");
		CAction* pAction_Left_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_45)))
			throw TEXT("Failed Create_Behavior pAction_Left_45");
		CAction* pAction_Right_45 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_45)))
			throw TEXT("Failed Create_Behavior pAction_Right_45");
		CAction* pAction_Left_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_90)))
			throw TEXT("Failed Create_Behavior pAction_Left_90");
		CAction* pAction_Right_90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_90)))
			throw TEXT("Failed Create_Behavior pAction_Right_90");
		CAction* pAction_Left_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_135)))
			throw TEXT("Failed Create_Behavior pAction_Left_135");
		CAction* pAction_Right_135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_135)))
			throw TEXT("Failed Create_Behavior pAction_Right_135");
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");

		/* Set Decorations */

		/* Set Options */
		pAction_Left_Front->Set_Options(TEXT("Run_Start_Front"), m_pModelCom);
		pAction_Right_Front->Set_Options(TEXT("Run_Start_Front"), m_pModelCom);
		pAction_Left_45->Set_Options(TEXT("Turn_Start_Run_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Turn_Start_Run_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Turn_Start_Run_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Turn_Start_Run_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Turn_Start_Run_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Turn_Start_Run_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Turn_Start_Run_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Turn_Start_Run_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_FRONT, pAction_Left_Front)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_FRONT, pAction_Right_Front)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_FRONT");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Turn_Run : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CArmored_Troll::Enter_Light_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 10;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pWeapon->On_Collider_Attack(&m_CollisionRequestDesc);
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Whip_%d.wav"), 6, 0.3f);
	ENDINSTANCE;
}

void CArmored_Troll::Enter_Heavy_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 20;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pWeapon->On_Collider_Attack(&m_CollisionRequestDesc);
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Whip_%d.wav"), 6, 0.3f);
	ENDINSTANCE;
}

void CArmored_Troll::Enter_Body_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 0;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pRigidBody->Enable_Collision("Enemy_Body_Attack", this, &m_CollisionRequestDesc);
}

void CArmored_Troll::Exit_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_NONE;
	m_CollisionRequestDesc.iDamage = 0;
	m_pRigidBody->Disable_Collision("Enemy_Body_Attack");
	m_pWeapon->Off_Collider_Attack();
}

void CArmored_Troll::Play_FootStep_Sound()
{
	m_pStep_Shake->RandomUpAxisShake();
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Step.wav"), 0.55f);
	ENDINSTANCE;
}

void CArmored_Troll::Play_Hit_Sound()
{
	m_pHit_Shake->RandomUpAxisShake();
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Hit_%d.wav"), 5, 0.85f);
	ENDINSTANCE;
}

void CArmored_Troll::Play_Random_Attack_Sound()
{
	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Attack_%d.wav"), 5, 0.5f);
	ENDINSTANCE;
}

void CArmored_Troll::Play_Dead_Sound()
{
	m_pDeath_Shake->RandomUpAxisShake();

	BEGININSTANCE;
	pGameInstance->Play_Sound(TEXT("Troll_Step.wav"), 0.8f);
	ENDINSTANCE;
}

CArmored_Troll* CArmored_Troll::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArmored_Troll* pInstance = New CArmored_Troll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CArmored_Troll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArmored_Troll::Clone(void* pArg)
{
	CArmored_Troll* pInstance = New CArmored_Troll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CArmored_Troll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArmored_Troll::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_DarkAura.size(); i++)
	{
		Safe_Release(m_DarkAura[i]);
	}
	Safe_Release(m_pWeapon);
	Safe_Release(m_pEmissiveTexture);
	Safe_Release(m_pEyeEmissiveTexture);
	Safe_Release(m_pStep_Shake);
	Safe_Release(m_pHit_Shake);
	Safe_Release(m_pDeath_Shake);
}