#include "DarkWizard_F.h"
#include "PhysXConverter.h"

#include "Client_GameInstance_Functions.h"

#include "Camera_Shake.h"

#include "Death.h"
#include "Action.h"
#include "MagicSlot.h"
#include "MagicBall.h"
#include "RigidMove.h"
#include "Check_Degree.h"
#include "Check_Distance.h"
#include "Selector_Degree.h"
#include "Sequence_Attack.h"
#include "Sequence_Groggy.h"
#include "Sequence_Descendo.h"
#include "Sequence_Levitate.h"
#include "UI_Group_Enemy_HP.h"
#include "Sequence_MoveTarget.h"
#include "Weapon_DarkWizard_Wand.h"

#include "UI_Damage.h"

_uint CDarkWizard_F::iNumClass = { 0 };
_float CDarkWizard_F::fAttackCoolTime = { 0.f };

CDarkWizard_F::CDarkWizard_F(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CDarkWizard_F::CDarkWizard_F(const CDarkWizard_F& rhs)
	: CEnemy(rhs)
{
}

HRESULT CDarkWizard_F::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDarkWizard_F::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Components_for_Shake()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	++iNumClass;

	return S_OK;
}

HRESULT CDarkWizard_F::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CDarkWizard_F::Tick(_float fTimeDelta)
{
	fAttackCoolTime += fTimeDelta;

	Set_Current_Target();

	m_fProtegoCoolTime += fTimeDelta;

	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[2];

	Tick_Spells();

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CDarkWizard_F::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDarkWizard_F::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);

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

		/* 현재 프로테고를 사용한 경우 */
		if (BUFF_PROTEGO & m_iCurrentSpell)
		{
			if (false == isPowerSpell(eBuff))
				return;
		}

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() &&
			BUFF_LEVIOSO & eBuff)
		{
			m_CurrentTickSpells.emplace(eBuff, Action);
		}

		if (nullptr != m_pHitMatrix)
			m_pUI_Damage->Add_Font(iDamage, XMVector3TransformCoord(m_pHitMatrix->Translation(), m_pTransform->Get_WorldMatrix()));

		m_pHealth->Damaged(iDamage);

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
		auto iter = m_RangeInEnemies.find(wstrObjectTag);
		if (iter == m_RangeInEnemies.end())
			m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	}

	/* Collision Protego */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Attack")) &&
		wstring::npos != wstrOtherCollisionTag.find(TEXT("Protego")))
	{
		m_isParring = true;
	}
}

void CDarkWizard_F::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
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

HRESULT CDarkWizard_F::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CDarkWizard_F] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

void CDarkWizard_F::Set_Protego_Collision(CTransform* pTransform, CEnemy::ATTACKTYPE eType) const
{
	if (eType == ATTACK_BREAK || eType == ATTACK_SUPERBREAK)
	{
		if (m_iCurrentSpell & BUFF_PROTEGO)
			m_iCurrentSpell ^= BUFF_PROTEGO;
		m_fProtegoCoolTime = 0.f;
	}
}

HRESULT CDarkWizard_F::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		if (FAILED(m_pRootBehavior->Add_Type("fProtegoCoolTime", &m_fProtegoCoolTime)))
			throw TEXT("Failed Add_Type fProtegoCoolTime");
		if (FAILED(m_pRootBehavior->Add_Type("iNumClass", &iNumClass)))
			throw TEXT("Failed Add_Type iNumClass");
		if (FAILED(m_pRootBehavior->Add_Type("fAttackCoolTime", &fAttackCoolTime)))
			throw TEXT("Failed Add_Type fAttackCoolTime");
		if (FAILED(m_pRootBehavior->Add_Type("isAbleAttack", _bool())))
			throw TEXT("Failed Add_Type isAbleAttack");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CSelector* pSelector_Death = nullptr;
		if (FAILED(Create_Behavior(pSelector_Death)))
			throw TEXT("Failed Create_Behavior pSelector_Death");
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

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Death"), pSelector_Death)))
			throw TEXT("Failed Assemble_Behavior Selector_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");

		if (FAILED(Make_Death(pSelector_Death)))
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

