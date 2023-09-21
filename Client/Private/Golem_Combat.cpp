#include "Golem_Combat.h"
#include "GameInstance.h"

#include "Weapon_Golem_Combat.h"

#include "Wait.h"
#include "Action.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "Random_Select.h"
#include "Action_Deflect.h"
#include "Selector_Degree.h"
#include "Sequence_Groggy.h"
#include "Sequence_Attack.h"
#include "Sequence_Levitated.h"
#include "Sequence_MoveTarget.h"

CGolem_Combat::CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGolem_Combat::CGolem_Combat(const CGolem_Combat& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGolem_Combat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGolem_Combat::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4* pWorldMatric = reinterpret_cast<_float4x4*>(pArg);
		m_pTransform->Set_WorldMatrix(*pWorldMatric);
	}
	else
		m_pTransform->Set_Position(_float3(15.f, 2.f, 15.f));

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	return S_OK;
}

void CGolem_Combat::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	// Test Code 
	BEGININSTANCE;
	
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_PRESSING))
	{
		if (pGameInstance->Get_DIKeyState(DIK_5, CInput_Device::KEY_DOWN))
			m_isSpawn = true;
		if (pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN))
			m_isParring = true;
		if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN))
			m_iCurrentSpell |= BUFF_LEVIOSO;
		if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN))
			m_iCurrentSpell |= BUFF_STUPEFY;
	}

	ENDINSTANCE;
	////////////////////////////

	Set_Current_Target();

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	_float3 vPosition = m_pTransform->Get_Position();
	for (auto iter = m_CurrentTickSpells.begin(); iter != m_CurrentTickSpells.end(); )
	{
		if (iter->first & m_iCurrentSpell)
		{
			//iter->second(vPosition, fTimeDelta);
			++iter;
		}
		else
			iter = m_CurrentTickSpells.erase(iter);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CGolem_Combat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CGolem_Combat::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = { TEXT("") };
	if (nullptr != CollisionEventDesc.pOtherCollisionTag)
		wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_float fDamage = pCollisionMagicBallDesc->fDamage;

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end())
			m_CurrentTickSpells.emplace(eBuff, Action);

		m_iCurrentSpell |= eBuff;
	}

	/* Collision Player Fig */
	if (wstring::npos != wstrCollisionTag.find(TEXT("Body")))
	{
		if (wstring::npos != wstrObjectTag.find(TEXT("Player")) ||
			wstring::npos != wstrObjectTag.find(TEXT("Fig")))
		{
			m_RangeInEnemies.push_back({ wstrObjectTag, CollisionEventDesc.pOtherOwner });
		}
	}
}

void CGolem_Combat::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = { TEXT("") };
	if (nullptr != CollisionEventDesc.pOtherCollisionTag)
		wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("Body")))
	{
		if (wstring::npos != wstrObjectTag.find(TEXT("Player")) ||
			wstring::npos != wstrObjectTag.find(TEXT("Fig")))
		{
			if (FAILED(Remove_GameObject(wstrObjectTag)))
			{
				MSG_BOX("[CGolem_Combat] Failed OnCollisionExit : \nFailed Remove_GameObject");
				return;
			}
		}
	}
}

