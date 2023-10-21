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

_uint CDarkWizard_Fly::iNumClass = { 0 };
_float CDarkWizard_Fly::fAttackCoolTime = { 0.f };

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

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CDarkWizard_Fly::Tick(_float fTimeDelta)
{
	fAttackCoolTime += fTimeDelta;

	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[rand() % 3];

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CDarkWizard_Fly::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CDarkWizard_Fly::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
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

		if (nullptr != m_pHitMatrix)
			m_pUI_Damage->Add_Font(iDamage, XMVector3TransformCoord(m_pHitMatrix->Translation(), m_pTransform->Get_WorldMatrix()));

		m_pHealth->Damaged(iDamage);

		m_iCurrentSpell |= eBuff;
	}
}

void CDarkWizard_Fly::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
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

		CAction* pAction_Death = nullptr;
		if (FAILED(Create_Behavior(pAction_Death)))
			throw TEXT("Failed Create_Behavior pAction_Death");
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

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Death"), pAction_Death)))
			throw TEXT("Failed Assemble_Behavior Action_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");

		if (FAILED(Make_Death(pAction_Death)))
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

HRESULT CDarkWizard_Fly::Make_Magics()
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
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
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
		magicInitDesc.isChase = false;
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

	return S_OK;
}

HRESULT CDarkWizard_Fly::Make_Notifies()
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

	Func = [&] {(*this).Shot_Magic(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Magic"), Func)))
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
		HealthDesc.iMaxHP = 200;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
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

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_DarkWizard_Wand"),
			TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon");
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

void CDarkWizard_Fly::DeathBehavior(const _float& fTimeDelta)
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

HRESULT CDarkWizard_Fly::Make_Death(_Inout_ CAction* pAction)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pAction)
			throw TEXT("Parameter pAction is nullptr");

		CAction* pAction_Death = nullptr;
		if (FAILED(Create_Behavior(pAction_Death)))
			throw TEXT("Failed Create_Behavior pAction_Death");
		CDeath* pTsk_Death = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death)))
			throw TEXT("Failed Create_Behavior pTsk_Death");

		// Set Decorators
		pAction->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) { this->DeathBehavior(fTimeDelta); };
		pTsk_Death->Set_DeathFunction(Func);

		if (FAILED(pAction->Assemble_Behavior(TEXT("Tsk_Death"), pTsk_Death)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CDarkWizard_Fly] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CDarkWizard_Fly::Make_Alive(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behavior */
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

void CDarkWizard_Fly::Attack_Light()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_LIGHT;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
}

void CDarkWizard_Fly::Attack_Heavy()
{
	if (nullptr == m_pTarget)
		return;

	m_MagicDesc.eBuffType = BUFF_ATTACK_HEAVY;
	m_pMagicSlot->Add_Magics(m_MagicDesc);

	m_CastingMagic = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_PLAYER | COL_NPC | COL_SHIELD));
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

	--iNumClass;
}
