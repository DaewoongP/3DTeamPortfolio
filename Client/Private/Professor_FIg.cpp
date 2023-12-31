#include "Professor_Fig.h"

#include "Client_GameInstance_Functions.h"

#include "Player.h"
#include "Action.h"
#include "Script.h"
#include "MagicSlot.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "Quest_Manager.h"
#include "Check_Distance.h"
#include "Selector_Degree.h"
#include "Sequence_Attack.h"
#include "Weapon_Fig_Wand.h"

CProfessor_Fig::CProfessor_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CProfessor_Fig::CProfessor_Fig(const CProfessor_Fig& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProfessor_Fig::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProfessor_Fig::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float3* pPosition = reinterpret_cast<_float3*>(pArg);
		m_pTransform->Set_Position(*pPosition);
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;
	m_pMagicSlot->Set_OwnerType(CMagic_Sound_Manager::OWNER_FIG);
	m_pMagicSlot->Set_Volum(0.5f);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	BEGININSTANCE;
	if (nullptr == m_pPlayer)
	{
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_Component_In_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("GameObject_Player")));
	}
	ENDINSTANCE;

	return S_OK;
}

void CProfessor_Fig::Tick(_float fTimeDelta)
{
	Set_Current_Target();

	__super::Tick(fTimeDelta);

	if (true == m_isFinishCombat)
		Tick_Script(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
}

void CProfessor_Fig::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_isFinishCombat)
		Late_Tick_Script(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CProfessor_Fig::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("Attack")))
	{
		CEnemy::COLLISIONREQUESTDESC* pDesc = static_cast<CEnemy::COLLISIONREQUESTDESC*>(CollisionEventDesc.pArg);

		if (nullptr == pDesc ||
			CEnemy::ATTACK_NONE == pDesc->eType)
		{
			return;
		}
		BEGININSTANCE;
		_tchar szVoiceTag[2][MAX_PATH] = { {TEXT("eleazarfig_13367.wav") },{TEXT("eleazarfig_13368.wav")} };
		//pGameInstance->Play_Sound(szVoiceTag[rand() % 2], CSound_Manager::SOUND_VOICE, 0.5f, true);
		ENDINSTANCE;
		//Protego
		if (BUFF_PROTEGO & m_iCurrentSpell)
		{

		}
		//피격중인 상태일 경우 무시
		/*else if (m_pStateContext->Is_Current_State(TEXT("Hit")))
		{

		}*/
		//Hit
		else
		{
			switch (pDesc->eType)
			{
			case CEnemy::ATTACK_NONE:
				break;

			case CEnemy::ATTACK_LIGHT:
				break;
			case CEnemy::ATTACK_HEAVY:
				break;

			case CEnemy::ATTACKTYPE_END:
			default:
				break;
			}
		}
	}
}

void CProfessor_Fig::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (false == IsEnemy(wstrObjectTag))
			return;

		Remove_GameObject(wstrObjectTag);
	}
}