HRESULT CGolem_Combat::Render()
{
#ifdef _DEBUG
	//Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");

			if (FAILED(m_pShaderCom->Begin("AnimMesh")))
				throw TEXT("Shader Begin AnimMesh");

			if (FAILED(m_pModelCom->Render(i)))
				throw TEXT("Model Render");
		}
		catch (const _tchar* pErrorTag)
		{
			wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MSG_BOX(wstrErrorMSG.c_str());

			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CGolem_Combat::Render_Depth()
{
	return S_OK;
}

HRESULT CGolem_Combat::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
#pragma region Add_Types
		if (FAILED(m_pRootBehavior->Add_Type("pTransform", m_pTransform)))
			throw TEXT("Failed Add_Type pTransform");
		if (FAILED(m_pRootBehavior->Add_Type("pModel", m_pModelCom)))
			throw TEXT("Failed Add_Type pModel");
		if (FAILED(m_pRootBehavior->Add_Type("isChangeAnimation", &m_isChangeAnimation)))
			throw TEXT("Failed Add_Type isChangeAnimation");

		if (FAILED(m_pRootBehavior->Add_Type("fTargetDistance", _float())))
			throw TEXT("Failed Add_Type fTargetDistance");
		if (FAILED(m_pRootBehavior->Add_Type("fAttackRange", _float())))
			throw TEXT("Failed Add_Type fAttackRange");
		if (FAILED(m_pRootBehavior->Add_Type("fTargetToDegree", _float())))
			throw TEXT("Failed Add_Type fTargetToDegree");
		if (FAILED(m_pRootBehavior->Add_Type("isTargetToLeft", _bool())))
			throw TEXT("Failed Add_Type isTargetToLeft");

		if (FAILED(m_pRootBehavior->Add_Type("isParring", &m_isParring)))
			throw TEXT("Failed Add_Type isParring");
		if (FAILED(m_pRootBehavior->Add_Type("isSpawn", &m_isSpawn)))
			throw TEXT("Failed Add_Type isSpawn");
		if (FAILED(m_pRootBehavior->Add_Type("iCurrentSpell", &m_iCurrentSpell)))
			throw TEXT("Failed Add_Type iCurrentSpell");

		m_pTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Player"), TEXT("GameObject_Player")));
		
		if (nullptr == m_pTarget)
			throw TEXT("m_pTarget is nullptr");
		if (FAILED(m_pRootBehavior->Add_Type("cppTarget", &m_pTarget)))
			throw TEXT("Failed Add_Type cppTarget");
#pragma endregion //Add_Types

		/* Make Child Behaviors */
		CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector)
			throw TEXT("pSelector is nullptr");

		CAction* pAction_Spawn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Spawn)
			throw TEXT("pAction_Spawn is nullptr");
		CSelector* pSelector_NormalAttack = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_NormalAttack)
			throw TEXT("pSelector_NormalAttack is nullptr");
		CSelector* pSelector_CheckSpell = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_CheckSpell)
			throw TEXT("pSelector_CheckSpell is nullptr");

		/* Set Decorations */
		pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return *pIsSpawn;
			});

		/* Set Options */
		pAction_Spawn->Set_Options(TEXT("Spawn_Fall_1"), m_pModelCom, false, 0.f, true, true);

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Spawn"), pAction_Spawn)))
			throw TEXT("Failed Assemble_Behavior Action_Spawn");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_NormalAttack"), pSelector_NormalAttack)))
			throw TEXT("Failed Assemble_Behavior Selector_NormalAttack");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_CheckSpell"), pSelector_CheckSpell)))
			throw TEXT("Failed Assemble_Behavior Selector_CheckSpell");

		if (FAILED(Make_Check_Spell(pSelector_CheckSpell)))
			throw TEXT("Failed Make_NormalAttack");
		if (FAILED(Make_NormalAttack(pSelector_NormalAttack)))
			throw TEXT("Failed Make_NormalAttack");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Add_Components()
{
	try /* Check Add_Components */
	{
		/* Com_Renderer */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
			TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
			throw TEXT("Com_Renderer");

		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Model_Golem_Combat"),
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

		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.2f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.7f, 1.5f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		/* For.Com_RigidBody */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		/* For.Collider_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(15.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");

		m_pRigidBody->Create_Collider(&RigidBodyDesc);

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Golem_Combat::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(LEVEL_MAINGAME, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::SetUp_ShaderResources()
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
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed SetUp_ShaderResources : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CGolem_Combat::Set_Current_Target()
{
	_float3 vPosition = m_pTransform->Get_Position();

	for (auto& Pair : m_RangeInEnemies)
	{
		if (nullptr == m_pTarget)
			m_pTarget = Pair.second;
		else
		{
			_float3 vSrcTargetPosition = m_pTarget->Get_Transform()->Get_Position();
			_float3 vDstTargetPosition = Pair.second->Get_Transform()->Get_Position();

			_float vSrcDistance = _float3::Distance(vPosition, vSrcTargetPosition);
			_float vDstDistance = _float3::Distance(vPosition, vDstTargetPosition);

			m_pTarget = (vSrcDistance < vDstDistance) ? m_pTarget : Pair.second;
		}
	}

	m_isRangeInEnemy = (0 < m_RangeInEnemies.size()) ? true : false;

	if (false == m_isRangeInEnemy)
	{
		m_pTarget = nullptr;
		/*BEGININSTANCE;
		m_pTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_MAINGAME, TEXT("Layer_Player"), TEXT("GameObject_Player")));
		ENDINSTANCE;*/
	}
}

