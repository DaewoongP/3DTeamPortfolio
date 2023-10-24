#include "DarkWizard_Fly.h"
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

_int CDarkWizard_Fly::iNumClass = { 0 };

CDarkWizard_Fly::CDarkWizard_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CDarkWizard_Fly::CDarkWizard_Fly(const CDarkWizard_Fly& rhs)
	: CEnemy(rhs)
{
}

HRESULT CDarkWizard_Fly::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDarkWizard_Fly::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Components_for_Shake()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	m_vMoveTargetPosition = _float3(100.f, 30.f, 180.f);

	++iNumClass;

	return S_OK;
}

HRESULT CDarkWizard_Fly::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(Make_AI()))
		return E_FAIL;

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(Make_Notifies_for_Shake()))
		return E_FAIL;

	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));
	m_pRigidBody->Set_LinearDamping(2.f);
	m_pModelCom->Change_Animation(TEXT("Hover_Fly_Front_Loop"));

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CDarkWizard_Fly::Tick(_float fTimeDelta)
{
	if (CGameInstance::GetInstance()->Get_DIKeyState(DIK_6))
		m_isSpawn = true;

	Update_Target();

	__super::Tick(fTimeDelta);

	if (true == m_isSpawn)
	{
		Move(fTimeDelta);
	}
	
	m_pHitMatrix = m_HitMatrices[rand() % 3];

	if (nullptr != m_pModelCom)
	{
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UNDERBODY);
	}
}

void CDarkWizard_Fly::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDarkWizard_Fly::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);

	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrOtherCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		if (nullptr != m_pHitMatrix)
			m_pUI_Damage->Add_Font(iDamage, XMVector3TransformCoord(m_pHitMatrix->Translation(), m_pTransform->Get_WorldMatrix()));

		m_pHealth->Damaged(iDamage);

		m_iCurrentSpell |= eBuff;
	}

	if (true == is_Enemy(wstrOtherCollisionTag))
	{
		if (nullptr != Find_Enemy(wstrObjectTag))
			return;

		m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	}
}

void CDarkWizard_Fly::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;
	wstring wstrOtherCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (false == IsEnemy(wstrOtherCollisionTag))
			return;

		Remove_GameObject(wstrObjectTag);
	}
}

HRESULT CDarkWizard_Fly::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CDarkWizard_Fly] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDarkWizard_Fly::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		if (FAILED(m_pRootBehavior->Add_Type("vMoveTargetPosition", &m_vMoveTargetPosition)))
			throw TEXT("Failed Add_Type vMoveTargetPosition");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CSelector* pSelector_Check_Spell = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Check_Spell)))
			throw TEXT("Failed Create_Behavior pSelector_Check_Spell");
		CRandomChoose* pRandom_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");
		/*CSequence* pSequence_Move = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Move)))
			throw TEXT("Failed Create_Behavior pSequence_Move");*/

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

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Check_Spell"), pSelector_Check_Spell)))
			throw TEXT("Failed Assemble_Behavior Selector_Check_Spell");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");
		/*if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Move"), pSequence_Move)))
			throw TEXT("Failed Assemble_Behavior Sequence_Move");*/

		if (FAILED(Make_Check_Spell(pSelector_Check_Spell)))
			throw TEXT("Failed Make_Check_Spell");
		if (FAILED(Make_Attacks(pRandom_Attacks)))
			throw TEXT("Failed Make_Attacks");
		/*if (FAILED(Make_Move(pSequence_Move)))
			throw TEXT("Failed Make_Move");*/
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

