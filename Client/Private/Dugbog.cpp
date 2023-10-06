#include "Dugbog.h"

#include "Client_GameInstance_Functions.h"

#include "Wait.h"
#include "Turn.h"
#include "Death.h"
#include "LookAt.h"
#include "Action.h"
#include "RigidMove.h"
#include "MagicBall.h"
#include "Check_Degree.h"
#include "RandomChoose.h"
#include "Check_Distance.h"
#include "Selector_Degree.h"
#include "Sequence_Groggy.h"
#include "Sequence_Attack.h"
#include "UI_Group_Enemy_HP.h"
#include "Sequence_Descendo.h"
#include "Sequence_Levitate.h"
#include "Sequence_MoveTarget.h"

CDugbog::CDugbog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CDugbog::CDugbog(const CDugbog& rhs)
	: CEnemy(rhs)
{
}

HRESULT CDugbog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDugbog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDugbog::Initialize_Level(_uint iCurrentLevelIndex)
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

	return S_OK;
}

void CDugbog::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[rand() % 3];

	Set_Current_Target();

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	Tick_Spells();

	for (auto iter = m_CurrentTickSpells.begin(); iter != m_CurrentTickSpells.end(); )
	{
		if (iter->first & m_iCurrentSpell)
		{
			auto Desciter = m_MagicTickDesc.find(iter->first);
			if (Desciter != m_MagicTickDesc.end())
			{
				iter->second(Desciter->second);
			}
			
			++iter;
		}
		else
			iter = m_CurrentTickSpells.erase(iter);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CDugbog::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDugbog::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
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
		if (iter == m_CurrentTickSpells.end() &&
			BUFF_LEVIOSO & eBuff)
		{
			if (BUFF_LEVIOSO & eBuff && true == m_isAbleLevioso)
				eBuff = BUFF_LEVIOSO_TONGUE;

			m_CurrentTickSpells.emplace(eBuff, Action);
		}

		if (true == isCombo(eBuff))
			m_isHitCombo = true;

		if (eBuff & BUFF_ATTACK_LIGHT)
			m_isHitAttack = true;

		m_iCurrentSpell |= eBuff;
	}

	/* Collision Player Fig */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (false == IsEnemy(wstrObjectTag))
			return;

		m_isSpawn = true;
		m_RangeInEnemies.push_back({ wstrObjectTag, CollisionEventDesc.pOtherOwner });
	}
}

void CDugbog::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
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