HRESULT CProfessor_Fig::Render()
{
	if (FAILED(SetUp_ShaderResources()))
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

			if (0 == i)
			{
				if (FAILED(m_pShaderCom->Begin("HairMesh")))
					throw TEXT("Shader Begin HairMesh");
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
					throw TEXT("Bind_Material Normal");

				if (FAILED(m_pShaderCom->Begin("AnimMesh")))
					throw TEXT("Shader Begin AnimMesh");
			}

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CProfessor_Fig::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
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
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CProfessor_Fig::Make_AI()
{
	BEGININSTANCE;

	try /* Check Failed Make_AI */
	{
#pragma region Add_Types
		/* Add Types */
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("pModel", m_pModelCom)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("isRangeInEnemy", &m_isRangeInEnemy)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("isChangeAnimation", &m_isChangeAnimation)))
			throw TEXT("Failed Add_Type isChangeAnimation");
		if (FAILED(m_pRootBehavior->Add_Type("isFinishCombat", &m_isFinishCombat)))
			throw TEXT("Failed Add_Type isFinishCombat");
		if (FAILED(m_pRootBehavior->Add_Type("isSpawn", &m_isSpawn)))
			throw TEXT("Failed Add_Type isSpawn");

		if (FAILED(m_pRootBehavior->Add_Type("fTargetDistance", _float())))
			throw TEXT("Failed Add_Type fTargetDistance");
		if (FAILED(m_pRootBehavior->Add_Type("fTargetToDegree", _float())))
			throw TEXT("Failed Add_Type fTargetToDegree");
		if (FAILED(m_pRootBehavior->Add_Type("isTargetToLeft", _bool())))
			throw TEXT("Failed Add_Type isTargetToLeft");

		if (FAILED(m_pRootBehavior->Add_Type("cppTarget", &m_pTarget)))
			throw TEXT("Failed Add_Type cppTarget");
#pragma endregion //Add_Types

		/* Craete Child Behaviors */
		CSelector* pSelector = { nullptr };
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CAction* pAction_Break = { nullptr };
		if (FAILED(Create_Behavior(pAction_Break)))
			throw TEXT("Failed Create_Behavior pAction_Break");
		CSelector* pSelector_NonCombat = { nullptr };
		if (FAILED(Create_Behavior(pSelector_NonCombat)))
			throw TEXT("Failed Create_Behavior pSelector_NonCombat");
		CSelector* pSelector_Combat = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Combat)))
			throw TEXT("Failed Create_Behavior pSelector_Combat");

		/* Set Decorators */
		pAction_Break->Add_Decorator([&](CBlackBoard* pBlackBaord)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBaord->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return false == *pIsSpawn;
			});

		/* Set Options */
		pAction_Break->Set_Options(TEXT("Idle_Combat_Show"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Break"), pAction_Break)))
			throw TEXT("Failed Assemble_Behavior Action_Break");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_NonCombat"), pSelector_NonCombat)))
			throw TEXT("Failed Assemble_Behavior Selector_NonCombat");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Combat"), pSelector_Combat)))
			throw TEXT("Failed Assemble_Behavior Selector_Combat");

		if (FAILED(Make_Non_Combat(pSelector_NonCombat)))
			throw TEXT("Failed Make_Non_Combat");
		if (FAILED(Make_Combat(pSelector_Combat)))
			throw TEXT("Failed Make_Combat");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Magics()
{
	//Basic Magic BasicCast
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_ATTACK_LIGHT;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = BASICCAST;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
		m_MagicDesc = magicInitDesc;
	}

	//Skill Magic LEVIOSO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_LEVIOSO;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = LEVIOSO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, LEVIOSO);

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Notifies()
{
	function<void()> Func = [&] { (*this).Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Light(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Attack_Light"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Heavy(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Attack_Heavy"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Cast_Levioso(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Levioso"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Cast_Protego(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Protego"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProfessor_Fig::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Professor_Fig"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Shader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			throw TEXT("Com_Shader");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* For.Com_RootBehavior */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			TEXT("Com_RootBehavior"), reinterpret_cast<CComponent**>(&m_pRootBehavior))))
			throw TEXT("Com_RootBehavior");

		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.25f, 0.6f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Body");
		RigidBodyDesc.eThisCollsion = COL_NPC;
		RigidBodyDesc.eCollisionFlag = COL_STATIC | COL_ENEMY_RANGE;

		/* Com_RigidBody */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* Collision Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.85f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(10.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Range");
		RigidBodyDesc.eThisCollsion = COL_NPC_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_ENEMY;

		m_pRigidBody->Create_Collider(&RigidBodyDesc);

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Fig_Wand::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_Fig_Wand"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");

		/* For.MagicSlot */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_MagicSlot)");
			return E_FAIL;
		}

		BEGININSTANCE;

		m_pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script")));
		m_pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_1_1.png"));
		m_pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_1_2.png"));
		m_pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_1_3.png"));
		m_pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Fig_1_4.png"));

		ENDINSTANCE;

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CProfessor_Fig::SetUp_ShaderResources()
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");

		_float3 vHairColor = { 1.f, 1.f, 1.f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CProfessor_Fig::Set_Current_Target()
{
	BEGININSTANCE;
	_uint iCurrentLevelIndex = pGameInstance->Get_CurrentLevelIndex();
	auto pLayer = pGameInstance->Find_Components_In_Layer(iCurrentLevelIndex, TEXT("Layer_Monster"));
	ENDINSTANCE;

	if (nullptr == pLayer)
		return;

	m_pTarget = { nullptr };
	for (auto& Pair : *pLayer)
	{
		CGameObject* pGameObject = static_cast<CGameObject*>(Pair.second);
		_float3 vTargetPosition = pGameObject->Get_Transform()->Get_Position();
		_float3 vPosition = m_pTransform->Get_Position();

		if (10.f < _float3::Distance(vTargetPosition, vPosition))
			continue;

		if (nullptr == m_pTarget)
			m_pTarget = pGameObject;
		else
		{
			if (true == pGameObject->isDead())
				continue;

			_float3 vSrcTargetPosition = m_pTarget->Get_Transform()->Get_Position();
			_float3 vDstTargetPosition = pGameObject->Get_Transform()->Get_Position();

			_float vSrcDistance = _float3::Distance(vPosition, vSrcTargetPosition);
			_float vDstDistance = _float3::Distance(vPosition, vDstTargetPosition);

			m_pTarget = (vSrcDistance < vDstDistance) ? m_pTarget : pGameObject;
		}
	}

	m_isRangeInEnemy = (nullptr == m_pTarget) ? false : true;

	if (false == m_isRangeInEnemy)
		m_pTarget = m_pPlayer;
}

HRESULT CProfessor_Fig::Remove_GameObject(const wstring& wstrObjectTag)
{
	/*auto iter = m_RangeInEnemies.find(wstrObjectTag);
	if (iter == m_RangeInEnemies.end())
		return E_FAIL;

	m_RangeInEnemies.erase(iter);*/

	return S_OK;
}

_bool CProfessor_Fig::IsEnemy(const wstring& wstrObjectTag)
{
	/* 골렘인 경우 */
	if (wstring::npos != wstrObjectTag.find(TEXT("Golem")))
		return true;

	/* 트롤인 경우 */
	if (wstring::npos != wstrObjectTag.find(TEXT("Troll")))
		return true;

	/* 더그보그인 경우 */
	if (wstring::npos != wstrObjectTag.find(TEXT("Dugbog")))
		return true;

	/* 검은마법사인 경우 */
	if (wstring::npos != wstrObjectTag.find(TEXT("DarkWizard")))
		return true;

	return false;
}

HRESULT CProfessor_Fig::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	try /* Check SetUp_ShaderResources */
	{
		if (nullptr == m_pShadowShaderCom)
			throw TEXT("m_pShaderCom is nullptr");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
			throw TEXT("Failed Bind_Matrix : g_WorldMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ViewMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
			throw TEXT("Failed Bind_Matrix : g_ProjMatrix");

		if (FAILED(m_pShadowShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
			throw TEXT("Failed Bind_RawValue : g_fCamFar");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CAction* pAction_Right_45 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_45)))
			throw TEXT("Failed Create_Behavior pAction_Right_45");
		CAction* pAction_Left_45 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_45)))
			throw TEXT("Failed Create_Behavior pAction_Left_45");
		CAction* pAction_Left_90 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_90)))
			throw TEXT("Failed Create_Behavior pAction_Left_90");
		CAction* pAction_Right_90 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_90)))
			throw TEXT("Failed Create_Behavior pAction_Right_90");
		CAction* pAction_Left_135 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_135)))
			throw TEXT("Failed Create_Behavior pAction_Left_135");
		CAction* pAction_Right_135 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_135)))
			throw TEXT("Failed Create_Behavior pAction_Right_135");
		CAction* pAction_Left_180 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		CAction* pAction_Right_180 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");

		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Turn_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Turn_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Turn_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Turn_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Turn_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Turn_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_45, pAction_Left_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_45, pAction_Right_45)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_45");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right_90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right_135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Non_Combat(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Turns = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsRangeInEnemy = { nullptr };

				if (FAILED(pBlackBoard->Get_Type("isRangeInEnemy", pIsRangeInEnemy)))
					return false;

				return !*pIsRangeInEnemy;
			});
		pSequence_Turns->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsFinishCombat = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isFinishCombat", pIsFinishCombat)))
					return false;

				*pIsFinishCombat = true;

				return true;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Combat(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Attack_Degree = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Degree)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Degree");
		CSequence* pSequence_Attack_Combo1 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Combo1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Combo1");
		CAction* pAction_Levioso = { nullptr };
		if (FAILED(Create_Behavior(pAction_Levioso)))
			throw TEXT("Failed Create_Behavior pAction_Levioso");
		CAction* pAction_Protego = { nullptr };
		if (FAILED(Create_Behavior(pAction_Protego)))
			throw TEXT("Failed Create_Behavior pAction_Protego");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsRangeInEnemy = { nullptr };

				if (FAILED(pBlackBoard->Get_Type("isRangeInEnemy", pIsRangeInEnemy)))
					return false;

				return *pIsRangeInEnemy;
			});

		/* Set Options */
		pAction_Levioso->Set_Options(TEXT("Attack_Cast_Levioso"), m_pModelCom, false, 10.f);
		pAction_Protego->Set_Options(TEXT("Cast_Protego"), m_pModelCom, false, 10.f);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attack_Degree"), pSequence_Attack_Degree)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Degree");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Levioso"), pAction_Levioso)))
			throw TEXT("Failed Assemble_Behavior Action_Levioso");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Protego"), pAction_Protego)))
			throw TEXT("Failed Assemble_Behavior Action_Protego");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attack_Combo1"), pSequence_Attack_Combo1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Combo1");

		if (FAILED(Make_Attack_Degree(pSequence_Attack_Degree)))
			throw TEXT("Failed Make_Attack_Degree");
		if (FAILED(Make_Attack_Combo1(pSequence_Attack_Combo1)))
			throw TEXT("Failed Make_Attack_Combo1");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Attack_Combo1(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CSequence_Attack* pAttack_Combo1 = { nullptr };
		if (FAILED(Create_Behavior(pAttack_Combo1)))
			throw TEXT("Failed Create_Behavior pAttack_Combo1");
		CSequence_Attack* pAttack_Combo2 = { nullptr };
		if (FAILED(Create_Behavior(pAttack_Combo2)))
			throw TEXT("Failed Create_Behavior pAttack_Combo2");
		CSequence_Attack* pAttack_Combo3 = { nullptr };
		if (FAILED(Create_Behavior(pAttack_Combo3)))
			throw TEXT("Failed Create_Behavior pAttack_Combo3");
		CSequence_Attack* pAttack_Combo4 = { nullptr };
		if (FAILED(Create_Behavior(pAttack_Combo4)))
			throw TEXT("Failed Create_Behavior pAttack_Combo4");

		/* Set Decorations */

		/* Set Options */
		pAttack_Combo1->Set_Attack_Action_Options(TEXT("Attack_Cast_Light_Front_1"), m_pModelCom);
		pAttack_Combo1->Set_Attack_Option(10.f);
		pAttack_Combo2->Set_Attack_Action_Options(TEXT("Attack_Cast_Light_Step_Back_2"), m_pModelCom);
		pAttack_Combo2->Set_Attack_Option(10.f);
		pAttack_Combo3->Set_Attack_Action_Options(TEXT("Attack_Cast_Light_Step_Back_3"), m_pModelCom);
		pAttack_Combo3->Set_Attack_Option(10.f);
		pAttack_Combo4->Set_Attack_Action_Options(TEXT("Attack_Cast_Heavy_Front_2"), m_pModelCom);
		pAttack_Combo4->Set_Attack_Option(10.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Attack_Combo1"), pAttack_Combo1)))
			throw TEXT("Failed Assemble_Behavior Attack_Combo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Attack_Combo2"), pAttack_Combo2)))
			throw TEXT("Failed Assemble_Behavior Attack_Combo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Attack_Combo3"), pAttack_Combo3)))
			throw TEXT("Failed Assemble_Behavior Attack_Combo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Attack_Combo4"), pAttack_Combo4)))
			throw TEXT("Failed Assemble_Behavior Attack_Combo1");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CProfessor_Fig::Make_Attack_Degree(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Degree */
	{
		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CAction* pAction_Right_45 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_45)))
			throw TEXT("Failed Create_Behavior pAction_Right_45");
		CAction* pAction_Left_45 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_45)))
			throw TEXT("Failed Create_Behavior pAction_Left_45");
		CAction* pAction_Left_90 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_90)))
			throw TEXT("Failed Create_Behavior pAction_Left_90");
		CAction* pAction_Right_90 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_90)))
			throw TEXT("Failed Create_Behavior pAction_Right_90");
		CAction* pAction_Left_135 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_135)))
			throw TEXT("Failed Create_Behavior pAction_Left_135");
		CAction* pAction_Right_135 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_135)))
			throw TEXT("Failed Create_Behavior pAction_Right_135");
		CAction* pAction_Left_180 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		CAction* pAction_Right_180 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Attack_Cast_Light_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Attack_Cast_Light_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Attack_Cast_Light_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Attack_Cast_Light_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Attack_Cast_Light_Left_135"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Attack_Cast_Light_Right_135"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Attack_Cast_Light_Right_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Attack_Cast_Light_Right_180"), m_pModelCom);

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
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CProfessor_Fig] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CProfessor_Fig::Attack_Light()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_LIGHT;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	_float4x4 OffsetMatrix = _float4x4();
	if (nullptr != m_pTarget)
		OffsetMatrix = m_pTarget->Get_Offset_Matrix();

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CProfessor_Fig::Attack_Heavy()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_HEAVY;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	_float4x4 OffsetMatrix = _float4x4();
	if (nullptr != m_pTarget)
		OffsetMatrix = m_pTarget->Get_Offset_Matrix();

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CProfessor_Fig::Cast_Levioso()
{
	if (nullptr == m_pTarget)
		return;

	_float4x4 OffsetMatrix = _float4x4();
	if (nullptr != m_pTarget)
		OffsetMatrix = m_pTarget->Get_Offset_Matrix();

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CProfessor_Fig::Cast_Protego()
{
	if (nullptr == m_pTarget)
		return;

	CMagic::MAGICDESC magicInitDesc;
	magicInitDesc.eBuffType = BUFF_PROTEGO;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_ALL;
	magicInitDesc.eMagicTag = PROTEGO;
	magicInitDesc.fInitCoolTime = 0.f;
	magicInitDesc.iDamage = 0;
	magicInitDesc.fLifeTime = 5.f;
	magicInitDesc.fScale = 4.0f;
	m_pMagicSlot->Add_Magics(magicInitDesc);

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(1, this, m_pWeapon, COL_ENEMY_ATTACK);
}

void CProfessor_Fig::Shot_Magic()
{
	m_CastingMagic->Do_MagicBallState_To_Next();
}

void CProfessor_Fig::Tick_Script(_float fTimeDelta)
{
	if (!m_isPlayScript)
	{
		m_isPlayScript = true;
		m_pScript->Reset_Script();
		m_pScript->Set_isRender(true);
	}

	if (m_isPlayScript)
	{
		m_pScript->Tick(fTimeDelta);

		if (true == m_pScript->Is_Finished())
		{
			CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
			Safe_AddRef(pQuest_Manager);
			pQuest_Manager->Clear_Quest(TEXT("Quest_Save_Fig"));
			Safe_Release(pQuest_Manager);
			g_isNight = true;
		}
	}
}

void CProfessor_Fig::Late_Tick_Script(_float fTimeDelta)
{
	if (m_isPlayScript)
		m_pScript->Late_Tick(fTimeDelta);
}

CProfessor_Fig* CProfessor_Fig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProfessor_Fig* pInstance = New CProfessor_Fig(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CProfessor_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProfessor_Fig::Clone(void* pArg)
{
	CProfessor_Fig* pInstance = New CProfessor_Fig(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CProfessor_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProfessor_Fig::Free()
{
	__super::Free();

	Safe_Release(m_pRootBehavior);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pWeapon);
	Safe_Release(m_pScript);
}
