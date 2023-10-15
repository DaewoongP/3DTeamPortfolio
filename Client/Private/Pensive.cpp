#include "Pensive.h"
#include "GameInstance.h"
#include "StateContext_Enemy.h"
#include "Weapon_Pensive.h"
#include "UI_Group_Enemy_HP.h"
#include "MagicSlot.h"
#include "Weapon_Dragon_Head.h"
#include "MagicBall.h"

CPensive::CPensive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CPensive::CPensive(const CPensive& rhs)
	: CEnemy(rhs)
{
}

HRESULT CPensive::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPensive::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPensive::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	if (FAILED(Ready_StateMachine(iCurrentLevelIndex)))
		return E_FAIL;

	if (FAILED(Add_Magic()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CPensive::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta);
}

void CPensive::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPensive::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(__super::Render()))
	{
		MSG_BOX("[CArmored_Troll] Failed Render");
		return E_FAIL;
	}

	return S_OK;
}

void CPensive::Set_Protego_Collision(CTransform* pTransform, ATTACKTYPE eType) const
{
	if (eType == ATTACK_BREAK ||
		eType == ATTACK_SUPERBREAK)
	{
		m_pStateContext->Set_StateMachine(TEXT("Groogy"));
		m_pModelCom->Change_Animation(TEXT("Stun_Start"));
		//º’ø° ¿÷¥¯ ∏∂π˝ ≈Õ∂ﬂ∑¡¡÷±‚
		m_pMagicBall_Fail->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_DYING);
	}
}

HRESULT CPensive::Ready_StateMachine(_uint iCurrentLevelIndex)
{
	m_StateMachineDesc.pOwnerModel = m_pModelCom;
	m_StateMachineDesc.pPhase = &m_iPhase;
	m_StateMachineDesc.pAttackType = &m_iAttackType;
	BEGININSTANCE;
	CComponent* pPlayer = pGameInstance->Find_Component_In_Layer(0, TEXT("Layer_Player"), TEXT("GameObject_Player"));
	if (pPlayer != nullptr)
	{
		m_StateMachineDesc.pTarget = static_cast<CGameObject*>(pPlayer)->Get_Transform();
	}
	else 
	{
		MSG_BOX("Cant Find Player");
	}
	ENDINSTANCE;

	Safe_AddRef(m_StateMachineDesc.pOwnerModel);
	Safe_AddRef(m_StateMachineDesc.pTarget);

	Client::LEVELID elevelID = (Client::LEVELID)iCurrentLevelIndex;

	try
	{
		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Hit"),
			TEXT("Hit"),
			TEXT("Prototype_Component_Pensive_Hit"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Hit");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Groogy"),
			TEXT("Groogy"),
			TEXT("Prototype_Component_Pensive_Groogy"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Groogy");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Charge_Attack"),
			TEXT("Charge_Attack"),
			TEXT("Prototype_Component_Pensive_Charge_Attack"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Charge_Attack");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Death"),
			TEXT("Death"),
			TEXT("Prototype_Component_Pensive_Death"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Death");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Appearence"),
			TEXT("Appearence"),
			TEXT("Prototype_Component_Pensive_Appearence"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Appearence");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Idle"),
			TEXT("Idle"),
			TEXT("Prototype_Component_Pensive_Idle"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Idle");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Physical_Attack"),
			TEXT("Physical_Attack"),
			TEXT("Prototype_Component_Pensive_Physical_Attack"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Physical_Attack");

		if (FAILED(m_pStateContext->Add_StateMachine(
			elevelID,
			TEXT("Com_Pensive_Physical_Orb_Attack"),
			TEXT("Orb_Attack"),
			TEXT("Prototype_Component_Pensive_Orb_Attack"),
			&m_StateMachineDesc)))
			throw("Failed Add Component_Pensive_Physical_Orb_Attack");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = {};
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}
	m_pStateContext->Set_StateMachine(TEXT("Appearence"));
	return S_OK;
}

HRESULT CPensive::Make_Notifies()
{
	function<void()> Func = [&] {(*this).Attack_Ground(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Ground_Crack"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Orb(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("OrbCreate"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Next_Orb(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("ShootOrb"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPensive::Add_Magic()
{
	CMagic::MAGICDESC magicInitDesc;
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PENSIVE_GROUND_BALL;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = false;
		magicInitDesc.fLifeTime = 0.2f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PENSIVE_FAIL_BALL;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = false;
		magicInitDesc.fLifeTime = 0.6f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}
	{
		m_ProtegoInitDesc[0].eBuffType = BUFF_NONE;
		m_ProtegoInitDesc[0].eMagicGroup = CMagic::MG_POWER;
		m_ProtegoInitDesc[0].eMagicType = CMagic::MT_RED;
		m_ProtegoInitDesc[0].eMagicTag = PROTEGO;
		m_ProtegoInitDesc[0].fInitCoolTime = 0.f;
		m_ProtegoInitDesc[0].iDamage = 0;
		m_ProtegoInitDesc[0].isChase = false;
		m_ProtegoInitDesc[0].fLifeTime = 4.f;
	}
	{
		m_ProtegoInitDesc[1].eBuffType = BUFF_NONE;
		m_ProtegoInitDesc[1].eMagicGroup = CMagic::MG_POWER;
		m_ProtegoInitDesc[1].eMagicType = CMagic::MT_YELLOW;
		m_ProtegoInitDesc[1].eMagicTag = PROTEGO;
		m_ProtegoInitDesc[1].fInitCoolTime = 0.f;
		m_ProtegoInitDesc[1].iDamage = 0;
		m_ProtegoInitDesc[1].isChase = false;
		m_ProtegoInitDesc[1].fLifeTime = 4.f;
	}
	{
		m_ProtegoInitDesc[2].eBuffType = BUFF_NONE;
		m_ProtegoInitDesc[2].eMagicGroup = CMagic::MG_POWER;
		m_ProtegoInitDesc[2].eMagicType = CMagic::MT_PURPLE;
		m_ProtegoInitDesc[2].eMagicTag = PROTEGO;
		m_ProtegoInitDesc[2].fInitCoolTime = 0.f;
		m_ProtegoInitDesc[2].iDamage = 0;
		m_ProtegoInitDesc[2].isChase = false;
		m_ProtegoInitDesc[2].fLifeTime = 4.f;
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, PENSIVE_GROUND_BALL);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, PENSIVE_FAIL_BALL);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(2, PROTEGO);
	return S_OK;
}

void CPensive::Attack_Ground()
{
	m_pMagicSlot->Action_Magic_Skill(0, m_pPlayer, m_pDragonHead[0], COLLISIONFLAG(COL_PLAYER | COL_SHIELD));
	_float3 vAxis = _float3(1, 1, 1);

	vAxis.Normalize();
	BEGININSTANCE;
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_TYPE)0,
		(CCamera_Manager::SHAKE_AXIS)3,
		(CEase::EASE)0,
		20,
		0.6f,
		0.2f,
		(CCamera_Manager::SHAKE_POWER)1,
		vAxis);
	ENDINSTANCE;
}

