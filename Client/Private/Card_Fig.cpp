#include "Card_Fig.h"

#include "Client_GameInstance_Functions.h"

#include "MagicSlot.h"
#include "MagicBall.h"
#include "Weapon_Fig_Wand.h"

#include "UI_Dissolve.h"
#include "Quest_Manager.h"
#include "Script.h"
#include "UI_Card.h"

#include "Action.h"
#include "LookAt.h"
#include "RigidMove.h"

CCard_Fig::CCard_Fig(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CCard_Fig::CCard_Fig(const CCard_Fig& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCard_Fig::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCard_Fig::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CARDFIGINITDESC* pInitDesc = reinterpret_cast<CARDFIGINITDESC*>(pArg);
		m_pParentWorldMatrix = pInitDesc->pParentWorldMatrix;
		m_OffsetMatrix = XMMatrixTranslation(1.f, 0.f, -2.f);
	}

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_pModelCom->Change_Animation(TEXT("Stand_Listen"));

	m_AttackTags.push_back(TEXT("Attack_Cast_Crucio"));
	m_AttackTags.push_back(TEXT("Attack_Cast_Bombarda"));
	m_AttackTags.push_back(TEXT("Attack_Cast_Finisher"));

	m_pMagicSlot->Set_OwnerType(CMagic_Sound_Manager::OWNER_FIG);
	m_pMagicSlot->Set_Volum(0.5f);

	return S_OK;
}

void CCard_Fig::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (false == m_isSpawn)
	{
		BEGININSTANCE;
		_float4x4 CameraWorldMatrix = *m_pParentWorldMatrix;
		_float4x4 WorldMatrix = m_OffsetMatrix * CameraWorldMatrix;
		m_pTransform->Set_WorldMatrix(WorldMatrix);
		ENDINSTANCE;
	}
	else
		m_pTransform->LookAt_Lerp(m_pTarget->Get_Transform()->Get_Position(), fTimeDelta, true);

	Jump_Up(fTimeDelta);

	Play_Script(fTimeDelta);

	m_pUI_Card->Set_PlayDissolve(m_isShowCard);
	if (m_isShowCard)
		m_pUI_Card->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, &m_SoundChannel, CModel::UPPERBODY, m_pTransform);
}

void CCard_Fig::Late_Tick(_float fTimeDelta)
{
	if (m_isPlayScript)
		m_pScripts[m_iScriptIndex]->Late_Tick(fTimeDelta);

	if (m_isShowCard && m_isInteraction == false)
		m_pUI_Card->Late_Tick(fTimeDelta);

	if (false == m_isSpawn)
		return;

	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
	}
}

HRESULT CCard_Fig::Render()
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
			wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CCard_Fig::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
			wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

void CCard_Fig::Spawn_Fig(CGameObject* pTarget)
{
	if (nullptr == pTarget)
		return;

	Safe_Release(m_pTarget);
	m_pTarget = pTarget;
	Safe_AddRef(m_pTarget);
	m_isSpawn = true;
}

HRESULT CCard_Fig::Make_AI()
{
	BEGININSTANCE;

	try
	{
		/* Add Types */
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("pModel", m_pModelCom)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("pRigidBody", m_pRigidBody)))
			throw TEXT("Failed Add_Type pRigidBody");

		if (FAILED(m_pRootBehavior->Add_Type("isSpawn", &m_isSpawn)))
			throw TEXT("Failed Add_Type isSpawn");
		if (FAILED(m_pRootBehavior->Add_Type("isJump", &m_isJump)))
			throw TEXT("Failed Add_Type isJump");
		if (FAILED(m_pRootBehavior->Add_Type("isChangeAnimation", &m_isChangeAnimation)))
			throw TEXT("Failed Add_Type isChangeAnimation");

		/* Create Child Behaviors */
		CSequence* pSequence = { nullptr };
		if (FAILED(Create_Behavior(pSequence)))
			throw TEXT("Failed Create_Behavior pSequence");

		CAction* pAction_Jump = { nullptr };
		if (FAILED(Create_Behavior(pAction_Jump)))
			throw TEXT("Failed Create_Behavior pAction_Jump");
		CAction* pAction_Land = { nullptr };
		if (FAILED(Create_Behavior(pAction_Land)))
			throw TEXT("Failed Create_Behavior pAction_Land");
		CAction* pAction_Attack_1 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Attack_1)))
			throw TEXT("Failed Create_Behavior pAction_Attack_1");
		CAction* pAction_Attack_2 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Attack_2)))
			throw TEXT("Failed Create_Behavior pAction_Attack_2");
		CAction* pAction_Attack_3 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Attack_3)))
			throw TEXT("Failed Create_Behavior pAction_Attack_3");
		CAction* pAction_Jump_Out = { nullptr };
		if (FAILED(Create_Behavior(pAction_Jump_Out)))
			throw TEXT("Failed Create_Behavior pAction_Jump_Out");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return true == *pIsSpawn;
			});
		pAction_Jump_Out->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsJump = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isJump", pIsJump)))
					return false;

				*pIsJump = true;

				return true;
			});
		pAction_Jump_Out->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				_bool* pIsJump = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isJump", pIsJump)))
					return false;

				*pIsSpawn = false;
				*pIsJump = false;

				return true;
			});

		/* Set Options */
		pAction_Jump->Set_Options(TEXT("Jump_Down_Run"), m_pModelCom);
		pAction_Land->Set_Options(TEXT("Soft"), m_pModelCom);
		pAction_Attack_1->Set_Options(TEXT("Attack_Cast_Crucio"), m_pModelCom);
		pAction_Attack_2->Set_Options(TEXT("Attack_Cast_Bombarda"), m_pModelCom);
		pAction_Attack_3->Set_Options(TEXT("Attack_Cast_Finisher"), m_pModelCom);
		pAction_Jump_Out->Set_Options(TEXT("Spawn_Out"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("pSequence"), pSequence)))
			throw TEXT("Failed Assemble_Behavior pSequence");

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Jump"), pAction_Jump)))
			throw TEXT("Failed Assemble_Behavior Action_Jump");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Land"), pAction_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Land");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_1"), pAction_Attack_1)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_2"), pAction_Attack_2)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_2");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_3"), pAction_Attack_3)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_3");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Jump_Out"), pAction_Jump_Out)))
			throw TEXT("Failed Assemble_Behavior Action_Jump_Out");

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Make_AI : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCard_Fig::Make_Magics()
{
	//Skill Magic CRUCIO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_CURSE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = CRUCIO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic BOMBARDA
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = BOMBARDA;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic FINISHER
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = FINISHER;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, CRUCIO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, BOMBARDA);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(2, FINISHER);

	return S_OK;
}