HRESULT CGolem_Combat::Remove_GameObject(const wstring & wstrObjectTag)
{
	auto iter = find_if(m_RangeInEnemies.begin(), m_RangeInEnemies.end(), [&](auto& Pair)->_bool
		{
			if (wstring::npos != Pair.first.find(wstrObjectTag))
				return true;

			return false;
		});

	if (iter == m_RangeInEnemies.end())
		return E_FAIL;

	//Safe_Release(iter->second);
	m_RangeInEnemies.erase(iter);

	return S_OK;
}

#ifdef _DEBUG
void CGolem_Combat::Tick_ImGui()
{
	RECT clientRect;
	GetClientRect(g_hWnd, &clientRect);
	POINT leftTop = { clientRect.left, clientRect.top };
	POINT rightBottom = { clientRect.right, clientRect.bottom };
	ClientToScreen(g_hWnd, &leftTop);
	ClientToScreen(g_hWnd, &rightBottom);
	int Left = leftTop.x;
	int Top = rightBottom.y;
	ImVec2 vWinpos = { _float(Left + 1280.f), _float(Top - 300.f) };
	ImGui::SetNextWindowPos(vWinpos);

	ImGui::Begin("Test Golem_Combat");

	if (ImGui::Button("Set 0, 0, 0"))
		m_pTransform->Set_Position(_float3(0.f, 0.f, 0.f));

	ImGui::SeparatorText("Behavior");

	vector<wstring> DebugBehaviorTags = m_pRootBehavior->Get_DebugBahaviorTags();

	for (auto& Tag : DebugBehaviorTags)
	{
		ImGui::Text(wstrToStr(Tag).c_str());
	}

	ImGui::End();
}
#endif // _DEBUG

