#include "Armored_Troll.h"
#include "GameInstance.h"
#include "Weapon_Armored_Troll.h"

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

CArmored_Troll::CArmored_Troll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CArmored_Troll::CArmored_Troll(const CArmored_Troll& rhs)
	: CEnemy(rhs)
{
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

	if (nullptr != pArg)
	{
		_float4x4* pWorldMatrix = reinterpret_cast<_float4x4*>(pArg);
		m_pTransform->Set_WorldMatrix(*pWorldMatrix);
	}
	else
		m_pTransform->Set_Position(_float3(20.f, 2.f, 20.f));

	if (FAILED(Add_Components()))
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

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	return S_OK;
}

void CArmored_Troll::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CArmored_Troll::Late_Tick(_float fTimeDelta)
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

void CArmored_Troll::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		m_pHealth->Damaged(iDamage);

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() && BUFF_LEVIOSO == eBuff)
		{
			m_CurrentTickSpells.emplace(eBuff, Action);
		}

		m_iCurrentSpell |= eBuff;
	}
}

HRESULT CArmored_Troll::Render()
{
#ifdef _DEBUG
	//Tick_ImGui();
#endif // _DEBUG

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CGolem_Combat] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArmored_Troll::Make_AI()
{
	BEGININSTANCE;

	try
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		m_pTarget = dynamic_cast<CGameObject*>(pGameInstance->Find_Component_In_Layer(LEVEL_CLIFFSIDE, TEXT("Layer_Player"), TEXT("GameObject_Player")));
		if (nullptr == m_pTarget)
			throw TEXT("m_pTarget is nullptr");

		/* Make Childs */
		CSelector* pSelector = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector)
			throw TEXT("pSelector is nullptr");

		CSequence* pSequence_Death = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Death)
			throw TEXT("pSequence_Death is nullptr");
		CSelector* pSelector_Alive = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Alive)
			throw TEXT("pSelector_Alive is nullptr");

		CSequence* pSequence_Attacks_Degree = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attacks_Degree)
			throw TEXT("pSequence_Attacks_Degree is nullptr");
		CRandomChoose* pRandom_Select_Attacks_Far = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Select_Attacks_Far)
			throw TEXT("pRandom_Select_Attakcs_Far is nullptr");
		CSequence* pSequence_Taunts = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Taunts)
			throw TEXT("pSequence_Taunts is nullptr");
		CCheck_Distance* pTsk_Check_Distance = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pTsk_Check_Distance)
			throw TEXT("pTsk_Check_Distance is nullptr");

		/* Set Decorator */
		pSelector_Alive->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});

		/* Set Options */
		pTsk_Check_Distance->Set_Transform(m_pTransform);

		/* Bind Childs */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("RootBehavior Assemble pSelector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Selector Assemble Sequence_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Selector Assemble Selector_Alive");

		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Sequence_Attacks_Degree"), pSequence_Attacks_Degree)))
			throw TEXT("Failed Selector Assemble_Behavior Sequence_Attacks_Degree");
		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Random_Select_Attacks_Far"), pRandom_Select_Attacks_Far)))
			throw TEXT("Failed Selector Assemble Random_Select_Attacks_Far");
		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Sequence_Taunts"), pSequence_Taunts)))
			throw TEXT("Failed Selector Assemble_Behavior Sequence_Taunts");
		if (FAILED(pSelector_Alive->Assemble_Behavior(TEXT("Tsk_Check_Distance"), pTsk_Check_Distance)))
			throw TEXT("Failed Selector Assemble Tsk_Check_Distance");

		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Attack_Degree(pSequence_Attacks_Degree)))
			throw TEXT("Failed Make_Attack_Degree");
		if (FAILED(Make_Pattern_Attack_Far(pRandom_Select_Attacks_Far)))
			throw TEXT("Failed Make_Pattern_Attack_Far");

		if (FAILED(Make_Taunt_Degree(pSequence_Taunts)))
			throw TEXT("Failed Make_Taunt_Degree");
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
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Run_Start_Front"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_1Step_Swing_Front_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_2Step_Swing_Front_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Front_ForHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Left_90_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Right_180_ForHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_ForHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_ForHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Left_90_ForHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Right_90_BackHnd"), TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Light_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_1Step_Swing_Front_BackHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_2Step_Swing_Front_BackHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Front_ForHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Left_90_BackHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Right_180_ForHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_ForHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_BackHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_ForHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Left_90_ForHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Right_90_BackHnd"), TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Heavy_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Hop_Backhand_BackHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Hop_Backhand_ForHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_LeftHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_RightHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Left_180"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Left_90"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Right_180"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Right_90"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_BackHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_ForHnd"), TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Body_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Charge_Enter"), TEXT("Enter_Body_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Exit_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Charge_End"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Hop_Backhand_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Hop_Backhand_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_LeftHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_RightHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Left_180"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Left_90"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Right_180"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Overhead_Slam_Right_90"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_1Step_Swing_Front_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_2Step_Swing_Front_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Front_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Left_90_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Step_Swing_Right_180_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Front_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_BackHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Run_Swing_Front_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Left_90_ForHnd"), TEXT("Exit_Attack"), Func)))
		return E_FAIL;
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Attack_Swing_Right_90_BackHnd"), TEXT("Exit_Attack"), Func)))
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
		HealthDesc.iMaxHP = 500;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.5f, 0.f);
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
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_NPC_RANGE | COL_MAGIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider_Body_Attack */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC;
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
		m_pUI_HP = dynamic_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
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