HRESULT CCard_Fig::Make_Notifies()
{
	function<void()> Func = [&] { (*this).Cast_Crucio(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Crucio"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Cast_Bombarda(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Bombarda"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Cast_Finisher(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Finisher"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func)))
		return E_FAIL;

	Func = [&] { (*this).On_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Gravity"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Off_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Gravity"), Func)))
		return E_FAIL;

	Func = [&] { (*this).Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCard_Fig::Add_Components()
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

		/* For.Com_RootBehavior */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RootBehavior"),
			TEXT("Com_RootBehavior"), reinterpret_cast<CComponent**>(&m_pRootBehavior))))
			throw TEXT("Com_RootBehavior");

		/* For.Com_ShadowShader */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
			TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShaderCom))))
			throw TEXT("Com_ShadowShader");

		/* Com_RigidBody */
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
		RigidBodyDesc.eCollisionFlag = COL_STATIC;

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

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

		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		m_pUI_Card = static_cast<CUI_Dissolve*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Dissolve")));

		CUI::UIDESC UIDesc;
		UIDesc.vCombinedXY = { 0.f, 0.f };
		UIDesc.fX = { 50.f };
		UIDesc.fY = { 440.f };
		UIDesc.fZ = { 0.5f };
		UIDesc.fSizeX = { 96.f };
		UIDesc.fSizeY = { 96.f };
		_tchar szTexturePath[MAX_PATH] = TEXT("../../Resources/UI/Game/UI_Edit/Card_Fig.png");
		lstrcpy(UIDesc.szTexturePath, szTexturePath);
		m_pUI_Card->Load(UIDesc);
		m_pUI_Card->Set_PlayDissolve(false);
		//m_pUI_Card->Set_Effecttype();



		m_pScripts.clear();
		_bool isCard = true;
		CScript* pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script"), &isCard));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_1_1.png"));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_1_2_Edit.png"));
		m_pScripts.push_back(pScript);

		pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script"), &isCard));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_2.png"));
		m_pScripts.push_back(pScript);

		pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script"), &isCard));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_3_1.png"));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_3_2.png"));
		m_pScripts.push_back(pScript);

		pScript = static_cast<CScript*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Script"), &isCard));
		pScript->Add_Script(TEXT("../../Resources/UI/Game/Script/Fig/Card_Fig_4.png"));
		m_pScripts.push_back(pScript);

		Safe_Release(pGameInstance);

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCard_Fig::SetUp_ShaderResources()
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

		_float3 vHairColor = { 0.7f, 0.7f, 0.7f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
			throw TEXT("Failed Bind_RawValue : g_vHairColor");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CCard_Fig::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
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
		wstring wstrErrorMSG = TEXT("[CCard_Fig] Failed SetUp_ShadowShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CCard_Fig::Jump_Up(const _float& fTimeDelta)
{
	if (false == m_isJump)
	{
		m_fTimeAcc = 0.f;
		return;
	}

	m_fTimeAcc += fTimeDelta;
	_float fWaitTime = 0.4f;

	if (m_fTimeAcc < fWaitTime)
		return;

	_float3 vForce = _float3();
	_float fUpTimeAcc = m_fTimeAcc - fWaitTime;
	_float fUpTime = 1.f;
	vForce = m_pTransform->Get_Up() * 300.f;

	vForce = vForce - vForce * (fUpTimeAcc / (fUpTime / 2.f));

	if (fUpTimeAcc < fUpTime)
	{
		m_pRigidBody->Add_Force(vForce);
	}
}

void CCard_Fig::Cast_Crucio()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Cast_Bombarda()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Cast_Finisher()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(2, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CCard_Fig::Shot_Magic()
{
	m_CastingMagic->Do_MagicBallState_To_Next();
}

void CCard_Fig::On_Gravity()
{
	if (nullptr != m_pRigidBody)
		m_pRigidBody->Set_Gravity(true);
}

void CCard_Fig::Off_Gravity()
{
	if (nullptr != m_pRigidBody)
		m_pRigidBody->Set_Gravity(false);
}

void CCard_Fig::Ready_Card_UI()
{
	if (!m_isShowCard)
	{
		CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
		Safe_AddRef(pQuest_Manager);

		if (pQuest_Manager->Is_Quest_Finished(TEXT("Quest_Potion")))
			m_isShowCard = true;

		Safe_Release(pQuest_Manager);
	}

	m_pUI_Card->Set_PlayDissolve(m_isShowCard);
}

void CCard_Fig::Script_Finish_Check()
{
	if (m_pScripts[ENTERVAULT]->Is_Finished() && !m_isEnterValutScriptEnd)
	{
		m_pScripts[ENTERVAULT]->Set_isRender(false);
		m_isEnterValutScriptEnd = true;
		m_isPlayScript = false;
	}

	if (m_pScripts[ENTERSANCTUM]->Is_Finished() && !m_isEnterSanctumScriptEnd)
	{
		m_pScripts[ENTERSANCTUM]->Set_isRender(false);
		m_isEnterSanctumScriptEnd = true;
		m_isPlayScript = false;
	}

	if (m_pScripts[CREATEDRAGON]->Is_Finished() && !m_isDragonHpDownScriptEnd)
	{
		m_pScripts[CREATEDRAGON]->Set_isRender(false);
		m_isDragonHpDownScriptEnd = true;
		m_isPlayScript = false;
	}

	if (m_pScripts[DRAGONHPDOWN]->Is_Finished() && !m_isDragonDeathScriptEnd)
	{
		m_pScripts[DRAGONHPDOWN]->Set_isRender(false);
		m_isDragonDeathScriptEnd = true;
		m_isPlayScript = false;
	}
}

void CCard_Fig::Play_Script(_float fTimeDelta)
{
	if (m_isEnterVault)
	{
		m_isPlayScript = true;
		m_isEnterVault = false;
		m_pScripts[ENTERVAULT]->Reset_Script();
		m_pScripts[ENTERVAULT]->Set_isRender(true);
		m_iScriptIndex = ENTERVAULT;
	}

	if (m_isEnterSanctum)
	{ 
		m_isPlayScript = true;
		m_isEnterSanctum = false;
		m_pScripts[ENTERSANCTUM]->Reset_Script();
		m_pScripts[ENTERSANCTUM]->Set_isRender(true);
		m_iScriptIndex = ENTERSANCTUM;
	}


	if (m_isCreateDragon)
	{
		m_isPlayScript = true;
		m_isCreateDragon = false;
		m_pScripts[CREATEDRAGON]->Reset_Script();
		m_pScripts[CREATEDRAGON]->Set_isRender(true);
		m_iScriptIndex = CREATEDRAGON;
	}

	if (m_isDragonHpDown)
	{
		m_isPlayScript = true;
		m_isDragonHpDown = false;
		m_pScripts[DRAGONHPDOWN]->Reset_Script();
		m_pScripts[DRAGONHPDOWN]->Set_isRender(true);
		m_iScriptIndex = DRAGONHPDOWN;
	}

	//if (m_isDragonDeath)
	//{
	//	m_isPlayScript = true;
	//	m_isDragonDeath = false;
	//	m_pScripts[DRAGONDEATH]->Reset_Script();
	//	m_pScripts[DRAGONDEATH]->Set_isRender(true);
	//	m_iScriptIndex = DRAGONDEATH;
	//}

	Script_Finish_Check();

	if (m_isPlayScript)
		m_pScripts[m_iScriptIndex]->Tick(fTimeDelta);
}

CCard_Fig* CCard_Fig::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCard_Fig* pInstance = New CCard_Fig(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CCard_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCard_Fig::Clone(void* pArg)
{
	CCard_Fig* pInstance = New CCard_Fig(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CCard_Fig");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCard_Fig::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShadowShaderCom);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pWeapon);
	Safe_Release(m_pRigidBody);
	Safe_Release(m_pRootBehavior);

	Safe_Release(m_pUI_Card);

	Safe_Release(m_pTarget);

	for (auto& pScript : m_pScripts)
	{
		Safe_Release(pScript);
	}
}