void CPensive::Attack_Orb()
{
	m_pMagicBall_Fail = m_pMagicSlot->Action_Magic_Skill(1, m_pPlayer, m_pDragonHead[1], COLLISIONFLAG(COL_PLAYER | COL_SHIELD | COL_STATIC));
	m_pMagicSlot->Add_Magics(m_ProtegoInitDesc[rand()%3]);
	m_pMagicBall_Protego = m_pMagicSlot->Action_Magic_Skill(2, m_pDragonHead[1], m_pDragonHead[1], COLLISIONFLAG(COL_MAGIC));
}

void CPensive::Next_Orb()
{
	if (m_pMagicBall_Fail != nullptr)
	{
		m_pMagicBall_Fail->Do_MagicBallState_To_Next();
	}
}

HRESULT CPensive::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
			throw TEXT("Com_MagicSlot");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 10000;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* Com_RigidBody */
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
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_NPC_RANGE | COL_MAGIC | COL_STATIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::BOSS;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("999"));
		lstrcpy(Desc.wszObjectName, TEXT("√÷∞≠ ∞Ò∑Ω"));

		BEGININSTANCE;
		m_pUI_HP = dynamic_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components : ");
		wstrErrorMSG += pErrorTag;
		MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPensive::Add_Components_Level(_uint iCurrentLevelIndex)
{
	/* For.Com_Model */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Pensive"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		wstring wstrErrorMSG = TEXT("[CDugbog] Failed Add_Components_Level : ");
		wstrErrorMSG += TEXT("Com_Model");
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}
	
	/*if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_Pensive_Flail"),
		TEXT("Com_Mace"), reinterpret_cast<CComponent**>(&m_pWeapon[0]), &ParentMatrixDesc)))
	{
		wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
		wstrErrorMSG += TEXT("Com_Mace");
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Weapon_Pensive_Sword"),
		TEXT("Com_Sword"), reinterpret_cast<CComponent**>(&m_pWeapon[1]), &ParentMatrixDesc)))
	{
		wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
		wstrErrorMSG += TEXT("Com_Sword");
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}*/

	/* For.Com_StateContext */
	if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_StateContext_Enemy"),
		TEXT("Com_StateContext"), reinterpret_cast<CComponent**>(&m_pStateContext))))
	{
		wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
		wstrErrorMSG += TEXT("Com_StateContext");
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();
		return E_FAIL;
	}
	/* For.Com_DragonHead */
	{
		const CBone* pBone = m_pModelCom->Get_Bone_Index(113);
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Dragon_Head::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (nullptr == pBone)
		{
			wstring wstrErrorMSG = TEXT("pBone is nullptr");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Dragon_Head"),
			TEXT("Com_DragonHead01"), reinterpret_cast<CComponent**>(&m_pDragonHead[0]), &ParentMatrixDesc)))
		{
			wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
			wstrErrorMSG += TEXT("Com_DragonHead01");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}
	}
	{
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Dragon_Head::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (nullptr == pBone)
		{
			wstring wstrErrorMSG = TEXT("pBone is nullptr");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Dragon_Head"),
			TEXT("Com_DragonHead02"), reinterpret_cast<CComponent**>(&m_pDragonHead[1]), &ParentMatrixDesc)))
		{
			wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
			wstrErrorMSG += TEXT("Com_DragonHead02");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}
	}
	{
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("SKT_RightHand"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Dragon_Head::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (nullptr == pBone)
		{
			wstring wstrErrorMSG = TEXT("pBone is nullptr");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Dragon_Head"),
			TEXT("Com_DragonHead03"), reinterpret_cast<CComponent**>(&m_pDragonHead[2]), &ParentMatrixDesc)))
		{
			wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
			wstrErrorMSG += TEXT("Com_DragonHead03");
			MSG_BOX(wstrErrorMSG.c_str());
			__debugbreak();
			return E_FAIL;
		}
	}
	
	return S_OK;
}

CPensive* CPensive::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPensive* pInstance = New CPensive(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPensive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPensive::Clone(void* pArg)
{
	CPensive* pInstance = New CPensive(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPensive::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pDragonHead[0]);
		Safe_Release(m_pDragonHead[1]);
		Safe_Release(m_pDragonHead[2]);

		Safe_Release(m_pStateContext);
		Safe_Release(m_pMagicSlot);

		Safe_Release(m_StateMachineDesc.pOwnerModel);
		Safe_Release(m_StateMachineDesc.pTarget);
	}
}