HRESULT CArmored_Troll::SetUp_ShaderResources()
{
	return __super::SetUp_ShaderResources();
}

#ifdef _DEBUG
void CArmored_Troll::Tick_ImGui()
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

	ImGui::Begin("Test Troll");

	string strHp = to_string(m_pHealth->Get_HP());
	ImGui::Text("Current HP");
	ImGui::Text(strHp.c_str());

	ImGui::SeparatorText("Behavior");

	vector<wstring> DebugBehaviorTags = m_pRootBehavior->Get_DebugBahaviorTags();

	for (auto& Tag : DebugBehaviorTags)
	{
		ImGui::Text(wstrToStr(Tag).c_str());
	}

	ImGui::End();
}

#endif // _DEBUG

void CArmored_Troll::DeathBehavior(const _float& fTimeDelta)
{
	m_isDead = true;

	m_fDeadTimeAcc += fTimeDelta;
	if (3.f < m_fDeadTimeAcc)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CArmored_Troll::Make_Turns(_Inout_ CSequence* pSequence)
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

		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_90)
			throw TEXT("pAction_Left_90 is nullptr");
		CAction* pAction_Right_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_90)
			throw TEXT("pAction_Right_90 is nullptr");
		CAction* pAction_Left_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_135)
			throw TEXT("pAction_Left_135 is nullptr");
		CAction* pAction_Right_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_135)
			throw TEXT("pAction_Right_135 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");
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

		pTsk_Check_Degree->Set_Transform(m_pTransform);

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
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CAction* pAction_Left_Front = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front)
			throw TEXT("pAction_Left_Front is nullptr");
		CAction* pAction_Right_Front = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front)
			throw TEXT("pAction_Right_Front is nullptr");
		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_90)
			throw TEXT("pAction_Left_90 is nullptr");
		CAction* pAction_Right_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_90)
			throw TEXT("pAction_Right_90 is nullptr");
		CAction* pAction_Left_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_135)
			throw TEXT("pAction_Left_135 is nullptr");
		CAction* pAction_Right_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_135)
			throw TEXT("pAction_Right_135 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");
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

		pTsk_Check_Degree->Set_Transform(m_pTransform);

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