HRESULT CDarkWizard_Fly::Make_Magics()
{
	//Skill Magic LEVIOSO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_LEVIOSO;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = LEVIOSO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 1;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic DESCENDO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_DESCENDO;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
		magicInitDesc.eMagicTag = DESCENDO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 1;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic FLIPENDO
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_FLIPENDO;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
		magicInitDesc.eMagicTag = FLIPENDO;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 1;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//Skill Magic ARRESTOMOMENTUM
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_ARRESTOMOMENTUM;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = ARRESTOMOMENTUM;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 1;
		magicInitDesc.fLifeTime = 0.8f;
		magicInitDesc.isChase = true;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, LEVIOSO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, DESCENDO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(2, FLIPENDO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(3, ARRESTOMOMENTUM);

	return S_OK;
}

HRESULT CDarkWizard_Fly::Make_Notifies()
{
	function<void()> Func = [&] { this->Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] { this->Cast_Levioso(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Cast_Levioso"), TEXT("Cast_Levioso"), Func, CModel::UNDERBODY)))
		return E_FAIL;

	Func = [&] { this->Cast_Descendo(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Cast_Descendo"), TEXT("Cast_Descendo"), Func, CModel::UNDERBODY)))
		return E_FAIL;

	Func = [&] { this->Cast_Flipendo(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Cast_Flipendo"), TEXT("Cast_Flipendo"), Func, CModel::UNDERBODY)))
		return E_FAIL;

	Func = [&] { this->Cast_Arrestomomentum(); };
	if (FAILED(m_pModelCom->Bind_Notify(TEXT("Cast_Arrestomomentum"), TEXT("Cast_Arrestomomentum"), Func, CModel::UNDERBODY)))
		return E_FAIL;

	Func = [&] { this->Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func, CModel::UNDERBODY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDarkWizard_Fly::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 1000000;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		PxSphereGeometry pSphereGeomatry = PxSphereGeometry(0.8f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_ENEMY_RANGE | COL_MAGIC | COL_STATIC;

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
		pSphereGeomatry = PxSphereGeometry(9.f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_ENEMY;

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
		lstrcpy(Desc.wszObjectLevel, TEXT("5"));
		wstring wstrEnemyTag = TEXT("검은마법사_");
		wstrEnemyTag += to_wstring(iNumClass);
		lstrcpy(Desc.wszObjectName, wstrEnemyTag.c_str());

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDarkWizard_Fly::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_DarkWizard_Fly"),
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

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Weapon_DarkWizard_Wand"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");

		/* For.Com_Broom_Stick */
		pBone = m_pModelCom->Get_Bone(TEXT("LeftInHandRing"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Broom_Stick_DarkWizard"),
			TEXT("Com_Broom_Stick"), reinterpret_cast<CComponent**>(&m_pBroom_Stick), &ParentMatrixDesc)))
			throw TEXT("Com_Broom_Stick");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDarkWizard_Fly::Bind_HitMatrices()
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

HRESULT CDarkWizard_Fly::Make_Check_Spell(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Levitate = nullptr;
		if (FAILED(Create_Behavior(pAction_Levitate)))
			throw TEXT("Failed Create_Behavior pAction_Levitate");
		CAction* pAction_Descendo = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo)))
			throw TEXT("Failed Create_Behavior pAction_Descendo");
		CAction* pAction_Flipendo = nullptr;
		if (FAILED(Create_Behavior(pAction_Flipendo)))
			throw TEXT("Failed Create_Behavior pAction_Flipendo");
		CAction* pAction_Arrestomomentum = nullptr;
		if (FAILED(Create_Behavior(pAction_Arrestomomentum)))
			throw TEXT("Failed Create_Behavior pAction_Arrestomomentum");

		CRigidMove* pTsk_RigidMove_Up = { nullptr };
		if (FAILED(Create_Behavior(pTsk_RigidMove_Up)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove_Up");
		CRigidMove* pTsk_RigidMove_Down = { nullptr };
		if (FAILED(Create_Behavior(pTsk_RigidMove_Down)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove_Down");

		/* Set Decorations */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_NONE != *pICurrentSpell;
			});
		pSelector->Add_Fail_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				*pICurrentSpell = BUFF_NONE;

				return true;
			});

		pAction_Levitate->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_LEVIOSO & *pICurrentSpell;
			});
		pAction_Levitate->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_LEVIOSO & *pICurrentSpell)
					*pICurrentSpell ^= BUFF_LEVIOSO;

				return true;
			});
		pAction_Descendo->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_DESCENDO & *pICurrentSpell;
			}); 
		pAction_Descendo->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
				{
					_uint* pICurrentSpell = { nullptr };
					if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
						return false;

					if (BUFF_DESCENDO & *pICurrentSpell)
						*pICurrentSpell ^= BUFF_DESCENDO;

					return true;
				});
		pAction_Flipendo->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_FLIPENDO & *pICurrentSpell;
			});
		pAction_Flipendo->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_FLIPENDO & *pICurrentSpell)
					*pICurrentSpell ^= BUFF_FLIPENDO;

				return true;
			});
		pAction_Arrestomomentum->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_ARRESTOMOMENTUM & *pICurrentSpell;
			});
		pAction_Arrestomomentum->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				if (BUFF_ARRESTOMOMENTUM & *pICurrentSpell)
					*pICurrentSpell ^= BUFF_ARRESTOMOMENTUM;

				return true;
			});

		/* Set_Options */
		pAction_Levitate->Set_Options(TEXT("Impact_Levitate"), m_pModelCom, true);
		pAction_Descendo->Set_Options(TEXT("Impact_Descendo"), m_pModelCom, true);
		pAction_Flipendo->Set_Options(TEXT("Impact_Flipendo"), m_pModelCom);
		pAction_Arrestomomentum->Set_Options(TEXT("Impact_Arrestomomentum"), m_pModelCom);
		pTsk_RigidMove_Up->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, 100.f, 1.f);
		pTsk_RigidMove_Down->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_UP, -100.f, 1.f);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Levitate"), pAction_Levitate)))
			throw TEXT("Failed Assemble_Behavior Action_Levitate");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Descendo"), pAction_Descendo)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Flipendo"), pAction_Flipendo)))
			throw TEXT("Failed Assemble_Behavior Action_Flipendo");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Arrestomomentum"), pAction_Arrestomomentum)))
			throw TEXT("Failed Assemble_Behavior Action_Arrestomomentum");

		if (FAILED(pAction_Levitate->Assemble_Behavior(TEXT("Tsk_RigidMove_Up"), pTsk_RigidMove_Up)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Up");
		if (FAILED(pAction_Descendo->Assemble_Behavior(TEXT("Tsk_RigidMove_Down"), pTsk_RigidMove_Down)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Down");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Make_Check_Spell : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;;
}

HRESULT CDarkWizard_Fly::Make_Attacks(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Make Child Behaviors */
		CAction* pAction_Levioso = nullptr;
		if (FAILED(Create_Behavior(pAction_Levioso)))
			throw TEXT("Failed Create_Behavior pAction_Levioso");
		CAction* pAction_Descendo = nullptr;
		if (FAILED(Create_Behavior(pAction_Descendo)))
			throw TEXT("Failed Create_Behavior pAction_Descendo");
		CAction* pAction_Flipendo = nullptr;
		if (FAILED(Create_Behavior(pAction_Flipendo)))
			throw TEXT("Failed Create_Behavior pAction_Flipendo");
		CAction* pAction_Arrestomomentum = nullptr;
		if (FAILED(Create_Behavior(pAction_Arrestomomentum)))
			throw TEXT("Failed Create_Behavior pAction_Arrestomomentum");

		/* Set Decorations */
		pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_uint* pICurrentSpell = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("iCurrentSpell", pICurrentSpell)))
					return false;

				return BUFF_NONE == *pICurrentSpell;
			});
		pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				const CGameObject** ppTarget = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("cppTarget", ppTarget)))
					return false;

				return nullptr != *ppTarget;
			});
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set_Options */
		pAction_Levioso->Set_Options(TEXT("Cast_Levioso"), m_pModelCom, false, 0.f, false, true, CModel::UNDERBODY);
		pAction_Descendo->Set_Options(TEXT("Cast_Descendo"), m_pModelCom, false, 0.f, false, true, CModel::UNDERBODY);
		pAction_Flipendo->Set_Options(TEXT("Cast_Flipendo"), m_pModelCom, false, 0.f, false, true, CModel::UNDERBODY);
		pAction_Arrestomomentum->Set_Options(TEXT("Cast_Arrestomomentum"), m_pModelCom, false, 0.f, false, true, CModel::UNDERBODY);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Levioso"), pAction_Levioso, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Action_Levioso");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Descendo"), pAction_Descendo, 0.5f)))
			throw TEXT("Failed Assemble_Behavior Action_Descendo");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Flipendo"), pAction_Flipendo, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Flipendo");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Arrestomomentum"), pAction_Arrestomomentum, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Action_Arrestomomentum");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Make_Attacks : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

