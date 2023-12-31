#include "Golem_Combat.h"

#include "Client_GameInstance_Functions.h"

#include "Weapon_Golem_Combat.h"

#include "Camera_Shake.h"

#include "Wait.h"
#include "Death.h"
#include "Action.h"
#include "MagicBall.h"
#include "RigidMove.h"
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

#include "UI_Damage.h"

CGolem_Combat::CGolem_Combat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CGolem_Combat::CGolem_Combat(const CGolem_Combat& rhs)
	: CEnemy(rhs)
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

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Components_for_Shake()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGolem_Combat::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pModelCom->Change_Animation(TEXT("Spawn_Fall_Loop"));

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(Make_Notifies_for_Shake()))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	m_pTarget = m_pPlayer;

	m_isSpawn = true;

	return S_OK;
}

void CGolem_Combat::Tick(_float fTimeDelta)
{
	if (false == m_isSpawn)
		return;

	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[rand() % 3];

	if (true == m_isParring ||
		true == IsDebuff(BUFF_TYPE(m_iCurrentSpell)))
	{
		Exit_Attack();
	}

	Tick_Spells();

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);

	//m_pTransform->Set_Position(_float3(48.f, 0.6f, 42.f));
}

void CGolem_Combat::Late_Tick(_float fTimeDelta)
{
	if (false == m_isSpawn)
		return;

	__super::Late_Tick(fTimeDelta);
}

void CGolem_Combat::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;
	wstring wstrOtherCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		Print_Damage_Font(iDamage);

		m_pHealth->Damaged(iDamage);

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() &&
			BUFF_LEVIOSO & eBuff)
		{
			m_CurrentTickSpells.emplace(eBuff, Action);
		}

		if (true == isCombo(eBuff))
			m_isHitCombo = true;

		if (eBuff & BUFF_ATTACK_LIGHT)
			m_isHitAttack = true;

		m_iCurrentSpell |= eBuff;
	}

	/* Collision Protego */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Attack")) &&
		wstring::npos != wstrOtherCollisionTag.find(TEXT("Protego")))
	{
		m_isParring = true;
	}
}

void CGolem_Combat::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

}

HRESULT CGolem_Combat::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CGolem_Combat] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::Add_Components_for_Shake()
{
	try
	{
		CCamera_Shake::CAMERA_SHAKE_DESC Camera_Shake_Desc = { CCamera_Shake::CAMERA_SHAKE_DESC() };

		_float fMaxDistance = { 15.0f };
		_float fMinDistance = { 2.0f };


		Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
		Camera_Shake_Desc.isDistanceOption = true;
		Camera_Shake_Desc.pTransform = m_pTransform;
		Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::INOUT_EXPO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 1.0f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.1f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 15.0f;
		Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			TEXT("Com_Step_Shake"), reinterpret_cast<CComponent**>(&m_pDescendo_Shake), &Camera_Shake_Desc)))
			throw TEXT("Com_Step_Shake");

		m_pDescendo_Shake->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);
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

HRESULT CGolem_Combat::Make_Notifies_for_Shake()
{
	if (nullptr == m_pDescendo_Shake)
	{
		MSG_BOX("Failed Make_Notifies_for_Shake");
		return E_FAIL;
	}

	function<void()> func = [&] {m_pDescendo_Shake->RandomUpAxisShake(); };

	m_pModelCom->Bind_Notifies(TEXT("Camera_Shake_Descendo"), func);

	return S_OK;
}