HRESULT CDugbog::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CDugbog] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDugbog::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");
		if (FAILED(m_pRootBehavior->Add_Type("isAbleLevioso", m_isAbleLevioso)))
			throw TEXT("Failed Add_Type isAbleLevioso");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CRandomChoose* pRandom_Break = nullptr;
		if (FAILED(Create_Behavior(pRandom_Break)))
			throw TEXT("Failed Create_Behavior pRandom_Break");
		CAction* pAction_Spawn = nullptr;
		if (FAILED(Create_Behavior(pAction_Spawn)))
			throw TEXT("Failed Create_Behavior pAction_Spawn");
		CSequence* pSequence_Death = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death)))
			throw TEXT("Failed Create_Behavior pSequence_Death");
		CSelector* pSelector_Alive = nullptr;
		if (FAILED(Create_Behavior(pSelector_Alive)))
			throw TEXT("Failed Create_Behavior pSelector_Alive");

		/* Set Options */
		pAction_Spawn->Set_Options(TEXT("Hide_Up"), m_pModelCom, false, 0.f, true, true);

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Break"), pRandom_Break)))
			throw TEXT("Failed Assemble_Behavior Random_Break");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Spawn"), pAction_Spawn)))
			throw TEXT("Failed Assemble_Behavior Action_Spawn");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");

		if (FAILED(Make_Idle_Break(pRandom_Break)))
			throw TEXT("Failed Make_Idle_Break");
		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Alive(pSelector_Alive)))
			throw TEXT("Failed Make_Alive");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Notifies()
{
	function<void()> Func = [&] {(*this).Enter_Light_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enter_Heavy_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Heavy_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Exit_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Exit_Attack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Enable_Levioso(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enable_Levioso"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Disable_Levioso(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Disable_Levioso"), Func)))
		return E_FAIL;

	Func = [&] {(*this).On_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Gravity"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Off_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Gravity"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDugbog::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 200;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxSphereGeometry pSphereGeomatry1 = PxSphereGeometry(0.5f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry1;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_NPC_RANGE | COL_MAGIC | COL_STATIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		PxSphereGeometry pSphereGeomatry2 = PxSphereGeometry(15.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry2;
		RigidBodyDesc.eThisCollsion = COL_ENEMY_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");
		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		/* For.Collider Enemy_Attack_Body */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_SHIELD;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Attack_Body");
		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		/* For.Collider Enemy_Attack_Tongue */
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.5f, 1.5f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f);
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.6f, 1.2f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 1.f, 1.f);
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_SHIELD;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Attack_Tongue");
		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::MONSTER;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("1"));
		lstrcpy(Desc.wszObjectName, TEXT("대척추 더그보그"));

		BEGININSTANCE;
		m_pUI_HP = dynamic_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDugbog::Add_Components_Level(_uint iCurrentLevelIndex)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Dugbog"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Add_Components_Level : ");
		wstrErrorMSG += TEXT("Com_Model");
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDugbog::Bind_HitMatrices()
{
	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("headTop"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[0] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("Hip_left"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[1] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("tail_01"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[2] = pBone->Get_CombinedTransformationMatrixPtr();

	return S_OK;
}

void CDugbog::DeathBehavior(const _float& fTimeDelta)
{
	m_isDead = true;

	m_fDeadTimeAcc += fTimeDelta;
	if (3.f < m_fDeadTimeAcc)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CDugbog::Make_Idle_Break(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Hide_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hide_1)))
			throw TEXT("Failed Create_Behavior pAction_Hide_1");
		CAction* pAction_Hide_2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hide_2)))
			throw TEXT("Failed Create_Behavior pAction_Hide_2");
		CAction* pAction_Hide_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hide_3)))
			throw TEXT("Failed Create_Behavior pAction_Hide_3");
		CAction* pAction_Hide_4 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hide_4)))
			throw TEXT("Failed Create_Behavior pAction_Hide_4");

		/* Set Decoretors */
		pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return !(*pIsSpawn);
			});
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Hide_1->Set_Options(TEXT("Hide_1"), m_pModelCom);
		pAction_Hide_2->Set_Options(TEXT("Hide_2"), m_pModelCom);
		pAction_Hide_3->Set_Options(TEXT("Hide_3"), m_pModelCom);
		pAction_Hide_4->Set_Options(TEXT("Hide_4"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Hide_1"), pAction_Hide_1, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Hide_1");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Hide_2"), pAction_Hide_2, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Hide_2");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Hide_3"), pAction_Hide_3, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Hide_3");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Hide_4"), pAction_Hide_4, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Hide_4");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Idle_Break : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CSelector* pSelector_Choose = nullptr;
		if (FAILED(Create_Behavior(pSelector_Choose)))
			throw TEXT("Failed Create_Behavior pSelector_Choose");

		CAction* pAction_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Ground)))
			throw TEXT("Failed Create_Behavior pAction_Death_Ground");
		CSequence* pSequence_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death_Air)))
			throw TEXT("Failed Create_Behavior pSequence_Death_Air");
		CAction* pAction_Death_Tongue = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Tongue)))
			throw TEXT("Failed Create_Behavior pAction_Death_Tongue");
		CAction* pAction_Death_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Back)))
			throw TEXT("Failed Create_Behavior pAction_Death_Back");

		CAction* pAction_Knockback = nullptr;
		if (FAILED(Create_Behavior(pAction_Knockback)))
			throw TEXT("Failed Create_Behavior pAction_Knockback");
		CAction* pAction_Splat_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat_Back)))
			throw TEXT("Failed Create_Behavior pAction_Splat_Back");

		CDeath* pTsk_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Ground)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Ground");
		CDeath* pTsk_Death_Back = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Back)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Back");
		CDeath* pTsk_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Air)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Air");
		CDeath* pTsk_Death_Tongue = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Tongue)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Tongue");

		// Set Decorators
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});
		pAction_Death_Ground->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				if (BUFF_LEVIOSO & *piCurrentSpell ||
					BUFF_LEVIOSO_TONGUE & *piCurrentSpell)
					return false;

				return true;
			});
		pSequence_Death_Air->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				if (BUFF_LEVIOSO & *piCurrentSpell)
					return true;

				return false;
			});
		pAction_Death_Tongue->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				if (BUFF_LEVIOSO_TONGUE & *piCurrentSpell)
					return true;

				return false;
			});

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) { this->DeathBehavior(fTimeDelta); };
		pTsk_Death_Ground->Set_DeathFunction(Func);
		pTsk_Death_Back->Set_DeathFunction(Func);
		pTsk_Death_Air->Set_DeathFunction(Func);
		pTsk_Death_Tongue->Set_DeathFunction(Func);
		pAction_Death_Ground->Set_Options(TEXT("Death_Ground"), m_pModelCom);
		pAction_Death_Back->Set_Options(TEXT("Splat_Back"), m_pModelCom, true);
		pAction_Knockback->Set_Options(TEXT("Knockback"), m_pModelCom);
		pAction_Splat_Back->Set_Options(TEXT("Splat_Back"), m_pModelCom, true);
		pAction_Death_Tongue->Set_Options(TEXT("Death_Tongue_Levitate"), m_pModelCom);

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Choose"), pSelector_Choose)))
			throw TEXT("Failed Assemble_Behavior Selector_Choose");

		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Sequence_Death_Ground"), pAction_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Ground");
		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("pSequence_Death_Air"), pSequence_Death_Air)))
			throw TEXT("Failed Assemble_Behavior pSequence_Death_Air");
		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Action_Death_Tongue"), pAction_Death_Tongue)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Tongue");
		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("pAction_Death_Back"), pAction_Death_Back)))
			throw TEXT("Failed Assemble_Behavior pAction_Death_Back");

		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("pAction_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior pAction_Knockback");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("pAction_Splat_Back"), pAction_Splat_Back)))
			throw TEXT("Failed Assemble_Behavior pAction_Splat_Back");

		if (FAILED(pAction_Death_Back->Assemble_Behavior(TEXT("pTsk_Death_Back"), pTsk_Death_Back)))
			throw TEXT("Failed Assemble_Behavior pTsk_Death_Back");
		if (FAILED(pAction_Splat_Back->Assemble_Behavior(TEXT("pTsk_Death_Air"), pTsk_Death_Air)))
			throw TEXT("Failed Assemble_Behavior pTsk_Death_Air");
		if (FAILED(pAction_Death_Ground->Assemble_Behavior(TEXT("Tsk_Death_Ground"), pTsk_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Ground");
		if (FAILED(pAction_Death_Tongue->Assemble_Behavior(TEXT("Tsk_Death_Tongue"), pTsk_Death_Tongue)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Tongue");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Alive(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CSelector* pSelector_Hit_Combo = nullptr;
		if (FAILED(Create_Behavior(pSelector_Hit_Combo)))
			throw TEXT("Failed Create_Behavior pSelector_Hit_Combo");
		CSelector* pSelector_Check_Spell = nullptr;
		if (FAILED(Create_Behavior(pSelector_Check_Spell)))
			throw TEXT("Failed Create_Behavior pSelector_Check_Spell");
		CRandomChoose* pRandom_Attacks = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");

		CSelector* pSelector_Run_Attack = nullptr;
		if (FAILED(Create_Behavior(pSelector_Run_Attack)))
			throw TEXT("Failed Create_Behavior pSelector_Run_Attack");
		CSequence* pSequence_Tongue_Attack = nullptr;
		if (FAILED(Create_Behavior(pSequence_Tongue_Attack)))
			throw TEXT("Failed Create_Behavior pSequence_Tongue_Attack");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});
		pRandom_Attacks->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (true == IsDebuff(*pICurrentSpell))
					return false;

				return true;
			});
		pRandom_Attacks->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Hit_Combo"), pSelector_Hit_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Hit_Combo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Check_Spell"), pSelector_Check_Spell)))
			throw TEXT("Failed Assemble_Behavior Selector_Check_Spell");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Selector_Run_Attack"), pSelector_Run_Attack, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Selector_Run_Attack");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Tongue_Attack"), pSequence_Tongue_Attack, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Tongue_Attack");

		if (FAILED(Make_Hit_Combo(pSelector_Hit_Combo)))
			throw TEXT("Failed Make_Hit_Combo");
		if (FAILED(Make_Check_Spell(pSelector_Check_Spell)))
			throw TEXT("Failed Make_Check_Spell");
		if (FAILED(Make_Run_Attack(pSelector_Run_Attack)))
			throw TEXT("Failed Make_Run_Attack");
		if (FAILED(Make_Tongue_Attack(pSequence_Tongue_Attack)))
			throw TEXT("Failed Make_Run_Attack");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Alive : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Hit_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		if (FAILED(__super::Make_Hit_Combo(pSelector)))
			throw TEXT("Failed __super Make_Hit_Combo");

		/* Create Child Behavior */
		CSelector* pSelector_Fly_Combo = nullptr;
		if (FAILED(Create_Behavior(pSelector_Fly_Combo)))
			throw TEXT("Failed Create_Behavior pSelector_Fly_Combo");

		/* Set Decorator */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Fly_Combo"), pSelector_Fly_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Fly_Combo");

		if (FAILED(Make_Fly_Combo(pSelector_Fly_Combo)))
			throw TEXT("Failed Make_Fly_Combo");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Hit_Combo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence_Levitate* pSequence_Levitate = nullptr;
		if (FAILED(Create_Behavior(pSequence_Levitate)))
			throw TEXT("Failed Create_Behavior pSequence_Levitate");
		CSequence* pSequence_Levitate_Tongue = nullptr;
		if (FAILED(Create_Behavior(pSequence_Levitate_Tongue)))
			throw TEXT("Failed Create_Behavior pSequence_Levitate_Tongue");
		CSequence_Descendo* pSequence_Descendo = nullptr;
		if (FAILED(Create_Behavior(pSequence_Descendo)))
			throw TEXT("Failed Create_Behavior pSequence_Descendo");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_NONE == *pICurrentSpell ||
					true == *pIsHitCombo)
					return false;

				return true;
			});
		pSequence_Levitate_Tongue->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_LEVIOSO_TONGUE & *pICurrentSpell)
					return true;

				return false;
			});

		/* Set_Options */
		pSequence_Levitate->Set_Option(6.f, 1.2f);
		
		/* Levioso */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Levitate"), pSequence_Levitate)))
			throw TEXT("Failed Assemble_Behavior Sequence_Levitate");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Levitate_Tongue"), pSequence_Levitate_Tongue)))
			throw TEXT("Failed Assemble_Behavior Sequence_Levitate_Tongue");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop"), 1.f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(Make_Levioso_Tongue(pSequence_Levitate_Tongue)))
			throw TEXT("Failed Make_Levioso_Tongue");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Check_Spell : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