HRESULT CArmored_Troll::Make_Attack_Degree(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Degree */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pTsk_Check_Distance = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pTsk_Check_Distance)
			throw TEXT("pTsk_Check_Distance is nullptr");
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");

		CSequence* pSequence_Attack_Left_Front = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Left_Front)
			throw TEXT("pSequence_Attack_Left_Front is nullptr");
		CSequence* pSequence_Attack_Right_Front = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Right_Front)
			throw TEXT("pSequence_Attack_Right_Front is nullptr");
		CSequence* pSequence_Attack_Left_45 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Left_45)
			throw TEXT("pSequence_Attack_Left_45 is nullptr");
		CSequence* pSequence_Attack_Left_90 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Left_90)
			throw TEXT("Sequence_Attack_Left_90 is nullptr");
		CSequence* pSequence_Attack_Left_135 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Left_135)
			throw TEXT("pSequence_Attack_Left_135 is nullptr");
		CSequence* pSequence_Attack_Left_180 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Left_180)
			throw TEXT("pSequence_Attack_Left_180 is nullptr");
		CSequence* pSequence_Attack_Right_45 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Right_45)
			throw TEXT("pSequence_Attack_Right_45 is nullptr");
		CSequence* pSequence_Attack_Right_90 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Right_90)
			throw TEXT("pSequence_Attack_Right_90 is nullptr");
		CSequence* pSequence_Attack_Right_135 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Right_135)
			throw TEXT("pSequence_Attack_Right_135 is nullptr");
		CSequence* pSequence_Attack_Right_180 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Right_180)
			throw TEXT("pSequence_Attack_Right_180 is nullptr");

		/* Set Decorations */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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

		/* Set Options */
		pSelector_Degree->Set_Option(1.5f);
		pTsk_Check_Distance->Set_Transform(m_pTransform);
		pTsk_Check_Degree->Set_Transform(m_pTransform);

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

		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_Left_Front)))
			throw TEXT("Failed Make_Pattern_Attack_ForHnd");
		if (FAILED(Make_Attack_Left_45(pSequence_Attack_Left_45)))
			throw TEXT("Failed Make_Attack_Left_45");
		if (FAILED(Make_Attack_Left_90(pSequence_Attack_Left_90)))
			throw TEXT("Failed Make_Attack_Left_90");
		if (FAILED(Make_Attack_Left_135(pSequence_Attack_Left_135)))
			throw TEXT("Failed Make_Attack_Left_135");
		if (FAILED(Make_Attack_Left_180(pSequence_Attack_Left_180)))
			throw TEXT("Failed Make_Attack_Left_180");

		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_Right_Front)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Attack_Right_45(pSequence_Attack_Right_45)))
			throw TEXT("Failed Make_Attack_Right_45");
		if (FAILED(Make_Attack_Right_90(pSequence_Attack_Right_90)))
			throw TEXT("Failed Make_Attack_Right_90");
		if (FAILED(Make_Attack_Right_135(pSequence_Attack_Right_135)))
			throw TEXT("Failed Make_Attack_Right_135");
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