HRESULT CGolem_Combat::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CAction* pAction_Spawn = nullptr;
		if (FAILED(Create_Behavior(pAction_Spawn)))
			throw TEXT("Failed Create_Behavior pAction_Spawn");
		CSequence* pSequence_Death = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death)))
			throw TEXT("Failed Create_Behavior pSequence_Death");
		CSelector* pSelector_Alive = nullptr;
		if (FAILED(Create_Behavior(pSelector_Alive)))
			throw TEXT("Failed Create_Behavior pSelector_Alive");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");

		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Alive(pSelector_Alive)))
			throw TEXT("FAiled Make_Alive");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Notifies()
{
	function<void()> Func = [&] { this->Enter_Light_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Light_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->Enter_Body_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Body_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->Exit_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Exit_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->On_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Gravity"), Func)))
		return E_FAIL;

	Func = [&] { this->Off_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Gravity"), Func)))
		return E_FAIL;

	Func = [&] { this->Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] {(*this).StopAllSound(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("StopAllSound"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGolem_Combat::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 350;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");
		
		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.2f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.6f, 1.5f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_NPC_RANGE | COL_MAGIC | COL_STATIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Attack */
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 2.2f, 1.f);
		PxSphereGeometry pSphereGeomatry2 = PxSphereGeometry(1.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry2;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Attack");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_SHIELD;

		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::MONSTER;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("23"));
		lstrcpy(Desc.wszObjectName, TEXT("가디언"));

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		if (nullptr == m_pUI_HP)
		{
			ENDINSTANCE;
			throw TEXT("m_pUI_HP is nullptr");
		}

		ENDINSTANCE;
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGolem_Combat::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CSelector* pSelector_Choose = nullptr;
		if (FAILED(Create_Behavior(pSelector_Choose)))
			throw TEXT("Failed Create_Behavior pSelector_Choose");

		CSequence* pSequence_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death_Ground)))
			throw TEXT("Failed Create_Behavior pSequence_Death_Ground");
		CSequence* pSequence_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death_Air)))
			throw TEXT("Failed Create_Behavior pSequence_Death_Air");

		CAction* pAction_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Ground)))
			throw TEXT("Failed Create_Behavior pAction_Death_Ground");
		CAction* pAction_Death_Ground_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Ground_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Death_Ground_Loop");
		CAction* pAction_Knockback = nullptr;
		if (FAILED(Create_Behavior(pAction_Knockback)))
			throw TEXT("Failed Create_Behavior pAction_Knockback");
		CAction* pAction_Splat = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat)))
			throw TEXT("Failed Create_Behavior pAction_Splat");
		CAction* pAction_Splat_Loop = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Splat_Loop");

		CDeath* pTsk_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Ground)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Ground");
		CDeath* pTsk_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Air)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Air");

		// Set Decorators
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});
		pSequence_Death_Ground->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				if (true == isFlying(*piCurrentSpell))
					return false;

				return true;
			});
		pSequence_Death_Air->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* piCurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", piCurrentSpell)))
					return false;

				if (true == isFlying(*piCurrentSpell))
				{
					CRigidBody* pRigidBody = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
						return false;

					pRigidBody->Set_Gravity(true);

					return true;
				}

				return false;
			});
		pAction_Death_Ground->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				Set_Dead();
				return true;
			});
		pAction_Knockback->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				Set_Dead();
				return true;
			});

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) { this->DeathBehavior(fTimeDelta); };
		pTsk_Death_Ground->Set_DeathFunction(Func);
		pTsk_Death_Air->Set_DeathFunction(Func);
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Knockback_Back_Splat"), m_pModelCom, true);
		pAction_Splat_Loop->Set_Options(TEXT("Send_Splat_Loop"), m_pModelCom, true);
		pAction_Death_Ground->Set_Options(TEXT("Death"), m_pModelCom);
		pAction_Death_Ground_Loop->Set_Options(TEXT("Send_Splat_Loop"), m_pModelCom, true);

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Choose"), pSelector_Choose)))
			throw TEXT("Failed Assemble_Behavior Selector_Choose");

		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Sequence_Death_Ground"), pSequence_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Ground");
		if (FAILED(pSelector_Choose->Assemble_Behavior(TEXT("Sequence_Death_Air"), pSequence_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Air");

		if (FAILED(pSequence_Death_Ground->Assemble_Behavior(TEXT("Action_Death_Ground"), pAction_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Ground");
		if (FAILED(pSequence_Death_Ground->Assemble_Behavior(TEXT("Action_Death_Ground_Loop"), pAction_Death_Ground_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Ground_Loop");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Splat"), pAction_Splat)))
			throw TEXT("Failed Assemble_Behavior Action_Splat");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Splat_Loop"), pAction_Splat_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Splat_Loop");

		if (FAILED(pAction_Death_Ground_Loop->Assemble_Behavior(TEXT("Tsk_Death_Ground"), pTsk_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Ground");
		if (FAILED(pAction_Splat_Loop->Assemble_Behavior(TEXT("Tsk_Death_Air"), pTsk_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Air");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Alive(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behavior */
		CSelector* pSelector_Hit_Combo = nullptr;
		if (FAILED(Create_Behavior(pSelector_Hit_Combo)))
			throw TEXT("Failed Create_Behavior pSelector_Hit_Combo");
		CSelector* pSelector_CheckSpell = nullptr;
		if (FAILED(Create_Behavior(pSelector_CheckSpell)))
			throw TEXT("Failed Create_Behavior pSelector_CheckSpell");
		CSelector* pSelector_NormalAttack = nullptr;
		if (FAILED(Create_Behavior(pSelector_NormalAttack)))
			throw TEXT("Failed Create_Behavior pSelector_NormalAttack");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Hit_Combo"), pSelector_Hit_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Hit_Combo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_CheckSpell"), pSelector_CheckSpell)))
			throw TEXT("Failed Assemble_Behavior Selector_CheckSpell");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_NormalAttack"), pSelector_NormalAttack)))
			throw TEXT("Failed Assemble_Behavior Selector_NormalAttack");

		if (FAILED(Make_Hit_Combo(pSelector_Hit_Combo)))
			throw TEXT("Failed Make_Hit_Combo");
		if (FAILED(Make_Check_Spell(pSelector_CheckSpell)))
			throw TEXT("Failed Make_Check_Spell");
		if (FAILED(Make_NormalAttack(pSelector_NormalAttack)))
			throw TEXT("Failed Make_NormalAttack");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Hit_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		if (FAILED(__super::Make_Hit_Combo(pSelector)))
			throw TEXT("Failed __super Make_Hit_Combo");

		/* Make Child Behaviors */
		CSelector* pSelector_Fly_Combo = nullptr;
		if (FAILED(Create_Behavior(pSelector_Fly_Combo)))
			throw TEXT("Failed Create_Behavior pSelector_Fly_Combo");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Fly_Combo"), pSelector_Fly_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Fly_Combo");

		if (FAILED(Make_Fly_Combo(pSelector_Fly_Combo)))
			throw TEXT("Failed Make_Levioso_Combo");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Hit : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		if (FAILED(__super::Make_Check_Spell(pSelector)))
			throw TEXT("Failed __super Make_Check_Spell");

		/* Make Child Behaviors */
		CSequence_Groggy* pSequence_Groggy = nullptr;
		if (FAILED(Create_Behavior(pSequence_Groggy)))
			throw TEXT("Failed Create_Behavior pSequence_Groggy");
		CSequence_Levitate* pSequence_Levitate = nullptr;
		if (FAILED(Create_Behavior(pSequence_Levitate)))
			throw TEXT("Failed Create_Behavior pSequence_Levitate");
		CSequence_Descendo* pSequence_Descendo = nullptr;
		if (FAILED(Create_Behavior(pSequence_Descendo)))
			throw TEXT("Failed Create_Behavior pSequence_Descendo");

		/* Set Decorations */

		/* Set_Options */
		pSequence_Groggy->Set_LoopTime(3.f);
		pSequence_Levitate->Set_Option(3.f, 1.6f);

		/* Stupefy */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Groggy"), pSequence_Groggy)))
			throw TEXT("Failed Assemble_Behavior Sequence_Groggy");
		/* Levioso */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Levitate"), pSequence_Levitate)))
			throw TEXT("Failed Assemble_Behavior Sequence_Levitate");
		/* Descendo */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Descendo"), pSequence_Descendo)))
			throw TEXT("Failed Assemble_Behavior Sequence_Descendo");

		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop_1"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop_2"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop_3"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop_4"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
		if (FAILED(pSequence_Levitate->Assemble_Random_Select_Behavior(TEXT("Levitate_Loop_5"), 0.2f, 4.f)))
			throw TEXT("Failed Assemble_Random_Select_Behavior");
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

HRESULT CGolem_Combat::Make_NormalAttack(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CSequence* pSequence_Turns = nullptr;
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");
		CSequence_MoveTarget* pSequence_MoveTarget = nullptr;
		if (FAILED(Create_Behavior(pSequence_MoveTarget)))
			throw TEXT("Failed Create_Behavior pSequence_MoveTarget");
		CSequence_MoveTarget* pSequence_MoveBackTarget = nullptr;
		if (FAILED(Create_Behavior(pSequence_MoveBackTarget)))
			throw TEXT("Failed Create_Behavior pSequence_MoveBackTarget");
		CSequence* pSequence_Attacks = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attacks)))
			throw TEXT("Failed Create_Behavior pSequence_Attacks");
		CRandomChoose* pRandom_IdleMove = nullptr;
		if (FAILED(Create_Behavior(pRandom_IdleMove)))
			throw TEXT("Failed Create_Behavior pRandom_IdleMove");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (true == IsDebuff((BUFF_TYPE)*pICurrentSpell))
					return false;

				if (BUFF_STUPEFY & *pICurrentSpell)
					return false;

				return true;
			});
		pSequence_MoveTarget->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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
		pSequence_MoveBackTarget->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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

		/* Set Options */
		pSequence_Attacks->Set_Option(2.f);
		pSequence_MoveTarget->Set_Action_Options(TEXT("Move_Front"), m_pModelCom);
		pSequence_MoveBackTarget->Set_Action_Options(TEXT("Move_Back"), m_pModelCom);

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveTarget"), pSequence_MoveTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_MoveBackTarget"), pSequence_MoveBackTarget)))
			throw TEXT("Failed Assemble_Behavior Sequence_MoveBackTarget");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attacks"), pSequence_Attacks)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attacks");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Idle_Move"), pRandom_IdleMove)))
			throw TEXT("Failed Assemble_Behavior Random_Idle_Move");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");
		if (FAILED(Make_Attack(pSequence_Attacks)))
			throw TEXT("Failed Make_Attack");
		if (FAILED(Make_Random_Idle_Move(pRandom_IdleMove)))
			throw TEXT("Failed Make_Random_Idle_Move");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_NormalAttack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Fly_Combo(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try /* Failed Check Make_Levioso_Combo */
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CSequence* pSequence_Fly_Descendo = nullptr;
		if (FAILED(Create_Behavior(pSequence_Fly_Descendo)))
			throw TEXT("Failed Create_Behavior pSequence_Fly_Descendo");
		CSequence* pSequence_AirHit = nullptr;
		if (FAILED(Create_Behavior(pSequence_AirHit)))
			throw TEXT("Failed Create_Behavior pSequence_AirHit");

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
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_AirHit"), pSequence_AirHit)))
			throw TEXT("Failed Assemble_Behavior Sequence_AirHit");

		if (FAILED(Make_Fly_Descendo(pSequence_Fly_Descendo)))
			throw TEXT("Failed Make_Fly_Descendo");
		if (FAILED(Make_Air_Hit(pSequence_AirHit)))
			throw TEXT("Failed Make_Air_Hit");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Levioso_Combo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

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
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CCheck_Degree* pTsk_Check_Degree = nullptr;
		if (FAILED(Create_Behavior(pTsk_Check_Degree)))
			throw TEXT("Failed Create_Behavior pTsk_Check_Degree");
		CSelector_Degree* pSelector_Degree = nullptr;
		if (FAILED(Create_Behavior(pSelector_Degree)))
			throw TEXT("Failed Create_Behavior pSelector_Degree");

		CAction* pAction_Left90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left90)))
			throw TEXT("Failed Create_Behavior pAction_Left90");
		CAction* pAction_Right90 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right90)))
			throw TEXT("Failed Create_Behavior pAction_Right90");
		CAction* pAction_Left135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left135)))
			throw TEXT("Failed Create_Behavior pAction_Left135");
		CAction* pAction_Right135 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right135)))
			throw TEXT("Failed Create_Behavior pAction_Right135");
		CAction* pAction_Right_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_Back)))
			throw TEXT("Failed Create_Behavior pAction_Right_Back");
		CAction* pAction_Left_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_Back)))
			throw TEXT("Failed Create_Behavior pAction_Left_Back");
		/* Set Decorations */

		/* Set Options */
		pAction_Right_Back->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);
		pAction_Left_Back->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Left90->Set_Options(TEXT("Turn_Left_90"), m_pModelCom);
		pAction_Right90->Set_Options(TEXT("Turn_Right_90"), m_pModelCom);
		pAction_Left135->Set_Options(TEXT("Turn_Left_180"), m_pModelCom);
		pAction_Right135->Set_Options(TEXT("Turn_Right_180"), m_pModelCom);

		pTsk_Check_Degree->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_Check_Degree"), pTsk_Check_Degree)))
			throw TEXT("Failed Assemble_Behavior Tsk_Check_Degree");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Degree"), pSelector_Degree)))
			throw TEXT("Failed Assemble_Behavior Selector_Degree");

		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_Back)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_Back)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_90, pAction_Left90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_135, pAction_Left135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_135");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_90, pAction_Right90)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_90");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_135, pAction_Right135)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_135");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Attack(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try /* Failed Check Make_Attack */
	{
		/* Make Child Behaviors */
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CCheck_Distance* pCheck_Distance = { nullptr };
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CSelector* pSelector_Attack = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Attack)))
			throw TEXT("Failed Create_Behavior pSelector_Attack");

		CRandomChoose* pRandom_Attack = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attack)))
			throw TEXT("Failed Create_Behavior pRandom_Attack");
		CAction* pAction_Protego_Deflect = nullptr;
		if (FAILED(Create_Behavior(pAction_Protego_Deflect)))
			throw TEXT("Failed Create_Behavior pAction_Protego_Deflect");

		CSequence_Attack* pSequence_Attack_Slash = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Slash)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Slash");
		CSequence_Attack* pSequence_Attack_Shoulder = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Shoulder)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Shoulder");
		CSequence_Attack* pSequence_Attack_OverHand = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_OverHand)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_OverHand");
		CSequence_Attack* pSequence_Attack_Jab = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_Jab)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Jab");

		/* Set Decorations */
		pSelector_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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
		pSelector_Attack->Add_Fail_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				if (FAILED(pBlackBoard->Set_Type("fAttackRange", 5.f)))
					return false;

				return true;
			});
		pRandom_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				return !*pIsParring;
			});
		pRandom_Attack->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});
		pAction_Protego_Deflect->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				return *pIsParring;
			});
		pAction_Protego_Deflect->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsParring = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isParring", pIsParring)))
					return false;

				*pIsParring = false;

				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);
		pSequence_Attack_Slash->Set_Attack_Action_Options(TEXT("Attack_Slash_Sword"), m_pModelCom);
		pSequence_Attack_Slash->Set_Attack_Option(7.f);
		pSequence_Attack_Shoulder->Set_Attack_Action_Options(TEXT("Attack_Shoulder"), m_pModelCom);
		pSequence_Attack_Shoulder->Set_Attack_Option(4.5f);
		pSequence_Attack_OverHand->Set_Attack_Action_Options(TEXT("Attack_OverHand_Sword"), m_pModelCom);
		pSequence_Attack_OverHand->Set_Attack_Option(7.5f);
		pSequence_Attack_Jab->Set_Attack_Action_Options(TEXT("Attack_Jab"), m_pModelCom);
		pSequence_Attack_Jab->Set_Attack_Option(4.5f);

		pAction_Protego_Deflect->Set_Options(TEXT("Protego_Deflect"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_Attack"), pSelector_Attack)))
			throw TEXT("Failed Assemble_Behavior Selector_Attack");

		if (FAILED(pSelector_Attack->Assemble_Behavior(TEXT("RandomChoose"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior RandomChoose");
		if (FAILED(pSelector_Attack->Assemble_Behavior(TEXT("Action_Protego_Deflect"), pAction_Protego_Deflect)))
			throw TEXT("Failed Assemble_Behavior Action_Protego_Deflect");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Slash"), pSequence_Attack_Slash, 0.35f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Slash");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Shoulder"), pSequence_Attack_Shoulder, 0.01f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Shoulder");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_OverHand"), pSequence_Attack_OverHand, 0.32f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_OverHand");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Attack_Jab"), pSequence_Attack_Jab, 0.32f)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_Jab");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Attack : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Random_Idle_Move(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Idle = nullptr;
		if (FAILED(Create_Behavior(pAction_Idle)))
			throw TEXT("Failed Create_Behavior pAction_Idle");
		CSequence_MoveTarget* pSequence_Move_1StepLeft = nullptr;
		if (FAILED(Create_Behavior(pSequence_Move_1StepLeft)))
			throw TEXT("Failed Create_Behavior pSequence_Move_1StepLeft");
		CSequence_MoveTarget* pSequence_Move_1StepRight = nullptr;
		if (FAILED(Create_Behavior(pSequence_Move_1StepRight)))
			throw TEXT("Failed Create_Behavior pSequence_Move_1StepRight");

		CWait* pTsk_Wait = nullptr;
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorators */
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Idle->Set_Options(TEXT("Idle"), m_pModelCom, true);
		pSequence_Move_1StepLeft->Set_Action_Options(TEXT("1Step_Left"), m_pModelCom);
		pSequence_Move_1StepRight->Set_Action_Options(TEXT("1Step_Right"), m_pModelCom);
		pTsk_Wait->Set_Timer(1.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Idle"), pAction_Idle, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Idle");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Move_1StepLeft"), pSequence_Move_1StepLeft, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepLeft");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Move_1StepRight"), pSequence_Move_1StepRight, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move_1StepRight");

		if (FAILED(pAction_Idle->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
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

HRESULT CGolem_Combat::Make_Air_Hit(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */
		CSelector* pSelector_AirHit = nullptr;
		if (FAILED(Create_Behavior(pSelector_AirHit)))
			throw TEXT("Failed Create_Behavior pSelector_AirHit");
		CAction* pAction_Splat = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat)))
			throw TEXT("Failed Create_Behavior pAction_Splat");
		CAction* pAction_GetUp = nullptr;
		if (FAILED(Create_Behavior(pAction_GetUp)))
			throw TEXT("Failed Create_Behavior pAction_GetUp");

		/* 막타 맞고 날라가는 액션 */
		CAction* pAction_Knockback = nullptr;
		if (FAILED(Create_Behavior(pAction_Knockback)))
			throw TEXT("Failed Create_Behavior pAction_Knockback");
		/* 평타 맞고 공중에 뜨는 액션 */
		CRandomChoose* pRandom_AirHit = nullptr;
		if (FAILED(Create_Behavior(pRandom_AirHit)))
			throw TEXT("Failed Create_Behavior pRandom_AirHit");

		CAction* pAction_Hit_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_1)))
			throw TEXT("Failed Create_Behavior pAction_Hit_1");
		CAction* pAction_Hit_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_3)))
			throw TEXT("Failed Create_Behavior pAction_Hit_3");
		CAction* pAction_Hit_4 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_4)))
			throw TEXT("Failed Create_Behavior pAction_Hit_4");
		CAction* pAction_Hit_5 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_5)))
			throw TEXT("Failed Create_Behavior pAction_Hit_5");
		CAction* pAction_Hit_6 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_6)))
			throw TEXT("Failed Create_Behavior pAction_Hit_6");
		CAction* pAction_Hit_8 = nullptr;
		if (FAILED(Create_Behavior(pAction_Hit_8)))
			throw TEXT("Failed Create_Behavior pAction_Hit_8");

		/* Set Decorator*/
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_ATTACK_LIGHT & *pICurrentSpell || BUFF_ATTACK_HEAVY & *pICurrentSpell;
			});
		pRandom_AirHit->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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
		pRandom_AirHit->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
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

		pAction_GetUp->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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

		/* Set Options */
		pAction_Hit_1->Set_Options(TEXT("Air_Hit_1"), m_pModelCom);
		pAction_Hit_3->Set_Options(TEXT("Air_Hit_3"), m_pModelCom);
		pAction_Hit_4->Set_Options(TEXT("Air_Hit_4"), m_pModelCom);
		pAction_Hit_5->Set_Options(TEXT("Air_Hit_5"), m_pModelCom);
		pAction_Hit_6->Set_Options(TEXT("Air_Hit_6"), m_pModelCom);
		pAction_Hit_8->Set_Options(TEXT("Air_Hit_8"), m_pModelCom);
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Knockback_Back_Splat"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Getup_Back"), m_pModelCom);

		/* Assemble Childs */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Selector_AirHit"), pSelector_AirHit)))
			throw TEXT("Failed Assemble_Behavior Selector_AirHit");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Splat"), pAction_Splat)))
			throw TEXT("Failed Assemble_Behavior Action_Splat");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");

		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("Random_AirHit"), pRandom_AirHit)))
			throw TEXT("Failed Assemble_Behavior Random_AirHit");
		if (FAILED(pSelector_AirHit->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");

		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_1"), pAction_Hit_1, 0.133f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_1");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_3"), pAction_Hit_3, 0.133f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_3");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_4"), pAction_Hit_4, 0.133f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_4");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_5"), pAction_Hit_5, 0.133f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_5");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_6"), pAction_Hit_6, 0.133f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_6");
		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_8"), pAction_Hit_8, 0.135f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_8");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Air_Hit : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CGolem_Combat::Make_Fly_Descendo(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		if (FAILED(__super::Make_Fly_Descendo(pSequence)))
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
		CAction* pAction_Descendo4 = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo4)))
			throw TEXT("Failed Create_Behavior pAction_Descendo4");
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
		pAction_Descendo1->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				Print_Damage_Font(200);

				pHealth->Damaged(200);

				return true;
			});

		/* Set Options */
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Descendo_3"), m_pModelCom);
		pAction_Descendo4->Set_Options(TEXT("Descendo_Ground"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Getup_Front"), m_pModelCom);
		pTsk_RigidMove->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, -100.f, 0.6f);
		pTsk_RigidMove_Up->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, 3.f, 2.6f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo4"), pAction_Descendo4)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");

		if (FAILED(pAction_Descendo1->Assemble_Behavior(TEXT("Tsk_RigidMove"), pTsk_RigidMove)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove");
		if (FAILED(pAction_Descendo3->Assemble_Behavior(TEXT("Tsk_RigidMove_Up"), pTsk_RigidMove_Up)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Up");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Fly_Descendo : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CGolem_Combat::Enter_Light_Attack()
{
	if (true == m_isDead)
		return;

	m_CollisionRequestDesc.eType = ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 5;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	if (nullptr != m_pWeapon)
		m_pWeapon->On_Collider_Attack(&m_CollisionRequestDesc);
}

void CGolem_Combat::Enter_Heavy_Attack()
{
	if (true == m_isDead)
		return;

	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 10;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	if (nullptr != m_pWeapon)
		m_pWeapon->On_Collider_Attack(&m_CollisionRequestDesc);
}

void CGolem_Combat::Enter_Body_Attack()
{
	if (true == m_isDead)
		return;

	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 0;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	if (nullptr != m_pRigidBody)
		m_pRigidBody->Enable_Collision("Enemy_Attack", this, &m_CollisionRequestDesc);
}

void CGolem_Combat::Exit_Attack()
{
	if (true == m_isDead)
		return;

	m_CollisionRequestDesc.eType = ATTACK_NONE;
	m_CollisionRequestDesc.iDamage = 0;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	if (nullptr != m_pRigidBody)
		m_pRigidBody->Disable_Collision("Enemy_Attack");

	if (nullptr != m_pWeapon)
		m_pWeapon->Off_Collider_Attack(&m_CollisionRequestDesc);
}

void CGolem_Combat::DeathBehavior(const _float& fTimeDelta)
{
	m_isDead = true;
	m_fDeadTimeAcc += fTimeDelta;

	if (1.5f < m_fDeadTimeAcc)
	{
		m_isDissolve = true;
		m_fDissolveAmount += fTimeDelta / 1.5f; // 디졸브 값 증가
		m_pWeapon->On_Dissolve();
	}

	if (3.f < m_fDeadTimeAcc && m_fDissolveAmount >= 1.f)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CGolem_Combat::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Golem_Combat"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Golem_Combat::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_Golem_Combat"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGolem_Combat::Bind_HitMatrices()
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

CGolem_Combat* CGolem_Combat::Clone(void* pArg)
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

	Safe_Release(m_pWeapon);
	Safe_Release(m_pDescendo_Shake);
}