HRESULT CGolem_Combat::Make_Descendo(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Descendo */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Descendo1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo1)
			throw TEXT("pAction_Descendo1 is nullptr");
		CAction* pAction_Descendo2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo2)
			throw TEXT("pAction_Descendo2 is nullptr");
		CAction* pAction_Descendo3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo3)
			throw TEXT("pAction_Descendo3 is nullptr");
		CAction* pAction_Descendo4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Descendo4)
			throw TEXT("pAction_Descendo4 is nullptr");
		CAction* pAction_GetUp = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_GetUp)
			throw TEXT("pAction_GetUp is nullptr");

		/* Set Options */
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Descendo_3"), m_pModelCom);
		pAction_Descendo4->Set_Options(TEXT("Descendo_4"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Get_Up_Send_Front"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Descendo4"), pAction_Descendo4)))
			throw TEXT("Failed Assemble_Behavior pAction_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior pAction_GetUp");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Random_Idle_Move(_Inout_ CRandom_Select* pRandomSelect)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomSelect)
			throw TEXT("Parameter pRandomSelect is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Idle = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Idle)
			throw TEXT("pAction_Idle is nullptr");
		CSequence_MoveTarget* pSequence_Move_1StepLeft = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_Move_1StepLeft)
			throw TEXT("pSequence_Move_1StepLeft is nullptr");
		CSequence_MoveTarget* pSequence_Move_1StepRight = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_Move_1StepRight)
			throw TEXT("pSequence_Move_1StepRight is nullptr");

		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");

		/* Set Options */
		pAction_Idle->Set_Options(TEXT("Idle"), m_pModelCom, true);
		pSequence_Move_1StepLeft->Set_Action_Options(TEXT("1Step_Left"), m_pModelCom);
		pSequence_Move_1StepRight->Set_Action_Options(TEXT("1Step_Right"), m_pModelCom);
		pTsk_Wait->Set_Timer(1.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Action_Idle"), pAction_Idle, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Action_Idle");
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Sequence_Move_1StepLeft"), pSequence_Move_1StepLeft, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepLeft");
		if (FAILED(pRandomSelect->Assemble_Behavior(TEXT("Sequence_Move_1StepRight"), pSequence_Move_1StepRight, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepRight");

		if (FAILED(pAction_Idle->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Random_Idle_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
	{
		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CAction* pAction_Right_Back = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Back)
			throw TEXT("pAction_Right_Back is nullptr");
		CAction* pAction_Left_Back = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Back)
			throw TEXT("pAction_Left_Back is nullptr");
		CAction* pAction_Left90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left90)
			throw TEXT("pAction_Left90 is nullptr");
		CAction* pAction_Right90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right90)
			throw TEXT("pAction_Right90 is nullptr");
		CAction* pAction_Left135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left135)
			throw TEXT("pAction_Left135 is nullptr");
		CAction* pAction_Right135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right135)
			throw TEXT("pAction_Right135 is nullptr");
		/* Set Decorations */

		/* Set Options */
		pAction_Right_Back->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);
		pAction_Left_Back->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Left90->Set_Options(TEXT("Turn_Left_90"), m_pModelCom);
		pAction_Right90->Set_Options(TEXT("Turn_Right_90"), m_pModelCom);
		pAction_Left135->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right135->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_BACK, pAction_Right_Back)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_BACK, pAction_Left_Back)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_90, pAction_Left90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::LEFT_135, pAction_Left135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_90, pAction_Right90)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Childs(CSelector_Degree::RIGHT_135, pAction_Right135)))
			throw TEXT("Failed Assemble_Childs pSelector_Degree RIGHT_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Attack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack */
	{
		/* Make Child Behaviors */
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CRandom_Select* pRandom_Attack = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
		if (nullptr == pRandom_Attack)
			throw TEXT("pRandom_Attack is nullptr");

		CAction_Deflect* pAction_Protego_Deflect = dynamic_cast<CAction_Deflect*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action_Deflect")));
		if (nullptr == pAction_Protego_Deflect)
			throw TEXT("pAction_Protego_Deflect is nullptr");

		CSequence_Attack* pSequence_Attack_Slash = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Slash)
			throw TEXT("pSequence_Attack_Slash is nullptr");
		CSequence_Attack* pSequence_Attack_Shoulder = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Shoulder)
			throw TEXT("pSequence_Attack_Shoulder is nullptr");
		CSequence_Attack* pSequence_Attack_OverHand = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_OverHand)
			throw TEXT("pSequence_Attack_OverHand is nullptr");
		CSequence_Attack* pSequence_Attack_Jab = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Jab)
			throw TEXT("pSequence_Attack_Jab is nullptr");

		/* Set Decorations */
		pRandom_Attack->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				return !*pIsParring;
			});

		/* Set Options */
		pSequence_Attack_Slash->Set_Attack_Action_Options(TEXT("Attack_Slash_Sword"), m_pModelCom);
		pSequence_Attack_Slash->Set_Attack_Option(7.f);
		pSequence_Attack_Shoulder->Set_Attack_Action_Options(TEXT("Attack_Shoulder"), m_pModelCom);
		pSequence_Attack_Shoulder->Set_Attack_Option(4.f);
		pSequence_Attack_OverHand->Set_Attack_Action_Options(TEXT("Attack_OverHand_Sword"), m_pModelCom);
		pSequence_Attack_OverHand->Set_Attack_Option(7.5f);
		pSequence_Attack_Jab->Set_Attack_Action_Options(TEXT("Attack_Jab"), m_pModelCom);
		pSequence_Attack_Jab->Set_Attack_Option(4.5f);
		pRandom_Attack->Set_Option(5.f);

		pAction_Protego_Deflect->Set_Options(TEXT("Protego_Deflect"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("RandomChoose"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior RandomChoose");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Protego_Deflect"), pAction_Protego_Deflect)))
			throw TEXT("Failed Assemble_Behavior Action_Protego_Deflect");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Slash"), pSequence_Attack_Slash, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Slash");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Shoulder"), pSequence_Attack_Shoulder, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Shoulder");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_OverHand"), pSequence_Attack_OverHand, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_OverHand");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Jab"), pSequence_Attack_Jab, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Jab");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_NormalAttack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		CSequence* pSequence_Turns = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turns)
			throw TEXT("pSequence_Turns is nullptr");
		CSequence_MoveTarget* pSequence_MoveTarget = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_MoveTarget)
			throw TEXT("pSequence_MoveTarget is nullptr");
		CSequence_MoveTarget* pSequence_MoveBackTarget = dynamic_cast<CSequence_MoveTarget*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_MoveTarget")));
		if (nullptr == pSequence_MoveBackTarget)
			throw TEXT("pSequence_MoveBackTarget is nullptr");
		CSelector* pSelector_Attacks = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Attacks)
			throw TEXT("pSelector_Attacks is nullptr");
		CRandom_Select* pRandom_IdleMove = dynamic_cast<CRandom_Select*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Random_Select")));
		if (nullptr == pRandom_IdleMove)
			throw TEXT("pRandom_IdleMove is nullptr");

		/* Set Decorations */
		pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_NONE != *pICurrentSpell)
					return false;

				return true;
			});
		pSequence_MoveTarget->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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

				return fAttackRange < fTargetDistance;
			});
		pSequence_MoveBackTarget->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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

				return (fAttackRange - 1.f) > fTargetDistance;
			});
		pSelector_Attacks->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
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

				return (fAttackRange >= fTargetDistance && (fAttackRange - 1.f) <= fTargetDistance);
			});

		/* Set Options */
		pSequence_MoveTarget->Set_Action_Options(TEXT("Move_Front"), m_pModelCom);
		pSequence_MoveBackTarget->Set_Action_Options(TEXT("Move_Back"), m_pModelCom);

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveTarget"), pSequence_MoveTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveBackTarget"), pSequence_MoveBackTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveBackTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Attacks"), pSelector_Attacks)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attacks");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Idle_Move"), pRandom_IdleMove)))
			throw TEXT("Failed Assemble_Behavior Random_Idle_Move");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
		if (FAILED(Make_Attack(pSelector_Attacks)))
			throw TEXT("Failed Make_Attack");
		if (FAILED(Make_Random_Idle_Move(pRandom_IdleMove)))
			throw TEXT("Failed Make_Random_Idle_Move");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_NormalAttack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