HRESULT CArmored_Troll::Make_Pattern_Attack_BackHnd(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_BackHnd */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Distance* pCheck_Distance_Enter = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pCheck_Distance_Enter)
			throw TEXT("pCheck_Distance_Enter is nullptr");
		CRandomChoose* pRandom_Select = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Select)
			throw TEXT("pRandom_Select is nullptr");
		CCheck_Distance* pCheck_Distance_Exit = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pCheck_Distance_Exit)
			throw TEXT("pCheck_Distance_Exit is nullptr");

		CAction* pAttack_Swing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_Swing)
			throw TEXT("pAttack_Swing is nullptr");
		CAction* pAttack_1Step_Swing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_1Step_Swing)
			throw TEXT("pAttack_1Step_Swing is nullptr");
		CAction* pAttack_2Step_Swing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_2Step_Swing)
			throw TEXT("pAttack_2Step_Swing is nullptr");
		CAction* pAttack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_Overhead)
			throw TEXT("pAttack_Overhead is nullptr");

		CLookAt* pTsk_LookAt_1 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_1)
			throw TEXT("pTsk_LookAt_1 is nullptr");
		CLookAt* pTsk_LookAt_2 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_2)
			throw TEXT("pTsk_LookAt_2 is nullptr");
		CLookAt* pTsk_LookAt_3 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_3)
			throw TEXT("pTsk_LookAt_3 is nullptr");

		/* Set Decorations */
		pCheck_Distance_Enter->Set_Transform(m_pTransform);
		pCheck_Distance_Exit->Set_Transform(m_pTransform);

		/* Set Options */
		pAttack_Swing->Set_Options(TEXT("Attack_Swing_Front_BackHnd"), m_pModelCom);
		pAttack_1Step_Swing->Set_Options(TEXT("Attack_1Step_Swing_Front_BackHnd"), m_pModelCom);
		pAttack_2Step_Swing->Set_Options(TEXT("Attack_2Step_Swing_Front_BackHnd"), m_pModelCom);
		pAttack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_RightHnd"), m_pModelCom);

		pTsk_LookAt_1->Set_Transform(m_pTransform);
		pTsk_LookAt_2->Set_Transform(m_pTransform);
		pTsk_LookAt_3->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance_Enter"), pCheck_Distance_Enter)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance_Exit"), pCheck_Distance_Exit)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Exit");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_Swing"), pAttack_Swing, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Attack_Swing");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_1Step_Swing"), pAttack_1Step_Swing, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Attack_1Step_Swing");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_2Step_Swing"), pAttack_2Step_Swing, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Attack_2Step_Swing");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_Overhead"), pAttack_Overhead, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Attack_Overhead");

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
		CCheck_Distance* pCheck_Distance_Enter = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pCheck_Distance_Enter)
			throw TEXT("pCheck_Distance_Enter is nullptr");
		CRandomChoose* pRandom_Select = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Select)
			throw TEXT("pRandom_Select is nullptr");
		CCheck_Distance* pCheck_Distance_Exit = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pCheck_Distance_Exit)
			throw TEXT("pCheck_Distance_Exit is nullptr");

		CAction* pAttack_Swing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_Swing)
			throw TEXT("pAttack_Swing is nullptr");
		CAction* pAttack_Step_Swing = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_Step_Swing)
			throw TEXT("pAttack_Step_Swing is nullptr");
		CAction* pAttack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAttack_Overhead)
			throw TEXT("pAttack_Overhead is nullptr");

		CLookAt* pTsk_LookAt_1 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_1)
			throw TEXT("pTsk_LookAt_1 is nullptr");
		CLookAt* pTsk_LookAt_2 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_2)
			throw TEXT("pTsk_LookAt_2 is nullptr");

		/* Set Decorations */

		/* Set Options */
		pCheck_Distance_Enter->Set_Transform(m_pTransform);
		pCheck_Distance_Exit->Set_Transform(m_pTransform);
		pAttack_Swing->Set_Options(TEXT("Attack_Swing_Front_ForHnd"), m_pModelCom);
		pAttack_Step_Swing->Set_Options(TEXT("Attack_Step_Swing_Front_ForHnd"), m_pModelCom);
		pAttack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_LeftHnd"), m_pModelCom);

		pTsk_LookAt_1->Set_Transform(m_pTransform);
		pTsk_LookAt_2->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance_Enter"), pCheck_Distance_Enter)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance_Exit"), pCheck_Distance_Exit)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Exit");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_Swing"), pAttack_Swing, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Attack_Swing");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_Step_Swing"), pAttack_Step_Swing, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Attack_Step_Swing");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Attack_Overhead"), pAttack_Overhead, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Attack_Overhead");

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