HRESULT CDugbog::Make_Run_Attack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Create Child Behaviors */
		CSequence* pSequence_Turns = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");
		CSequence* pSequence_Attack = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack)))
			throw TEXT("Failed Create_Behavior pSequence_Attack");

		CCheck_Distance* pCheck_Distance = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CAction* pAction_Run_Enter = nullptr;
		if (FAILED(Create_Behavior(pAction_Run_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Run_Enter");
		CSelector* pSelector_Run_Loop = nullptr;
		if (FAILED(Create_Behavior(pSelector_Run_Loop)))
			throw TEXT("Failed Create_Behavior pSelector_Run_Loop");
		CAction* pAction_Attack_Lunge_Bite = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack_Lunge_Bite)))
			throw TEXT("Failed Create_Behavior pAction_Attack_Lunge_Bite");

		CAction* pAction_Run_Loop_Far = nullptr;
		if (FAILED(Create_Behavior(pAction_Run_Loop_Far)))
			throw TEXT("Failed Create_Behavior pAction_Run_Loop_Far");
		CAction* pAction_Run_Loop_Near = nullptr;
		if (FAILED(Create_Behavior(pAction_Run_Loop_Near)))
			throw TEXT("Failed Create_Behavior pAction_Run_Loop_Near");

		CLookAt* pTsk_LookAt_Far = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_Far)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_Far");
		CLookAt* pTsk_LookAt_Near = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_Near)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_Near");
		CCheck_Distance* pCheck_Distance_Far = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance_Far)))
			throw TEXT("Failed Create_Behavior pCheck_Distance_Far");
		CCheck_Distance* pCheck_Distance_Near = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance_Near)))
			throw TEXT("Failed Create_Behavior pCheck_Distance_Near");

		/* Set Decorators */
		pAction_Run_Loop_Far->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
					return false;

				return 7.f <= fTargetDistance;
			});

		/* Set Options */
		pCheck_Distance->Set_Transform(m_pTransform);
		pTsk_LookAt_Far->Set_Transform(m_pTransform);
		pTsk_LookAt_Near->Set_Transform(m_pTransform);
		pCheck_Distance_Far->Set_Transform(m_pTransform);
		pCheck_Distance_Near->Set_Option(3.5f, true);
		pCheck_Distance_Near->Set_Transform(m_pTransform);

		pAction_Run_Enter->Set_Options(TEXT("Attack_Run_Enter"), m_pModelCom);
		pAction_Run_Loop_Far->Set_Options(TEXT("Attack_Run_Loop_Far"), m_pModelCom, false, 0.f, false, false);
		pAction_Run_Loop_Near->Set_Options(TEXT("Attack_Run_Loop_Near"), m_pModelCom, true, 0.f, false, false);
		pAction_Attack_Lunge_Bite->Set_Options(TEXT("Attack_Lunge_Bite"), m_pModelCom);

		/* Assmble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attack"), pSequence_Attack)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack");

		if (FAILED(pSequence_Attack->Assemble_Behavior(TEXT("pCheck_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior pCheck_Distance");
		if (FAILED(pSequence_Attack->Assemble_Behavior(TEXT("Action_Run_Enter"), pAction_Run_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Run_Enter");
		if (FAILED(pSequence_Attack->Assemble_Behavior(TEXT("Selector_Run_Loop"), pSelector_Run_Loop)))
			throw TEXT("Failed Assemble_Behavior Selector_Run_Loop");
		if (FAILED(pSequence_Attack->Assemble_Behavior(TEXT("Action_Attack_Lunge_Bite"), pAction_Attack_Lunge_Bite)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Lunge_Bite");

		if (FAILED(pSelector_Run_Loop->Assemble_Behavior(TEXT("Action_Run_Loop_Far"), pAction_Run_Loop_Far)))
			throw TEXT("Failed Assemble_Behavior Action_Run_Loop_Far");
		if (FAILED(pSelector_Run_Loop->Assemble_Behavior(TEXT("Action_Run_Loop_Near"), pAction_Run_Loop_Near)))
			throw TEXT("Failed Assemble_Behavior Action_Run_Loop_Near");

		if (FAILED(pAction_Run_Loop_Far->Assemble_Behavior(TEXT("Tsk_LookAt_Far"), pTsk_LookAt_Far)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_Far");
		if (FAILED(pAction_Run_Loop_Far->Assemble_Behavior(TEXT("Check_Distance_Far"), pCheck_Distance_Far)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Far");
		if (FAILED(pAction_Run_Loop_Near->Assemble_Behavior(TEXT("Tsk_LookAt_Near"), pTsk_LookAt_Near)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_Near");
		if (FAILED(pAction_Run_Loop_Near->Assemble_Behavior(TEXT("Check_Distance_Near"), pCheck_Distance_Near)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Near");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Run_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Tongue_Attack(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behavior */
		CAction* pAction_Stance = nullptr;
		if (FAILED(Create_Behavior(pAction_Stance)))
			throw TEXT("Failed Create_Behavior pAction_Stance");
		CCheck_Distance* pCheck_Distance = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CSelector* pSelector_Distance = nullptr;
		if (FAILED(Create_Behavior(pSelector_Distance)))
			throw TEXT("Failed Create_Behavior pSelector_Distance");
		CAction* pAction_Attack = nullptr;
		if (FAILED(Create_Behavior(pAction_Attack)))
			throw TEXT("Failed Create_Behavior pAction_Attack");

		CSequence* pSequence_Far = nullptr;
		if (FAILED(Create_Behavior(pSequence_Far)))
			throw TEXT("Failed Create_Behavior pSequence_Far");
		CSelector* pSelector_Near = nullptr;
		if (FAILED(Create_Behavior(pSelector_Near)))
			throw TEXT("Failed Create_Behavior pSelector_Near");

		CSequence* pSequence_Turn_Runs = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turn_Runs)))
			throw TEXT("Failed Create_Behavior pSequence_Turn_Runs");
		CAction* pAction_Run_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Run_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Run_Loop");
		CSequence* pSequence_Turns = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");
		CTurn* pTsk_Turn = nullptr;
		if (FAILED(Create_Behavior(pTsk_Turn)))
			throw TEXT("Failed Create_Behavior pTsk_Turn");

		CCheck_Distance* pCheck_Distance_Far = nullptr;
		if (FAILED(Create_Behavior(pCheck_Distance_Far)))
			throw TEXT("Failed Create_Behavior pCheck_Distance_Far");
		CLookAt* pTsk_LookAt = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt");
		CLookAt* pTsk_LookAt_Attack = nullptr;
		if (FAILED(Create_Behavior(pTsk_LookAt_Attack)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_Attack");

		/* Set Decorators */
		pSequence_Far->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fTargetDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fTargetDistance)))
					return true;

				return 5.f < fTargetDistance;
			});

		/* Set Options */
		pAction_Stance->Set_Options(TEXT("Idle_Stance_1"), m_pModelCom);
		pCheck_Distance->Set_Transform(m_pTransform);
		pAction_Run_Loop->Set_Options(TEXT("Run_Loop"), m_pModelCom, true, 0.f, false, false);
		pCheck_Distance_Far->Set_Option(5.f, true);
		pCheck_Distance_Far->Set_Transform(m_pTransform);
		pAction_Attack->Set_Options(TEXT("Attack_Tongue_Strike"), m_pModelCom);
		pTsk_Turn->Set_Option(45.f, 3.f);
		pTsk_Turn->Set_Transform(m_pTransform);
		pTsk_LookAt->Set_Option(1.5f);
		pTsk_LookAt->Set_Transform(m_pTransform);
		pTsk_LookAt_Attack->Set_Option(2.f);
		pTsk_LookAt_Attack->Set_Transform(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Stance"), pAction_Stance)))
			throw TEXT("Failed Assemble_Behavior pAction_Stance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Distance"), pSelector_Distance)))
			throw TEXT("Failed Assemble_Behavior Selector_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack"), pAction_Attack)))
			throw TEXT("Failed Assemble_Behavior Action_Attack");

		if (FAILED(pSelector_Distance->Assemble_Behavior(TEXT("Sequence_Far"), pSequence_Far)))
			throw TEXT("Failed Assemble_Behavior Sequence_Far");
		if (FAILED(pSelector_Distance->Assemble_Behavior(TEXT("Selector_Near"), pSelector_Near)))
			throw TEXT("Failed Assemble_Behavior Selector_Near");

		if (FAILED(pSequence_Far->Assemble_Behavior(TEXT("Sequence_Turn_Runs"), pSequence_Turn_Runs)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turn_Runs");
		if (FAILED(pSequence_Far->Assemble_Behavior(TEXT("Action_Run_Loop"), pAction_Run_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Run_Loop");
		if (FAILED(pSelector_Near->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector_Near->Assemble_Behavior(TEXT("Tsk_Turn"), pTsk_Turn)))
			throw TEXT("Failed Assemble_Behavior Tsk_Turn");

		if (FAILED(pAction_Run_Loop->Assemble_Behavior(TEXT("Check_Distance_Far"), pCheck_Distance_Far)))
			throw TEXT("Failed Assemble_Behavior Check_Distance_Far");
		if (FAILED(pAction_Run_Loop->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");
		if (FAILED(pAction_Attack->Assemble_Behavior(TEXT("Tsk_LookAt_Attack"), pTsk_LookAt_Attack)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_Attack");

		if (FAILED(Make_Turn_Runs(pSequence_Turn_Runs)))
			throw TEXT("Failed Make_Turn_Runs");
		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Tongue_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Turns(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Turns */
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
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Turn_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Turn_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Turn_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Turn_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);

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
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Turn_Runs(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
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
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");

		/* Set Options */
		pAction_Left_Front->Set_Options(TEXT("Run_Enter"), m_pModelCom);
		pAction_Right_Front->Set_Options(TEXT("Run_Enter"), m_pModelCom);
		pAction_Left_45->Set_Options(TEXT("Run_Enter"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Run_Enter"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Turn_Run_Start_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Turn_Run_Start_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Turn_Run_Start_Left_90"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Turn_Run_Start_Right_90"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Turn_Run_Start_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Turn_Run_Start_Right_180"), m_pModelCom);

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
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Fly_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Fly_Descendo = nullptr;
		if (FAILED(Create_Behavior(pSequence_Fly_Descendo)))
			throw TEXT("Failed Create_Behavior pSequence_Fly_Descendo");
		CSelector* pSelector_AirHit = nullptr;
		if (FAILED(Create_Behavior(pSelector_AirHit)))
			throw TEXT("Failed Create_Behavior pSelector_AirHit");

		/* Set Decorator */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				return isFlying(*piCurrentSpell);
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Fly_Descendo"), pSequence_Fly_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Fly_Descendo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_AirHit"), pSelector_AirHit)))
			throw TEXT("Failed Assemble_Behavior Selector_AirHit");

		if (FAILED(Make_Fly_Descendo(pSequence_Fly_Descendo)))
			throw TEXT("Failed Make_Fly_Descendo");
		if (FAILED(Make_Air_Hit(pSelector_AirHit)))
			throw TEXT("Failed Make_Air_Hit");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Levioso_Combo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Levioso_Tongue(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behavior */
		CAction* pAction_Levitate_Enter = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Enter");
		CAction* pAction_Levitate_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Loop");
		CAction* pAction_Levitate_Exit = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate_Exit)))
			throw TEXT("Failed Create_Behavior pAction_Levitate_Exit");

		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorator */
		pAction_Levitate_Exit->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_LEVIOSO_TONGUE & *pICurrentSpell)
					*pICurrentSpell ^= BUFF_LEVIOSO_TONGUE;

				return true;
			});

		/* Set Options */
		pAction_Levitate_Enter->Set_Options(TEXT("Levitate_Tongue_Enter"), m_pModelCom);
		pAction_Levitate_Loop->Set_Options(TEXT("Levitate_Tongue_Loop"), m_pModelCom, true, 0.f, false, false);
		pAction_Levitate_Exit->Set_Options(TEXT("Levitate_Tongue_End"), m_pModelCom);
		pTsk_Wait->Set_Timer(5.f);
		_float3 vForce = _float3(0.f, 6.f, 0.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Levitate_Enter"), pAction_Levitate_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Levitate_Loop"), pAction_Levitate_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Levitate_Exit"), pAction_Levitate_Exit)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate_Exit");

		if (FAILED(pAction_Levitate_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Levioso_Tongue : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Air_Hit(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Air_Light = nullptr;
		if (FAILED(Create_Behavior(pSequence_Air_Light)))
			throw TEXT("Failed Create_Behavior pSequence_Air_Light");
		CSequence* pSequence_Air_Heavy = nullptr;
		if (FAILED(Create_Behavior(pSequence_Air_Heavy)))
			throw TEXT("Failed Create_Behavior pSequence_Air_Heavy");

		/* 평타 맞고 공중에 뜨는 액션 */
		CRandomChoose* pRandom_AirHit = nullptr;
		if (FAILED(Create_Behavior(pRandom_AirHit)))
			throw TEXT("Failed Create_Behavior pRandom_AirHit");
		CAction* pAction_Splat_Front = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat_Front)))
			throw TEXT("Failed Create_Behavior pAction_Splat_Front");
		CAction* pAction_GetUp_Front = nullptr;
		if (FAILED(Create_Behavior(pAction_GetUp_Front)))
			throw TEXT("Failed Create_Behavior pAction_GetUp_Front");

		/* 막타 맞고 날라가는 액션 */
		CAction* pAction_Knockback = nullptr;
		if (FAILED(Create_Behavior(pAction_Knockback)))
			throw TEXT("Failed Create_Behavior pAction_Knockback");
		CAction* pAction_Splat_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat_Back)))
			throw TEXT("Failed Create_Behavior pAction_Splat_Back");
		CAction* pAction_OnBack_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_OnBack_Loop)))
			throw TEXT("Failed Create_Behavior pAction_OnBack_Loop");
		CAction* pAction_GetUp_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_GetUp_Back)))
			throw TEXT("Failed Create_Behavior pAction_GetUp_Back");

		CAction* pAction_Hit_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_1)))
			throw TEXT("Failed Create_Behavior pAction_Hit_1");
		CAction* pAction_Hit_2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_2)))
			throw TEXT("Failed Create_Behavior pAction_Hit_2");
		CAction* pAction_Hit_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_3)))
			throw TEXT("Failed Create_Behavior pAction_Hit_3");

		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorator*/
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_ATTACK_LIGHT & *pICurrentSpell || BUFF_ATTACK_HEAVY & *pICurrentSpell;
			});
		pSequence_Air_Light->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ATTACK_HEAVY & *pICurrentSpell)
					return false;
				if (BUFF_ATTACK_LIGHT & *pICurrentSpell)
					return true;

				return false;
			});
		pAction_GetUp_Front->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				*pICurrentSpell = BUFF_NONE;

				*pIsHitCombo = false;

				return true;
			});
		pAction_GetUp_Back->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitCombo = { nullptr };
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitCombo", pIsHitCombo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				*pICurrentSpell = BUFF_NONE;

				*pIsHitCombo = false;

				return true;
			});
		pRandom_AirHit->Add_Change_Condition(CBehavior::BEHAVIOR_RUNNING, [&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsHitAttack = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isHitAttack", pIsHitAttack)))
					return false;

				if (true == *pIsHitAttack)
				{
					*pIsHitAttack = false;
					return true;
				}

				return false;
			});

		/* Set Options */
		pAction_Splat_Front->Set_Options(TEXT("Splat_Front"), m_pModelCom);
		pAction_GetUp_Front->Set_Options(TEXT("Getup_Front"), m_pModelCom);
		pAction_Knockback->Set_Options(TEXT("Knockback"), m_pModelCom);
		pAction_Splat_Back->Set_Options(TEXT("Knockback_Splat"), m_pModelCom);
		pAction_OnBack_Loop->Set_Options(TEXT("On_Back_Loop"), m_pModelCom, true, 0.f, false, false);
		pAction_GetUp_Back->Set_Options(TEXT("Getup_Back"), m_pModelCom);

		pAction_Hit_1->Set_Options(TEXT("Air_Hit_Front_1"), m_pModelCom);
		pAction_Hit_2->Set_Options(TEXT("Air_Hit_Front_2"), m_pModelCom);
		pAction_Hit_3->Set_Options(TEXT("Air_Hit_Front_3"), m_pModelCom);

		pTsk_Wait->Set_Timer(3.f);

		/* Assemble Childs */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Air_Light"), pSequence_Air_Light)))
			throw TEXT("Failed Assemble_Behavior Sequence_Air_Light");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Air_Heavy"), pSequence_Air_Heavy)))
			throw TEXT("Failed Assemble_Behavior Sequence_Air_Heavy");

		if (FAILED(pSequence_Air_Light->Assemble_Behavior(TEXT("Random_AirHit"), pRandom_AirHit)))
			throw TEXT("Failed Assemble_Behavior Random_AirHit");
		if (FAILED(pSequence_Air_Light->Assemble_Behavior(TEXT("Action_Splat_Front"), pAction_Splat_Front)))
			throw TEXT("Failed Assemble_Behavior Action_Splat_Front");
		if (FAILED(pSequence_Air_Light->Assemble_Behavior(TEXT("Action_GetUp_Front"), pAction_GetUp_Front)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp_Front");

		if (FAILED(pSequence_Air_Heavy->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");
		if (FAILED(pSequence_Air_Heavy->Assemble_Behavior(TEXT("Action_Splat_Back"), pAction_Splat_Back)))
			throw TEXT("Failed Assemble_Behavior Action_Splat_Back");
		if (FAILED(pSequence_Air_Heavy->Assemble_Behavior(TEXT("Action_OnBack_Loop"), pAction_OnBack_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_OnBack_Loop");
		if (FAILED(pSequence_Air_Heavy->Assemble_Behavior(TEXT("Action_GetUp_Back"), pAction_GetUp_Back)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp_Back");

		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_1"), pAction_Hit_1, 0.333f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_1");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_2"), pAction_Hit_2, 0.333f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_2");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_3"), pAction_Hit_3, 0.334f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_3");

		if (FAILED(pAction_OnBack_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Air_Hit : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDugbog::Make_Fly_Descendo(CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		if(FAILED(__super::Make_Fly_Descendo(pSequence)))
			throw TEXT("Failed __super Make_Fly_Descendo");

		/* Create Child Behaviors */
		CAction* pAction_Descendo1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo1)))
			throw TEXT("Failed Create_Behavior pAction_Descendo1");
		CAction* pAction_Descendo2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo2)))
			throw TEXT("Failed Create_Behavior pAction_Descendo2");
		CAction* pAction_Descendo3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo3)))
			throw TEXT("Failed Create_Behavior pAction_Descendo3");
		CAction* pAction_GetUp = nullptr;
		if (FAILED(Create_Behavior(pAction_GetUp)))
			throw TEXT("Failed Create_Behavior pAction_GetUp");

		CRigidMove* pTsk_RigidMove = nullptr;
		if (FAILED(Create_Behavior(pTsk_RigidMove)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove");
		CRigidMove* pTsk_RigidMove_Up = nullptr;
		if (FAILED(Create_Behavior(pTsk_RigidMove_Up)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove_Up");

		/* Set Decorators */

		/* Set Options */
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom, true);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Splat_Front"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Getup_Front"), m_pModelCom);

		_float3 vForce = _float3(0.f, -100.f, 0.f);
		pTsk_RigidMove->Set_Option(m_pRigidBody, vForce, 0.3f);
		vForce = _float3(0.f, 3.f, 0.f);
		pTsk_RigidMove_Up->Set_Option(m_pRigidBody, vForce, 2.6f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo2");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo3");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");

		if (FAILED(pAction_Descendo1->Assemble_Behavior(TEXT("Tsk_RigidMove"), pTsk_RigidMove)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove");
		if (FAILED(pAction_Descendo2->Assemble_Behavior(TEXT("Tsk_RigidMove_Up"), pTsk_RigidMove_Up)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Up");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Make_Fly_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CDugbog::Enter_Light_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 5;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pRigidBody->Enable_Collision("Enemy_Attack_Body", this, &m_CollisionRequestDesc);
}

void CDugbog::Enter_Heavy_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 10;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pRigidBody->Enable_Collision("Enemy_Attack_Tongue", this, &m_CollisionRequestDesc);
}

void CDugbog::Exit_Attack()
{
	m_CollisionRequestDesc.eType = ATTACK_NONE;
	m_CollisionRequestDesc.iDamage = 0;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pRigidBody->Disable_Collision("Enemy_Attack_Body");
	m_pRigidBody->Disable_Collision("Enemy_Attack_Tongue");
}

CDugbog* CDugbog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDugbog* pInstance = New CDugbog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDugbog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDugbog::Clone(void* pArg)
{
	CDugbog* pInstance = New CDugbog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDugbog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDugbog::Free()
{
	__super::Free();
}