// 현재 Look의 위, 아래, 좌, 우에 따른 애니메이션을 변화하는 방향으로 가는게 좋을 듯
HRESULT CDarkWizard_Fly::Make_Move(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Make Child Behaviors */


		/* Set Decorations */

		/* Set_Options */

		/* Assemble Behaviors */

	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Make_Move : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

_bool CDarkWizard_Fly::is_Enemy(const wstring& wstrObjectTag)
{
	if (wstring::npos != wstrObjectTag.find(TEXT("Player")))
		return true;

	if (wstring::npos != wstrObjectTag.find(TEXT("Enemy_Body")))
		return true;

	return false;
}

const CGameObject* CDarkWizard_Fly::Find_Enemy(const wstring& wstrObjectTag)
{
	auto iter = m_RangeInEnemies.find(wstrObjectTag);

	if (m_RangeInEnemies.end() == iter)
		return nullptr;

	return iter->second;
}

void CDarkWizard_Fly::Update_Target()
{
	_float3 vPosition = m_pTransform->Get_Position();

	m_pTarget = nullptr;

	for (auto& Pair : m_RangeInEnemies)
	{
		CTransform* pOtherTransform = Pair.second->Get_Transform();
		_float3 vOtherPosition = pOtherTransform->Get_Position();
		_float3 vPosition = m_pTransform->Get_Position();

		_float3 vDirection = vOtherPosition - vPosition;
		_float3 vLook = m_pTransform->Get_Look();
		vLook.y = 0.f;
		vDirection.y = 0.f;
		vLook.Normalize();
		vDirection.Normalize();

		_float fDegree = XMConvertToDegrees(acosf(vLook.Dot(vDirection)));

		if (true == isnan(fDegree))
			fDegree = 0.f;

		if (90.f < fDegree)
			continue;

		if (nullptr == m_pTarget)
			m_pTarget = Pair.second;
		else
		{
			_float3 vSrcTargetPosition = m_pTarget->Get_Transform()->Get_Position();
			_float3 vDstTargetPosition = vOtherPosition;

			_float vSrcDistance = _float3::Distance(vPosition, vSrcTargetPosition);
			_float vDstDistance = _float3::Distance(vPosition, vDstTargetPosition);

			m_pTarget = (vSrcDistance < vDstDistance) ? m_pTarget : Pair.second;
		}
	}

	m_isRangeInEnemy = (nullptr == m_pTarget) ? false : true;
}

void CDarkWizard_Fly::Move(const _float& fTimeDelta)
{
	_float3 vVelocity = m_pRigidBody->Get_Current_Velocity();
	if (10.f < vVelocity.Length())
		return;
	
	_float3 vLook = m_pTransform->Get_Look();

	m_pRigidBody->Add_Force(vLook * 50.f);

	m_pTransform->LookAt_Lerp(m_vMoveTargetPosition, fTimeDelta * 2.f);
}

void CDarkWizard_Fly::Cast_Levioso()
{
	if (nullptr == m_pTarget)
		return;
	++m_iCount;
	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_ENEMY | COL_SHIELD));
}

void CDarkWizard_Fly::Cast_Descendo()
{
	if (nullptr == m_pTarget)
		return;

	++m_iCount;
	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_ENEMY | COL_SHIELD));
}

void CDarkWizard_Fly::Cast_Flipendo()
{
	if (nullptr == m_pTarget)
		return;

	++m_iCount;
	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(2, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_ENEMY | COL_SHIELD));
}

void CDarkWizard_Fly::Cast_Arrestomomentum()
{
	if (nullptr == m_pTarget)
		return;

	++m_iCount;
	m_CastingMagic = m_pMagicSlot->Action_Magic_Skill(3, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_ENEMY | COL_SHIELD));
}

void CDarkWizard_Fly::Shot_Magic()
{
	m_CastingMagic->Do_MagicBallState_To_Next();
}

CDarkWizard_Fly* CDarkWizard_Fly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDarkWizard_Fly* pInstance = New CDarkWizard_Fly(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDarkWizard_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDarkWizard_Fly::Clone(void* pArg)
{
	CDarkWizard_Fly* pInstance = New CDarkWizard_Fly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDarkWizard_Fly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDarkWizard_Fly::Free()
{
	__super::Free();

	Safe_Release(m_pWeapon);
	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pBroom_Stick);

	--iNumClass;
}