HRESULT CArmored_Troll::Make_Attack_Left_45(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Left_45 */
	{
		if (nullptr == pSequence)
			throw TEXT("pSequence pSequence is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Attack_ForHnd1 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_ForHnd1)
			throw TEXT("pSequence_Attack_ForHnd1 is nullptr");
		CSequence* pSequence_Attack_BackHnd = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_BackHnd)
			throw TEXT("pSequence_Attack_BackHnd is nullptr");
		CSequence* pSequence_Attack_ForHnd2 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_ForHnd2)
			throw TEXT("pSequence_Attack_ForHnd2 is nullptr");

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
		CRandomChoose* pRandom_Select = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Select)
			throw TEXT("pRandom_Select is nullptr");

		CSequence* pSequence_Attack_BackHnd = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_BackHnd)
			throw TEXT("pSequence_Attack_BackHnd is nullptr");
		CSequence* pSequence_Attack_ForHnd = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_ForHnd)
			throw TEXT("pSequence_Attack_ForHnd is nullptr");

		CAction* pAction_Attack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Overhead)
			throw TEXT("pAction_Attack_Overhead is nullptr");
		CAction* pAction_Attack_BackHnd = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_BackHnd)
			throw TEXT("pAction_Attack_BackHnd is nullptr");
		CAction* pAction_Attack_ForHnd = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_ForHnd)
			throw TEXT("pAction_Attack_ForHnd is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Left_90"), m_pModelCom);
		pAction_Attack_BackHnd->Set_Options(TEXT("Attack_Step_Swing_Left_90_BackHnd"), m_pModelCom);
		pAction_Attack_ForHnd->Set_Options(TEXT("Attack_Swing_Left_90_ForHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("pSequence_Attack_BackHnd"), pSequence_Attack_BackHnd, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_BackHnd");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("pSequence_Attack_ForHnd"), pSequence_Attack_ForHnd, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Action_Overhead"), pAction_Attack_Overhead, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");

		if (FAILED(pSequence_Attack_BackHnd->Assemble_Behavior(TEXT("Action_Attack_ForHnd"), pAction_Attack_ForHnd)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_ForHnd");
		if (FAILED(pSequence_Attack_ForHnd->Assemble_Behavior(TEXT("Action_Attack_ForHnd"), pAction_Attack_BackHnd)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_ForHnd");

		if (FAILED(Make_Pattern_Attack_BackHnd(pSequence_Attack_BackHnd)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
		if (FAILED(Make_Pattern_Attack_ForHnd(pSequence_Attack_ForHnd)))
			throw TEXT("Failed Make_Pattern_Attack_BackHnd");
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

HRESULT CArmored_Troll::Make_Attack_Left_135(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Left_135 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Turn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Turn)
			throw TEXT("pAction_Turn is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Turn->Set_Options(TEXT("Idle_Turn_Left_135"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Turn_Left_135"), pAction_Turn)))
			throw TEXT("Failed Assemble_Behavior Action_Turn_Left_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Left_135 : \n");
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
		CAction* pAction_Attack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Overhead)
			throw TEXT("pSequence_Attack_Overhead is nullptr");

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

HRESULT CArmored_Troll::Make_Attack_Right_45(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Right_45 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Attack_BackHnd1 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_BackHnd1)
			throw TEXT("pSequence_Attack_BackHnd1 is nullptr");
		CSequence* pSequence_Attack_ForHnd = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_ForHnd)
			throw TEXT("pSequence_Attack_ForHnd is nullptr");
		CSequence* pSequence_Attack_BackHnd2 = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_BackHnd2)
			throw TEXT("pSequence_Attack_BackHnd2 is nullptr");

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
		CRandomChoose* pRandom_Select = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Select)
			throw TEXT("pRandom_Select is nullptr");

		CSequence* pSequence_Attack_ForHnd = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_ForHnd)
			throw TEXT("pSequence_Attack_ForHnd is nullptr");

		CAction* pAction_Attack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Overhead)
			throw TEXT("pAction_Attack_Overhead is nullptr");
		CAction* pAction_Attack_BackHnd = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_BackHnd)
			throw TEXT("pAction_Attack_BackHnd is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Attack_Overhead->Set_Options(TEXT("Attack_Overhead_Slam_Right_90"), m_pModelCom);
		pAction_Attack_BackHnd->Set_Options(TEXT("Attack_Swing_Right_90_BackHnd"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Select"), pRandom_Select)))
			throw TEXT("Failed Assemble_Behavior Random_Select");

		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_Attack_ForHnd"), pSequence_Attack_ForHnd, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_ForHnd");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Action_Attack_Overhead"), pAction_Attack_Overhead, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Overhead");

		if (FAILED(pSequence_Attack_ForHnd->Assemble_Behavior(TEXT("Action_Attack_BackHnd"), pAction_Attack_BackHnd)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_BackHnd");
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

HRESULT CArmored_Troll::Make_Attack_Right_135(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack_Right_135 */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Turn = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Turn)
			throw TEXT("pAction_Turn is nullptr");

		/* Set Decorations */

		/* Set Options */
		pAction_Turn->Set_Options(TEXT("Idle_Turn_Right_135"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Turn_Right_135"), pAction_Turn)))
			throw TEXT("Failed Assemble_Behavior Action_Turn_Right_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CArmored_Troll] Failed Make_Attack_Right_135 : \n");
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
		CAction* pAction_Attack_Overhead = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Attack_Overhead)
			throw TEXT("pSequence_Attack_Overhead is nullptr");

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

HRESULT CArmored_Troll::Make_Pattern_Attack_Far(_Inout_ CRandomChoose* pRandom_Select)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_Far */
	{
		if (nullptr == pRandom_Select)
			throw TEXT("Parameter pRandom_Select is nullptr");

		/* Make Child Behaviors */
		CSelector* pSelector_Attack_Charge = dynamic_cast<CSelector*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector")));
		if (nullptr == pSelector_Attack_Charge)
			throw TEXT("pSelector_Attack_Charge is nullptr");
		CSequence* pSequence_Attack_Run = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Run)
			throw TEXT("pSequence_Attack_Run is nullptr");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSelector_Attack_Charge, 0.6f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge");
		if (FAILED(pRandom_Select->Assemble_Behavior(TEXT("Sequence_Attack_Run"), pSequence_Attack_Run, 0.4f)))
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

HRESULT CArmored_Troll::Make_Pattern_Attack_Run(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Pattern_Attack_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Turn_Run = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turn_Run)
			throw TEXT("pSequence_Turn_Run is nullptr");
		CAction* pAction_Run_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Run_Loop)
			throw TEXT("pAction_Run_Loop is nullptr");
		CRandomChoose* pRandom_Attacks = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Attacks)
			TEXT("pRandom_Attacks is nullptr");

		CSequence_Attack* pSequence_Attack_Swing_BackHnd = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Swing_BackHnd)
			throw TEXT("pSequence_Attack_Swing_BackHnd is nullptr");
		CSequence_Attack* pSequence_Attack_Swing_ForHnd = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Swing_ForHnd)
			throw TEXT("pSequence_Attack_Swing_ForHnd is nullptr");
		CSequence_Attack* pSequence_Attack_Hop_ForHnd = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Hop_ForHnd)
			throw TEXT("pSequence_Attack_Hop_ForHnd is nullptr");
		CSequence_Attack* pSequence_Attack_Hop_BackHnd = dynamic_cast<CSequence_Attack*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence_Attack")));
		if (nullptr == pSequence_Attack_Hop_BackHnd)
			throw TEXT("pSequence_Attack_Hop_BackHnd is nullptr");

		CCheck_Distance* pTsk_Check_Distance = dynamic_cast<CCheck_Distance*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Distance")));
		if (nullptr == pTsk_Check_Distance)
			throw TEXT("pTsk_Check_Distance is nullptr");
		CLookAt* pTsk_LookAt = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt)
			throw TEXT("pTsk_LookAt is nullptr");

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

		/* Set Options */
		pAction_Run_Loop->Set_Options(TEXT("Run_Loop"), m_pModelCom, true, 0.f, false, false);
		pTsk_LookAt->Set_Transform(m_pTransform);
		pTsk_Check_Distance->Set_Option(7.f, true);
		pTsk_Check_Distance->Set_Transform(m_pTransform);
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
		CSequence* pSequence_Turns = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Turns)
			throw TEXT("pSequence_Turns is nullptr");
		CSequence* pSequence_Attack_Charge = dynamic_cast<CSequence*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sequence")));
		if (nullptr == pSequence_Attack_Charge)
			throw TEXT("pSequence_Attack_Charge is nullptr");

		CAction* pAction_Charge_Enter = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_Enter)
			throw TEXT("pAction_Charge_Enter is nullptr");
		CAction* pAction_Charge_Loop = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_Loop)
			throw TEXT("pAction_Charge_Loop is nullptr");
		CAction* pAction_Charge_End = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Charge_End)
			throw TEXT("pAction_Charge_End is nullptr");

		CLookAt* pTsk_LookAt_1 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_1)
			throw TEXT("pTsk_LookAt_1 is nullptr");
		CLookAt* pTsk_LookAt_2 = dynamic_cast<CLookAt*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_LookAt")));
		if (nullptr == pTsk_LookAt_2)
			throw TEXT("pTsk_LookAt_2 is nullptr");
		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");

		/* Set Decorations */

		/* Set Options */
		pTsk_LookAt_1->Set_Transform(m_pTransform);
		pTsk_LookAt_2->Set_Transform(m_pTransform);
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