HRESULT CDarkWizard_F::Make_Magics()
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

	//Basic Magic Protego
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_PROTEGO;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = PROTEGO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.fLifeTime = 3600.f;
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

	//Skill Magic CONFRINGO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_CONFRINGO;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = CONFRINGO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = false;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic DIFFINDO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_DIFFINDO;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = DIFFINDO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = false;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	_uint iOwnerType = rand() % 4 + 1;

	m_pMagicSlot->Set_OwnerType(CMagic_Sound_Manager::OWNERTYPE(iOwnerType));

	switch (CMagic_Sound_Manager::OWNERTYPE(iOwnerType))
	{
	case CMagic_Sound_Manager::OWNER_BLACKMAGIC_A:
		m_pMagicSlot->Add_Magic_To_Skill_Slot(0, DIFFINDO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(1, DIFFINDO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(2, DIFFINDO);
		break;

	case CMagic_Sound_Manager::OWNER_BLACKMAGIC_B:
	case CMagic_Sound_Manager::OWNER_BLACKMAGIC_C:
		m_pMagicSlot->Add_Magic_To_Skill_Slot(0, CONFRINGO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(1, CONFRINGO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(2, CONFRINGO);
		break;

	case CMagic_Sound_Manager::OWNER_BLACKMAGIC_D:
		m_pMagicSlot->Add_Magic_To_Skill_Slot(0, LEVIOSO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(1, CONFRINGO);
		m_pMagicSlot->Add_Magic_To_Skill_Slot(2, DIFFINDO);
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CDarkWizard_F::Make_Notifies()
{
	function<void()> Func = [&] { (*this).Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Light(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Attack_Light"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Light(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Attack_Light1"), Func)))
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

	Func = [&] {(*this).Cast_Confringo(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Confringo"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic1"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic2"), Func)))
		return E_FAIL;

	Func = [&] {(*this).On_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Gravity"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Off_Gravity(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Gravity"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDarkWizard_F::Add_Components()
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
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_NPC_RANGE | COL_MAGIC | COL_STATIC;

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(9.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC;

		m_pRigidBody->Create_Collider(&RigidBodyDesc);

		/* For.MagicSlot */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_MagicSlot)");
			return E_FAIL;
		}

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::MONSTER;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("3"));
		lstrcpy(Desc.wszObjectName, TEXT("검은마법사"));

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_F] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDarkWizard_F::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_DarkWizard_F"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.Com_Weapon */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_DarkWizard_Wand::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_DarkWizard_Wand"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_F] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDarkWizard_F::Bind_HitMatrices()
{
	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("Head"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[0] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("Hips"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[1] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("Spine"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[2] = pBone->Get_CombinedTransformationMatrixPtr();

	return S_OK;
}

void CDarkWizard_F::DeathBehavior(const _float& fTimeDelta)
{
	m_isDead = true;
	m_fDeadTimeAcc += fTimeDelta;

	if (2.f < m_fDeadTimeAcc)
	{
		m_isDissolve = true;
		m_fDissolveAmount += fTimeDelta / 1.5f; // 디졸브 값 증가
		m_pWeapon->On_Dissolve();
	}

	if (4.f < m_fDeadTimeAcc && m_fDissolveAmount >= 1.f)
		Set_ObjEvent(OBJ_DEAD);
}

HRESULT CDarkWizard_F::Make_Death(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		CRandomChoose* pRandom_Death_Ground = nullptr;
		if (FAILED(Create_Behavior(pRandom_Death_Ground)))
			throw TEXT("Failed Create_Behavior pRandom_Death_Ground");
		CSequence* pSequence_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death_Air)))
			throw TEXT("Failed Create_Behavior pSequence_Death_Air");

		CAction* pAction_Death_Front = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Front)))
			throw TEXT("Failed Create_Behavior pAction_Death_Front");
		CAction* pAction_Death_Back = nullptr;
		if (FAILED(Create_Behavior(pAction_Death_Back)))
			throw TEXT("Failed Create_Behavior pAction_Death_Back");
		CAction* pAction_Knockback = nullptr;
		if (FAILED(Create_Behavior(pAction_Knockback)))
			throw TEXT("Failed Create_Behavior pAction_Knockback");
		CAction* pAction_Splat = nullptr;
		if (FAILED(Create_Behavior(pAction_Splat)))
			throw TEXT("Failed Create_Behavior pAction_Splat");

		CDeath* pTsk_Death_Front = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Front)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Front");
		CDeath* pTsk_Death_Back = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Back)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Back");
		CDeath* pTsk_Death_Air = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death_Air)))
			throw TEXT("Failed Create_Behavior pTsk_Death_Air");

		// Set Decorators
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});
		pRandom_Death_Ground->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) { this->DeathBehavior(fTimeDelta); };
		pTsk_Death_Front->Set_DeathFunction(Func);
		pTsk_Death_Back->Set_DeathFunction(Func);
		pTsk_Death_Air->Set_DeathFunction(Func);
		pAction_Death_Front->Set_Options(TEXT("Death_Front"), m_pModelCom);
		pAction_Death_Back->Set_Options(TEXT("Death_Back"), m_pModelCom);
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Death_Back_Splat"), m_pModelCom, true);

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Death_Ground"), pRandom_Death_Ground)))
			throw TEXT("Failed Assemble_Behavior Random_Death_Ground");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death_Air"), pSequence_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death_Air");

		if (FAILED(pRandom_Death_Ground->Assemble_Behavior(TEXT("Action_Death_Front"), pAction_Death_Front, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Action_Death_Front");
		if (FAILED(pRandom_Death_Ground->Assemble_Behavior(TEXT("pAction_Death_Back"), pAction_Death_Back, 0.5f)))
			throw TEXT("Failed Assemble_Behavior pAction_Death_Back");

		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Knockback"), pAction_Knockback)))
			throw TEXT("Failed Assemble_Behavior Action_Knockback");
		if (FAILED(pSequence_Death_Air->Assemble_Behavior(TEXT("Action_Splat"), pAction_Splat)))
			throw TEXT("Failed Assemble_Behavior Action_Splat");

		if (FAILED(pAction_Death_Front->Assemble_Behavior(TEXT("Tsk_Death_Front"), pTsk_Death_Front)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Front");
		if (FAILED(pAction_Death_Back->Assemble_Behavior(TEXT("Tsk_Death_Back"), pTsk_Death_Back)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Back");
		if (FAILED(pAction_Splat->Assemble_Behavior(TEXT("Tsk_Death_Air"), pTsk_Death_Air)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death_Air");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_F] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDarkWizard_F::Make_Alive(_Inout_ CSelector* pSelector)
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
		CSequence* pSequence_Protego = nullptr;
		if (FAILED(Create_Behavior(pSequence_Protego)))
			throw TEXT("Failed Create_Behavior pSequence_Protego");
		CSelector* pSelector_NormalAttack = nullptr;
		if (FAILED(Create_Behavior(pSelector_NormalAttack)))
			throw TEXT("Failed Create_Behavior pSelector_NormalAttack");
		CRandomChoose* pRandom_Taunts = nullptr;
		if (FAILED(Create_Behavior(pRandom_Taunts)))
			throw TEXT("Failed Create_Behavior pRandom_Taunts");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return !(pHealth->isDead());
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Hit_Combo"), pSelector_Hit_Combo)))
			throw TEXT("Failed Assemble_Behavior Selector_Hit_Combo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_CheckSpell"), pSelector_CheckSpell)))
			throw TEXT("Failed Assemble_Behavior Selector_CheckSpell");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Protego"), pSequence_Protego)))
			throw TEXT("Failed Assemble_Behavior Sequence_Protego");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_NormalAttack"), pSelector_NormalAttack)))
			throw TEXT("Failed Assemble_Behavior Selector_NormalAttack");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Taunts"), pRandom_Taunts)))
			throw TEXT("Failed Assemble_Behavior Random_Taunts");

		if (FAILED(Make_Hit_Combo(pSelector_Hit_Combo)))
			throw TEXT("Failed Make_Hit_Combo");
		if (FAILED(Make_Check_Spell(pSelector_CheckSpell)))
			throw TEXT("Failed Make_Check_Spell");
		if (FAILED(Make_Protego(pSequence_Protego)))
			throw TEXT("Failed Make_Check_Spell");
		if (FAILED(Make_NormalAttack(pSelector_NormalAttack)))
			throw TEXT("Failed Make_NormalAttack");
		if (FAILED(Make_Taunts(pRandom_Taunts)))
			throw TEXT("Failed Make_Taunts");
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

HRESULT CDarkWizard_F::Make_Hit_Combo(_Inout_ CSelector* pSelector)
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

HRESULT CDarkWizard_F::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		if (FAILED(__super::Make_Check_Spell(pSelector)))
			throw TEXT("Failed __super Make_Check_Spell");

		/* Make Child Behaviors */
		CAction* pAction_Groggy = nullptr;
		if (FAILED(Create_Behavior(pAction_Groggy)))
			throw TEXT("Failed Create_Behavior pAction_Groggy");
		CSequence_Levitate* pSequence_Levitate = nullptr;
		if (FAILED(Create_Behavior(pSequence_Levitate)))
			throw TEXT("Failed Create_Behavior pSequence_Levitate");
		CSequence_Descendo* pSequence_Descendo = nullptr;
		if (FAILED(Create_Behavior(pSequence_Descendo)))
			throw TEXT("Failed Create_Behavior pSequence_Descendo");

		/* Set Decorations */
		pAction_Groggy->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_STUPEFY & *pICurrentSpell)
					return true;

				return false;
			});

		pAction_Groggy->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (*pICurrentSpell & BUFF_STUPEFY)
					*pICurrentSpell ^= BUFF_STUPEFY;

				return true;
			});

		/* Set_Options */
		pAction_Groggy->Set_Options(TEXT("Groggy"), m_pModelCom);
		pSequence_Levitate->Set_Option(3.f, 1.6f);

		/* Stupefy */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Groggy"), pAction_Groggy)))
			throw TEXT("Failed Assemble_Behavior Action_Groggy");
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

HRESULT CDarkWizard_F::Make_Protego(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behavior */
		CAction* pAction_Protego = { nullptr };
		if (FAILED(Create_Behavior(pAction_Protego)))
			throw TEXT("FAiled Create_Behavior pAction_Protego");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				_float* pProtegoCoolTime = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("fProtegoCoolTime", pProtegoCoolTime)))
					return false;

				if (BUFF_PROTEGO & *pICurrentSpell)
					return false;
				if (10.f > *pProtegoCoolTime)
					return false;

				return true;
			});

		/* Set Options */
		pAction_Protego->Set_Options(TEXT("Cast_Protego"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Protego"), pAction_Protego)))
			throw TEXT("Failed Assemble_Behavior Action_Protego");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CGolem_Combat] Failed Make_Protego : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDarkWizard_F::Make_NormalAttack(_Inout_ CSelector* pSelector)
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
		CRandomChoose* pRandom_Attack = nullptr;
		if (FAILED(Create_Behavior(pRandom_Attack)))
			throw TEXT("Failed Create_Behavior pRandom_Attack");

		CAction* pAction_Cast_Levioso = nullptr;
		if (FAILED(Create_Behavior(pAction_Cast_Levioso)))
			throw TEXT("Failed Create_Behavior pAction_Cast_Levioso");
		CAction* pAction_Cast_Confringo = { nullptr };
		if (FAILED(Create_Behavior(pAction_Cast_Confringo)))
			throw TEXT("Failed Create_Behavior pAction_Cast_Confringo");
		CSequence_Attack* pSequence_Attack_1 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_1");
		CSequence_Attack* pSequence_Attack_2 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_2");
		CSequence_Attack* pSequence_Attack_3 = nullptr;
		if (FAILED(Create_Behavior(pSequence_Attack_3)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_3");

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
		pRandom_Attack->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pNumClass = { nullptr };
				_float* pAttackCoolTime = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iNumClass", pNumClass)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("fAttackCoolTime", pAttackCoolTime)))
					return false;

				_float fCoolTime = _float(*pNumClass * 3.f);

				if (fCoolTime < *pAttackCoolTime)
				{
					*pAttackCoolTime = 0.f;
					if (FAILED(pBlackBoard->Set_Type("isAbleAttack", true)))
						return false;
					return true;
				}

				_bool isAbleAttack = { false };
				if (FAILED(pBlackBoard->Get_Type("isAbleAttack", isAbleAttack)))
					return false;

				return isAbleAttack;
			});
		pRandom_Attack->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				if (FAILED(pBlackBoard->Set_Type("isAbleAttack", false)))
					return false;

				return true;
			});
		pRandom_Attack->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Cast_Levioso->Set_Options(TEXT("Cast_Levioso"), m_pModelCom);
		pAction_Cast_Confringo->Set_Options(TEXT("Cast_Confringo"), m_pModelCom);
		pSequence_Attack_1->Set_Attack_Action_Options(TEXT("Attack_2Combo_1"), m_pModelCom);
		pSequence_Attack_1->Set_Attack_Option(15.f);
		pSequence_Attack_2->Set_Attack_Action_Options(TEXT("Attack_2Combo_2"), m_pModelCom);
		pSequence_Attack_2->Set_Attack_Option(15.f);
		pSequence_Attack_3->Set_Attack_Action_Options(TEXT("Attack_3Combo"), m_pModelCom);
		pSequence_Attack_3->Set_Attack_Option(15.f);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Attack"), pRandom_Attack)))
			throw TEXT("Failed Assemble_Behavior Random_Attack");

		if (FAILED(Make_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Turns");

		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Cast_Levioso"), pAction_Cast_Levioso, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Cast_Levioso");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Action_Cast_Confringo"), pAction_Cast_Confringo, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Action_Cast_Confringo");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Sequence_Attack_1"), pSequence_Attack_1, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_1");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Sequence_Attack_2"), pSequence_Attack_2, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_2");
		if (FAILED(pRandom_Attack->Assemble_Behavior(TEXT("Sequence_Attack_3"), pSequence_Attack_3, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_3");
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

	return S_OK;;
}

HRESULT CDarkWizard_F::Make_Taunts(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Taunt_1 = nullptr;
		if (FAILED(Create_Behavior(pAction_Taunt_1)))
			throw TEXT("Failed Create_Behavior pAction_Taunt_1");
		CAction* pAction_Taunt_2 = nullptr;
		if (FAILED(Create_Behavior(pAction_Taunt_2)))
			throw TEXT("Failed Create_Behavior pAction_Taunt_2");
		CAction* pAction_Taunt_3 = nullptr;
		if (FAILED(Create_Behavior(pAction_Taunt_3)))
			throw TEXT("Failed Create_Behavior pAction_Taunt_3");

		/* Set Decorations */
		pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
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
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Taunt_1->Set_Options(TEXT("Taunt_1"), m_pModelCom);
		pAction_Taunt_2->Set_Options(TEXT("Taunt_2"), m_pModelCom);
		pAction_Taunt_3->Set_Options(TEXT("Taunt_3"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Taunt_1"), pAction_Taunt_1, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Taunt_1");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Taunt_2"), pAction_Taunt_2, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Taunt_2");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Taunt_3"), pAction_Taunt_3, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Action_Taunt_3");
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

HRESULT CDarkWizard_F::Make_Fly_Combo(_Inout_ CSelector* pSelector)
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

HRESULT CDarkWizard_F::Make_Turns(_Inout_ CSequence* pSequence)
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

		CAction* pAction_Right_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_45)
			throw TEXT("pAction_Right_45 is nullptr");
		CAction* pAction_Left_45 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_45)
			throw TEXT("pAction_Left_45 is nullptr");
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
		CAction* pAction_Left_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Left_180)
			throw TEXT("pAction_Left_180 is nullptr");
		CAction* pAction_Right_180 = dynamic_cast<CAction*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Action")));
		if (nullptr == pAction_Right_180)
			throw TEXT("pAction_Right_180 is nullptr");
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
		wstring wstrErrorMSG = TEXT("[CDarkWizard_F] Failed Make_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDarkWizard_F::Make_Air_Hit(_Inout_ CSequence* pSequence)
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
		pAction_Knockback->Set_Options(TEXT("Knockback_Back"), m_pModelCom);
		pAction_Splat->Set_Options(TEXT("Splat_Back"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Getup_Back"), m_pModelCom);
		pAction_Hit_1->Add_Exit_Condition([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsOnGround = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isOnGround", pIsOnGround)))
					return false;
				if (true == *pIsOnGround)
					int i = 0;
				return *pIsOnGround;
			});
		pAction_Knockback->Add_Exit_Condition([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsOnGround = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isOnGround", pIsOnGround)))
					return false;

				return *pIsOnGround;
			});

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

		if (FAILED(pRandom_AirHit->Assemble_Behavior(TEXT("Action_Hit_1"), pAction_Hit_1, 1.f)))
			throw TEXT("Failed Assemble_Behavior Action_Hit_1");
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

HRESULT CDarkWizard_F::Make_Fly_Descendo(_Inout_ CSequence* pSequence)
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
		pAction_Descendo1->Set_Options(TEXT("Descendo_1"), m_pModelCom);
		pAction_Descendo2->Set_Options(TEXT("Descendo_2"), m_pModelCom);
		pAction_Descendo3->Set_Options(TEXT("Descendo_Ground"), m_pModelCom);
		pAction_GetUp->Set_Options(TEXT("Getup_Front"), m_pModelCom);
		pTsk_RigidMove->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, -100.f, 0.6f);
		pTsk_RigidMove_Up->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, 4.f, 1.2f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo1"), pAction_Descendo1)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo2"), pAction_Descendo2)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Descendo3"), pAction_Descendo3)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_GetUp"), pAction_GetUp)))
			throw TEXT("Failed Assemble_Behavior Action_GetUp");

		if (FAILED(pAction_Descendo1->Assemble_Behavior(TEXT("Tsk_RigidMove"), pTsk_RigidMove)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove");
		if (FAILED(pAction_Descendo2->Assemble_Behavior(TEXT("Tsk_RigidMove_Up"), pTsk_RigidMove_Up)))
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

void CDarkWizard_F::Attack_Light()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_LIGHT;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_F::Attack_Heavy()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_HEAVY;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_F::Cast_Levioso()
{
	if (nullptr == m_pTarget)
		return;

	_float4x4 OffsetMatrix = _float4x4();
	if (nullptr != m_pTarget)
		OffsetMatrix = m_pTarget->Get_Offset_Matrix();

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_F::Cast_Protego()
{
	if (nullptr == m_pTarget)
		return;

	m_pHitMatrix = m_HitMatrices[2];
	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(1, this, m_pWeapon, COL_MAGIC, COL_SHIELD_ENEMY);
	m_iCurrentSpell |= BUFF_PROTEGO;
}

void CDarkWizard_F::Cast_Confringo()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_F::Cast_Diffindo()
{
	if (nullptr == m_pTarget)
		return;

	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(2, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_F::Shot_Magic()
{
	m_CastingMagic->Do_MagicBallState_To_Next();
}

CDarkWizard_F* CDarkWizard_F::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDarkWizard_F* pInstance = New CDarkWizard_F(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDarkWizard_F");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDarkWizard_F::Clone(void* pArg)
{
	CDarkWizard_F* pInstance = New CDarkWizard_F(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDarkWizard_F");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDarkWizard_F::Free()
{
	__super::Free();

	Safe_Release(m_pWeapon);
	Safe_Release(m_pMagicSlot);

	--iNumClass;
}