HRESULT CGolem_Combat::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		/* Make Child Behaviors */
		CSequence_Groggy* pSequence_Groggy = dynamic_cast<CSequence_Groggy*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Groggy")));
		if (nullptr == pSequence_Groggy)
			throw TEXT("pSequence_Groggy is nullptr");
		CSequence_Levitated* pSequence_Levitated = dynamic_cast<CSequence_Levitated*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Levitated")));
		if (nullptr == pSequence_Levitated)
			throw TEXT("pSequence_Levitated is nullptr");
		CSequence* pSequence_Descendo = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Descendo)
			throw TEXT("pSequence_Descendo is nullptr");

		/* Set Decorations */
		pSelector->Add_Decoration([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_NONE == *pICurrentSpell)
					return false;

				return true;
			});

		/* Set_Options */
		pSequence_Groggy->Set_LoopTime(3.f);

		/* Stupefy */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Groggy"), pSequence_Groggy)))
			throw TEXT("Failed Assemble_Behavior Sequence_Groggy");
		/* Levioso */
		if(FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Levitated"), pSequence_Levitated)))
			throw TEXT("Failed Assemble_Behavior Sequence_Levitated");
		/* Descendo */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(Make_Descendo(pSequence_Descendo)))
			throw TEXT("Failed Make_Descendo");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Check_Spell : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

CGolem_Combat* CGolem_Combat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGolem_Combat* pInstance = New CGolem_Combat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CGolem_Combat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGolem_Combat::Clone(void* pArg)
{
	CGolem_Combat* pInstance = New CGolem_Combat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CGolem_Combat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolem_Combat::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pWeapon);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pRootBehavior);
	}
}