HRESULT CArmored_Troll::Make_Taunt_Degree(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turn_Run */
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = dynamic_cast<CCheck_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Check_Degree")));
		if (nullptr == pTsk_Check_Degree)
			throw TEXT("pTsk_Check_Degree is nullptr");
		CSelector_Degree* pSelector_Degree = dynamic_cast<CSelector_Degree*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Selector_Degree")));
		if (nullptr == pSelector_Degree)
			throw TEXT("pSelector_Choose_Degree is nullptr");
		CRandomChoose* pRandom_Taunt_Left_Front = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Taunt_Left_Front)
			throw TEXT("pRandom_Taunt_Left_Front is nullptr");
		CRandomChoose* pRandom_Taunt_Right_Front = dynamic_cast<CRandomChoose*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_RandomChoose")));
		if (nullptr == pRandom_Taunt_Right_Front)
			throw TEXT("pRandom_Taunt_Right_Front is nullptr");
		CAction* pAction_Idle = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Idle)
			throw TEXT("pAction_Idle is nullptr");

		CAction* pAction_Left_Front_1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front_1)
			throw TEXT("pAction_Left_Front_1 is nullptr");
		CAction* pAction_Left_Front_2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front_2)
			throw TEXT("pAction_Left_Front_2 is nullptr");
		CAction* pAction_Left_Front_3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front_3)
			throw TEXT("pAction_Left_Front_3 is nullptr");
		CAction* pAction_Left_Front_4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front_4)
			throw TEXT("pAction_Left_Front_4 is nullptr");
		CAction* pAction_Left_Front_5 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_Front_5)
			throw TEXT("pAction_Left_Front_5 is nullptr");

		CAction* pAction_Right_Front_1 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front_1)
			throw TEXT("pAction_Right_Front_1 is nullptr");
		CAction* pAction_Right_Front_2 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front_2)
			throw TEXT("pAction_Right_Front_2 is nullptr");
		CAction* pAction_Right_Front_3 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front_3)
			throw TEXT("pAction_Right_Front_3 is nullptr");
		CAction* pAction_Right_Front_4 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front_4)
			throw TEXT("pAction_Right_Front_4 is nullptr");
		CAction* pAction_Right_Front_5 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_Front_5)
			throw TEXT("pAction_Right_Front_5 is nullptr");

		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_90)
			throw TEXT("pAction_Left_90 is nullptr");
		CAction* pAction_Right_90 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_90)
			throw TEXT("pAction_Right_90 is nullptr");
		CAction* pAction_Left_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_135)
			throw TEXT("pAction_Left_135 is nullptr");
		CAction* pAction_Right_135 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_135)
			throw TEXT("pAction_Right_135 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");

		CWait* pTsk_Wait = dynamic_cast<CWait*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Wait")));
		if (nullptr == pTsk_Wait)
			throw TEXT("pTsk_Wait is nullptr");
		/* Set Decorations */

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

		pTsk_Check_Degree->Set_Transform(m_pTransform);

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

HRESULT CArmored_Troll::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	return S_OK;
}

HRESULT CArmored_Troll::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Death_Ground = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Death_Ground)
			throw TEXT("pAction_Death_Ground is nullptr");

		CDeath* pTsk_Death_Ground = dynamic_cast<CDeath*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Death")));
		if (nullptr == pTsk_Death_Ground)
			throw TEXT("pTsk_Death_Ground is nullptr");

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
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Random_Idle_Move : \n");
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
}

void CArmored_Troll::Enter_Heavy_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 20;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pWeapon->On_Collider_Attack(&m_CollisionRequestDesc);
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
	m_pWeapon->Off_Collider_Attack(&m_CollisionRequestDesc);
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

	if (true == m_isCloned)
	{
		Safe_Release(m_pWeapon);
	}
}
