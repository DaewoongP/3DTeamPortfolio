#include "ConjuredDragon.h"

#include "Client_GameInstance_Functions.h"

#include "Wait.h"
#include "Death.h"
#include "LookAt.h"
#include "Action.h"
#include "Selector.h"
#include "Sequence.h"
#include "MagicBall.h"
#include "RigidMove.h"
#include "Check_Degree.h"
#include "Check_Distance.h"
#include "Sequence_Attack.h"
#include "Selector_Degree.h"
#include "UI_Group_Enemy_HP.h"

CConjuredDragon::CConjuredDragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CConjuredDragon::CConjuredDragon(const CConjuredDragon& rhs)
	: CEnemy(rhs)
{
}

HRESULT CConjuredDragon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConjuredDragon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == m_pTarget)
		m_pTarget = m_pPlayer;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CConjuredDragon::Initialize_Level(_uint iCurrentLevelIndex)
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

void CConjuredDragon::Tick(_float fTimeDelta)
{
	/* 행동 테스트용 코드 입네다 */
	BEGININSTANCE;
	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_PRESSING))
	{
		if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN))
			m_isBreakInvincible = true;
		if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN))
			m_isSpawnPhaseTwo = true;
		if (pGameInstance->Get_DIKeyState(DIK_5, CInput_Device::KEY_DOWN))
			m_isSpawn = true;
	}
	ENDINSTANCE;
	/* ========================= */

	if (true == m_isPhaseTwo &&
		false == m_isSpawnPhaseTwo)
	{
		_float3 vPosition = _float3(-17.f, -40.f, 220.f);
		m_pTransform->Set_Position(vPosition);
		m_pModelCom->Change_Animation(TEXT("Hover_Loop"));
		return;
	}

	__super::Tick(fTimeDelta);

	Update_Invincible(fTimeDelta);
	Check_Air_Balance(fTimeDelta);
	Check_Phase();

	if (nullptr != m_pRootBehavior)
		m_pRootBehavior->Tick(fTimeDelta);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CConjuredDragon::Late_Tick(_float fTimeDelta)
{
	if (true == m_isPhaseTwo &&
		false == m_isSpawnPhaseTwo)
		return;

	CGameObject::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);
#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}
}

void CConjuredDragon::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		if (true == m_isInvincible)
			return;

		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		auto Action = pCollisionMagicBallDesc->Action;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		m_pHealth->Damaged(iDamage);

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() &&
			true == IsDebuff(eBuff))
		{
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
		auto iter = m_RangeInEnemies.find(wstrObjectTag);
		if (iter == m_RangeInEnemies.end())
			m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	}
}

void CConjuredDragon::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
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

HRESULT CConjuredDragon::Render()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (0 == i || 2 == i)
			continue;

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
			wstring wstrErrorMSG = TEXT("[CEnemy] Failed Render : ");
			wstrErrorMSG += pErrorTag;
			MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);

			return E_FAIL;
		}
	}

	return S_OK;
}

void CConjuredDragon::Update_Invincible(const _float& fTimeDelta)
{
	if (true == m_isInvincible)
		return;

	m_fInvincibleGauge += fTimeDelta * 7.f;
	m_fInvincibleGauge = (m_fInvincibleGauge > 100.f) ? 100.f : m_fInvincibleGauge;
}

void CConjuredDragon::DeathBehavior(const _float& fTimeDelta)
{
	m_isDead = true;

	m_fDeadTimeAcc += fTimeDelta;
	if (9.f < m_fDeadTimeAcc)
		Set_ObjEvent(OBJ_DEAD);
}

void CConjuredDragon::Check_Air_Balance(const _float& fTimeDelta)
{
	if (false == m_isSettingBalance)
		return;

	_float3 vTargetPosition = m_pTarget->Get_Transform()->Get_Position();
	_float3 vPosition = m_pTransform->Get_Position();
	_float fTargetDistance = _float3::Distance(vPosition, vTargetPosition);

	if (30.f < fTargetDistance)
	{
		_float3 vDirection = vTargetPosition - vPosition;
		vDirection = _float3(vDirection.x, 0.f, vDirection.z);
		vDirection.Normalize();
		m_pRigidBody->Add_Force(vDirection * 3.f);
	}
	else
	{
		_float3 vCurrentVelocity = m_pRigidBody->Get_Current_Velocity();
		if (0.07f > vCurrentVelocity.Length())
			return;

		m_pRigidBody->Add_Force(-vCurrentVelocity * 3.f);
	}
}

void CConjuredDragon::Check_Phase()
{
	if (true == m_isPhaseOne)
	{
		if (0.5f >= m_pHealth->Get_Current_HP_Percent())
		{
			m_pHealth->Set_HP(m_pHealth->Get_MaxHP() / 2);
			m_isPhaseOne = false;
			m_isInvincible = true;
		}
	}
}

HRESULT CConjuredDragon::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		if (FAILED(m_pRootBehavior->Add_Type("fInvincibleGauge", &m_fInvincibleGauge)))
			throw TEXT("Failed Add_Type fInvincibleGauge");
		if (FAILED(m_pRootBehavior->Add_Type("isInvincible", &m_isInvincible)))
			throw TEXT("Failed Add_Type isInvincible");
		if (FAILED(m_pRootBehavior->Add_Type("isBreakInvincible", &m_isBreakInvincible)))
			throw TEXT("Failed Add_Type isBreakInvincible");
		if (FAILED(m_pRootBehavior->Add_Type("isSettingBalance", &m_isSettingBalance)))
			throw TEXT("Failed Add_Type isSettingBalance");
		if (FAILED(m_pRootBehavior->Add_Type("isMoveLeft", &m_isMoveLeft)))
			throw TEXT("Failed Add_Type isMoveLeft");
		if (FAILED(m_pRootBehavior->Add_Type("isPhaseOne", &m_isPhaseOne)))
			throw TEXT("Failed Add_Type isPhaseOne");
		if (FAILED(m_pRootBehavior->Add_Type("isPhaseTwo", &m_isPhaseTwo)))
			throw TEXT("Failed Add_Type isPhaseTwo");
		if (FAILED(m_pRootBehavior->Add_Type("isSpawnPhaseTwo", &m_isSpawnPhaseTwo)))
			throw TEXT("Failed Add_Type isSpawnPhaseTwo");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

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

				return true == *pIsSpawn;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

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
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_AI : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Notifies()
{
	function<void()> Func = [&] { this->Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CConjuredDragon::Add_Components()
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
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 1.5f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxSphereGeometry pSphereGeometry = PxSphereGeometry(1.5f);
		RigidBodyDesc.pGeometry = &pSphereGeometry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_MAGIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::BOSS;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("99"));
		lstrcpy(Desc.wszObjectName, TEXT("Bone Dragon"));

		BEGININSTANCE;
		m_pUI_HP = dynamic_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CConjuredDragon::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_ConjuredDragon"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}


	return S_OK;
}

HRESULT CConjuredDragon::Bind_HitMatrices()
{
	return S_OK;
}

HRESULT CConjuredDragon::Make_Death(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Death = nullptr;
		if (FAILED(Create_Behavior(pAction_Death)))
			throw TEXT("Failed Create_Behavior pAction_Death");

		CDeath* pTsk_Death = nullptr;
		if (FAILED(Create_Behavior(pTsk_Death)))
			throw TEXT("Failed Create_Behavior pTsk_Death");

		// Set Decorators
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return pHealth->isDead();
			});

		// Set Options 
		function<void(const _float&)> Func = [&](const _float& fTimeDelta) {this->DeathBehavior(fTimeDelta); };
		pTsk_Death->Set_DeathFunction(Func);
		pAction_Death->Set_Options(TEXT("Death"), m_pModelCom);

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Death"), pAction_Death)))
			throw TEXT("Failed Assemble_Behavior Action_Death");

		if (FAILED(pAction_Death->Assemble_Behavior(TEXT("Tsk_Death"), pTsk_Death)))
			throw TEXT("Failed Assemble_Behavior Tsk_Death");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Alive(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behavior */
		CSequence* pSequence_Next_Phase = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Next_Phase)))
			throw TEXT("Failed Create_Behavior pSequence_Next_Phase");
		CSequence* pSequence_Start_Phase_Two = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Start_Phase_Two)))
			throw TEXT("Failed Create_Behavior pSequence_Start_Phase_Two");
		CSelector* pSelector_Ground_Pattern = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Ground_Pattern)))
			throw TEXT("Failed Create_Behavior pSelector_Ground_Pattern");
		CSelector* pSelector_Air_Pattern = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Air_Pattern)))
			throw TEXT("Failed Create_Behavior pSelector_Air_Pattern");

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
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Next_Phase"), pSequence_Next_Phase)))
			throw TEXT("Failed Assemble_Behavior Sequence_Next_Phase");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Start_Phase_Two"), pSequence_Start_Phase_Two)))
			throw TEXT("Failed Assemble_Behavior Sequence_Start_Phase_Two");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Ground_Pattern"), pSelector_Ground_Pattern)))
			throw TEXT("Failed Assemble_Behavior Selector_Ground_Pattern");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Air_Pattern"), pSelector_Air_Pattern)))
			throw TEXT("Failed Assemble_Behavior Selector_Air_Pattern");

		if (FAILED(Make_Next_Phase(pSequence_Next_Phase)))
			throw TEXT("Failed Make_Next_Phase");
		if (FAILED(Make_Start_Phase_Two(pSequence_Start_Phase_Two)))
			throw TEXT("Failed Make_Start_Phase_Two");
		if (FAILED(Make_Ground_Pattern(pSelector_Ground_Pattern)))
			throw TEXT("Failed Make_Ground_Pattern");
		if (FAILED(Make_Air_Pattern(pSelector_Air_Pattern)))
			throw TEXT("Failed Make_Air_Pattern");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Death : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Next_Phase(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Enter_Next_Phase = { nullptr };
		if (FAILED(Create_Behavior(pAction_Enter_Next_Phase)))
			throw TEXT("Failed Create_Behavior pAction_Enter_Next_Phase");
		CAction* pAction_Fly = { nullptr };
		if (FAILED(Create_Behavior(pAction_Fly)))
			throw TEXT("Failed Create_Behavior pAction_Fly");

		CRigidMove* pRigidMove = { nullptr };
		if (FAILED(Create_Behavior(pRigidMove)))
			throw TEXT("Failed Create_Behavior pRigidMove");

		/* Set Decorators */
		pAction_Enter_Next_Phase->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseOne = { nullptr };
				_bool* pIsSpawnPhaseTwo = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseOne", pIsPhaseOne)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isSpawnPhaseTwo", pIsSpawnPhaseTwo)))
					return false;

				return false == *pIsPhaseOne && false == *pIsSpawnPhaseTwo;
			});
		pAction_Fly->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSettingBalance = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSettingBalance", pIsSettingBalance)))
					return false;

				*pIsSettingBalance = false;

				return true;
			});
		pAction_Fly->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseTwo = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseTwo", pIsPhaseTwo)))
					return false;

				*pIsPhaseTwo = true;

				return true;
			});

		/* Set Options */
		pAction_Enter_Next_Phase->Set_Options(TEXT("Fly_Next_Phase"), m_pModelCom, false, 0.f, true);
		pAction_Fly->Set_Options(TEXT("Fly_Loop"), m_pModelCom, true, 0.f, true, false);
		pRigidMove->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_LOOK, 10.f, 10.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Enter_Next_Phase"), pAction_Enter_Next_Phase)))
			throw TEXT("Failed Assemble_Behavior Action_Enter_Next_Phase");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pAction_Fly"), pAction_Fly)))
			throw TEXT("Failed Assemble_Behavior pAction_Fly");

		if (FAILED(pAction_Fly->Assemble_Behavior(TEXT("RigidMove"), pRigidMove)))
			throw TEXT("Failed Assemble_Behavior RigidMove");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Next_Phase : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Start_Phase_Two(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Fly_Down = { nullptr };
		if (FAILED(Create_Behavior(pAction_Fly_Down)))
			throw TEXT("Failed Create_Behavior pAction_Fly_Down");
		CAction* pAction_Land_Ground = { nullptr };
		if (FAILED(Create_Behavior(pAction_Land_Ground)))
			throw TEXT("Failed Create_Behavior pAction_Land_Ground");

		CWait* pTsk_Wait = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseTwo = { nullptr };
				_bool* pIsSpawnPhaseTwo = { nullptr };
				CRigidBody* pRigidBody = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseTwo", pIsPhaseTwo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isSpawnPhaseTwo", pIsSpawnPhaseTwo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
					return false;

				if (true == *pIsPhaseTwo && true == *pIsSpawnPhaseTwo)
				{
					pRigidBody->Set_Gravity(true);
					pRigidBody->Set_CollisionFlag("Enemy_Body", COL_MAGIC | COL_STATIC);
					return true;
				}

				return false;
			});
		pSequence->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isInvincible", pIsInvincible)))
					return false;

				*pIsInvincible = false;

				return true;
			});

		/* Set Options */
		pAction_Fly_Down->Set_Options(TEXT("Fly_Loop"), m_pModelCom, true, 0.f, true);
		pAction_Land_Ground->Set_Options(TEXT("Landing_To_Cmbt"), m_pModelCom);
		pTsk_Wait->Set_Timer(2.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Fly_Down"), pAction_Fly_Down)))
			throw TEXT("Failed Assemble_Behavior Action_Fly_Down");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("pction_Land_Ground"), pAction_Land_Ground)))
			throw TEXT("Failed Assemble_Behavior Action_Land_Ground");

		if (FAILED(pAction_Fly_Down->Assemble_Behavior(TEXT("pTsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior pTsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Start_Phase_Two : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Pattern(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Create Child Behaviors */
		CSequence* pSequence_Turns = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Turns)))
			throw TEXT("Failed Create_Behavior pSequence_Turns");
		CSequence* pSequence_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attacks)))
			throw TEXT("Failed Create_Behavior pSequence_Attacks");
		CSequence* pSequence_Charge = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Charge)))
			throw TEXT("Failed Create_Behavior pSequence_Charge");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return 0.5f >= pHealth->Get_Current_HP_Percent();
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Turns"), pSequence_Turns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Turns");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Attacks"), pSequence_Attacks)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attacks");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Charge"), pSequence_Charge)))
			throw TEXT("Failed Assemble_Behavior Sequence_Charge");

		if (FAILED(Make_Ground_Turns(pSequence_Turns)))
			throw TEXT("Failed Make_Ground_Turns");
		if (FAILED(Make_Ground_Attacks(pSequence_Attacks)))
			throw TEXT("Failed Make_Ground_Attacks");
		if (FAILED(Make_Ground_Charge(pSequence_Charge)))
			throw TEXT("Failed Make_Ground_Charge");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Pattern : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Pattern(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Create Child Behaviors */
		CSequence* pSequence_Break_Invincible = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Break_Invincible)))
			throw TEXT("Failed Create_Behavior pSequence_Break_Invincible");
		CSequence* pSequence_Change_Invincible = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Change_Invincible)))
			throw TEXT("Failed Create_Behavior pSequence_Change_Invincible");
		CSequence* pSequence_Patterns = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Patterns)))
			throw TEXT("Failed Create_Behavior pSequence_Patterns");

		CRandomChoose* pRandom_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");
		CRandomChoose* pRandom_Idle_Breaks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Idle_Breaks)))
			throw TEXT("Failed Create_Behavior pRandom_Idle_Breaks");
		CAction* pAction_Hover_Loop = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Hover_Loop");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CHealth* pHealth = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pHealth", pHealth)))
					return false;

				return 0.5f < pHealth->Get_Current_HP_Percent();
			});
		pSequence_Patterns->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsBreakInvicible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isBreakInvincible", pIsBreakInvicible)))
					return false;

				return !(*pIsBreakInvicible);
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Break_Invincible"), pSequence_Break_Invincible)))
			throw TEXT("Failed Assemble_Behavior Sequence_Break_Invincible");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Change_Invincible"), pSequence_Change_Invincible)))
			throw TEXT("Failed Assemble_Behavior Sequence_Change_Invincible");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Patterns"), pSequence_Patterns)))
			throw TEXT("Failed Assemble_Behavior Sequence_Patterns");

		if (FAILED(pSequence_Patterns->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");
		if (FAILED(pSequence_Patterns->Assemble_Behavior(TEXT("Random_Idle_Breaks"), pRandom_Idle_Breaks)))
			throw TEXT("Failed Assemble_Behavior Random_Idle_Breaks");
		if (FAILED(pSequence_Patterns->Assemble_Behavior(TEXT("Action_Hover_Loop"), pAction_Hover_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Hover_Loop");

		if (FAILED(Make_Air_Break_Invincible(pSequence_Break_Invincible)))
			throw TEXT("Failed Make_Air_Break_Invincible");
		if (FAILED(Make_Air_Change_Invincible(pSequence_Change_Invincible)))
			throw TEXT("Failed Make_Air_Change_Invincible");
		if (FAILED(Make_Air_Attacks(pRandom_Attacks)))
			throw TEXT("Failed Make_Air_Attacks");
		if (FAILED(Make_Air_Idle_Breaks(pRandom_Idle_Breaks)))
			throw TEXT("Failed Make_Air_Idle_Breaks");
		if (FAILED(Make_Air_Hover(pAction_Hover_Loop)))
			throw TEXT("Failed Make_Air_Hover");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Air_Pattern : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Turns(_Inout_ CSequence* pSequence)
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
		CAction* pAction_Right_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Right_180)))
			throw TEXT("Failed Create_Behavior pAction_Right_180");
		CAction* pAction_Left_180 = nullptr;
		if (FAILED(Create_Behavior(pAction_Left_180)))
			throw TEXT("Failed Create_Behavior pAction_Left_180");
		/* Set Decorations */

		/* Set Options */
		pAction_Left_45->Set_Options(TEXT("Ground_Turn_Left_45"), m_pModelCom);
		pAction_Right_45->Set_Options(TEXT("Ground_Turn_Right_45"), m_pModelCom);
		pAction_Left_90->Set_Options(TEXT("Ground_Turn_Left_90"), m_pModelCom);
		pAction_Right_90->Set_Options(TEXT("Ground_Turn_Right_90"), m_pModelCom);
		pAction_Left_135->Set_Options(TEXT("Ground_Turn_Left_180"), m_pModelCom);
		pAction_Right_135->Set_Options(TEXT("Ground_Turn_Right_180"), m_pModelCom);
		pAction_Left_180->Set_Options(TEXT("Ground_Turn_Left_180"), m_pModelCom);
		pAction_Right_180->Set_Options(TEXT("Ground_Turn_Right_180"), m_pModelCom);

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
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::LEFT_BACK, pAction_Left_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree LEFT_BACK");
		if (FAILED(pSelector_Degree->Assemble_Behavior(CSelector_Degree::RIGHT_BACK, pAction_Right_180)))
			throw TEXT("Failed Assemble_Behavior pSelector_Degree RIGHT_BACK");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Turns : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Attacks(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CCheck_Distance* pCheck_Distance = { nullptr };
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CRandomChoose* pRandom_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");

		CSequence* pSequence_Attack_Pulse = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Pulse)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Pulse");
		CSequence* pSequence_Attack_Range = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Range)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Range");
		CSequence* pSequence_Attack_Melee = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Melee)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Melee");

		/* Set Decorators */
		pRandom_Attacks->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float fDistance = { 0.f };
				if (FAILED(pBlackBoard->Get_Type("fTargetDistance", fDistance)))
					return false;

				return 30.f > fDistance;
			});
		pRandom_Attacks->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Pulse"), pSequence_Attack_Pulse, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Pulse");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Range"), pSequence_Attack_Range, 0.8f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Range");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Melee"), pSequence_Attack_Melee, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Melee");

		if (FAILED(Make_Ground_Attack_Pulse(pSequence_Attack_Pulse)))
			throw TEXT("Failed Make_Ground_Attack_Pulse");
		if (FAILED(Make_Ground_Attacks_Range(pSequence_Attack_Range)))
			throw TEXT("Failed Make_Ground_Attacks_Range");
		if (FAILED(Make_Ground_Attacks_Melee(pSequence_Attack_Melee)))
			throw TEXT("Failed Make_Ground_Attacks_Melee");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Attacks : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Attacks_Melee(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CCheck_Distance* pCheck_Distance = { nullptr };
		if (FAILED(Create_Behavior(pCheck_Distance)))
			throw TEXT("Failed Create_Behavior pCheck_Distance");
		CRandomChoose* pRandom_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");

		CSequence_Attack* pSequence_Attack_Swipe_Left = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Swipe_Left)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Swipe_Left");
		CSequence_Attack* pSequence_Attack_Swipe_Right = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Swipe_Right)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Swipe_Right");
		CSequence_Attack* pSequence_Attack_Tail_360 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Tail_360)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Tail_360");

		/* Set Decorators */
		pRandom_Attacks->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pCheck_Distance->Set_Option(m_pTransform);
		pSequence_Attack_Swipe_Left->Set_Attack_Action_Options(TEXT("Ground_Attack_Swipe_Left"), m_pModelCom);
		pSequence_Attack_Swipe_Left->Set_Attack_Option(15.f);
		pSequence_Attack_Swipe_Right->Set_Attack_Action_Options(TEXT("Ground_Attack_Swipe_Right"), m_pModelCom);
		pSequence_Attack_Swipe_Right->Set_Attack_Option(15.f);
		pSequence_Attack_Tail_360->Set_Attack_Action_Options(TEXT("Ground_Attack_Tail_360"), m_pModelCom);
		pSequence_Attack_Tail_360->Set_Attack_Option(15.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swipe_Left"), pSequence_Attack_Swipe_Left, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swipe_Left");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swipe_Right"), pSequence_Attack_Swipe_Right, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swipe_Right");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Tail_360"), pSequence_Attack_Tail_360, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Tail_360");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Attacks_Melee : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Attacks_Range(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CRandomChoose* pRandom_Attacks = { nullptr };
		if (FAILED(Create_Behavior(pRandom_Attacks)))
			throw TEXT("Failed Create_Behavior pRandom_Attacks");

		CSequence_Attack* pSequence_Attack_Fireball_1 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fireball_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fireball_1");
		CSequence_Attack* pSequence_Attack_Fireball_2 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fireball_2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fireball_2");
		CSequence_Attack* pSequence_Attack_Fire_Sweep_Left = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fire_Sweep_Left)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fire_Sweep_Left");
		CSequence_Attack* pSequence_Attack_Fire_Sweep_Right = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fire_Sweep_Right)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fire_Sweep_Right");
		CSequence* pSequence_Attack_Charge = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Charge)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Charge");
		CSequence_Attack* pSequence_Attack_Breath = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Breath)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Breath");

		/* Set Decorators */
		pRandom_Attacks->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pSequence_Attack_Fireball_1->Set_Attack_Action_Options(TEXT("Ground_Attack_Fireball_1"), m_pModelCom);
		pSequence_Attack_Fireball_1->Set_Attack_Option(100.f);
		pSequence_Attack_Fireball_2->Set_Attack_Action_Options(TEXT("Ground_Attack_Fireball_2"), m_pModelCom);
		pSequence_Attack_Fireball_2->Set_Attack_Option(100.f);
		pSequence_Attack_Fire_Sweep_Left->Set_Attack_Action_Options(TEXT("Ground_Attack_Fire_Sweep_Left"), m_pModelCom);
		pSequence_Attack_Fire_Sweep_Left->Set_Attack_Option(100.f);
		pSequence_Attack_Fire_Sweep_Right->Set_Attack_Action_Options(TEXT("Ground_Attack_Fire_Sweep_Right"), m_pModelCom);
		pSequence_Attack_Fire_Sweep_Right->Set_Attack_Option(100.f);
		pSequence_Attack_Breath->Set_Attack_Action_Options(TEXT("Ground_Attack_Breath"), m_pModelCom);
		pSequence_Attack_Breath->Set_Attack_Option(100.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_1"), pSequence_Attack_Fireball_1, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_1");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_2"), pSequence_Attack_Fireball_2, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_2");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fire_Sweep_Left"), pSequence_Attack_Fire_Sweep_Left, 0.15f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fire_Sweep_Left");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fire_Sweep_Right"), pSequence_Attack_Fire_Sweep_Right, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fire_Sweep_Right");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSequence_Attack_Charge, 0.05f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("pSequence_Attack_Breath"), pSequence_Attack_Breath, 0.2f)))
			throw TEXT("Failed Assemble_Behavior pSequence_Attack_Breath");

		if (FAILED(Make_Ground_Charge(pSequence_Attack_Charge)))
			throw TEXT("Failed Make_Ground_Charge");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Attacks_Range : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Attack_Pulse(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Ground_To_Fly = { nullptr };
		if (FAILED(Create_Behavior(pAction_Ground_To_Fly)))
			throw TEXT("Failed Create_Behavior pAction_Ground_To_Fly");
		CSequence_Attack* pSequence_Attack_Purse = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Purse)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Purse");
		CAction* pAction_Land = { nullptr };
		if (FAILED(Create_Behavior(pAction_Land)))
			throw TEXT("Failed Create_Behavior pAction_Land");

		/* Set Options */
		pAction_Ground_To_Fly->Set_Options(TEXT("Ground_To_Fly"), m_pModelCom);
		pSequence_Attack_Purse->Set_Attack_Action_Options(TEXT("Attack_Pulse"), m_pModelCom);
		pSequence_Attack_Purse->Set_Attack_Option(100.f);
		pAction_Land->Set_Options(TEXT("Landing_To_Cmbt"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Ground_To_Fly"), pAction_Ground_To_Fly)))
			throw TEXT("Failed Assemble_Behavior Action_Ground_To_Fly");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_Purse"), pSequence_Attack_Purse)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Purse");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Land"), pAction_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Land");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Attack_Pulse : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Ground_Charge(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Charge_Enter = { nullptr };
		if (FAILED(Create_Behavior(pAction_Charge_Enter)))
			throw TEXT("Failed Create_Behavior pAction_Charge_Enter");
		CAction* pAction_Charge_Loop = { nullptr };
		if (FAILED(Create_Behavior(pAction_Charge_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Charge_Loop");
		CAction* pAction_Charge_Exit = { nullptr };
		if (FAILED(Create_Behavior(pAction_Charge_Exit)))
			throw TEXT("Failed Create_Behavior pAction_Charge_Exit");

		CWait* pTsk_Wait = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Options */
		pAction_Charge_Enter->Set_Options(TEXT("Ground_Charge_Enter"), m_pModelCom);
		pAction_Charge_Loop->Set_Options(TEXT("Ground_Charge_Loop"), m_pModelCom, true, 0.f, false, false);
		pAction_Charge_Exit->Set_Options(TEXT("Ground_Charge_End"), m_pModelCom);
		pTsk_Wait->Set_Timer(0.2f);  

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_Enter"), pAction_Charge_Enter)))
			throw TEXT("Failed Assemble_Behavior Action_Charge_Enter");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_Loop"), pAction_Charge_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Charge_Loop");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Charge_Exit"), pAction_Charge_Exit)))
			throw TEXT("Failed Assemble_Behavior Action_Charge_Exit");

		if (FAILED(pAction_Charge_Loop->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Charge : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Break_Invincible(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Break_Invinclble = { nullptr };
		if (FAILED(Create_Behavior(pAction_Break_Invinclble)))
			throw TEXT("Failed Create_Behavior pAction_Break_Invinclble");
		CAction* pAction_Hover_Loop = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Hover_Loop");

		CLookAt* pTsk_LookAt = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsBreakInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isBreakInvincible", pIsBreakInvincible)))
					return false;

				return true == *pIsBreakInvincible;
			});
		pSequence->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float* pInvincibleGauge = { nullptr };
				_bool* pIsInvincible = { nullptr };
				_bool* pIsBreakInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("fInvincibleGauge", pInvincibleGauge)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isInvincible", pIsInvincible)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isBreakInvincible", pIsBreakInvincible)))
					return false;

				*pInvincibleGauge = 0.f;
				*pIsInvincible = false;
				*pIsBreakInvincible = false;

				return true;
			});

		/* Set Options */
		pAction_Break_Invinclble->Set_Options(TEXT("Break_Invincible"), m_pModelCom);
		pTsk_LookAt->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Break_Invinclble"), pAction_Break_Invinclble)))
			throw TEXT("Failed Assemble_Behavior Action_Break_Invinclble");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Hover_Loop"), pAction_Hover_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Hover_Loop");

		if (FAILED(pAction_Break_Invinclble->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");
		if (FAILED(Make_Air_Hover(pAction_Hover_Loop)))
			throw TEXT("Failed Make_Air_Hover");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Change_Invincible : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Change_Invincible(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Change_Invinclble = { nullptr };
		if (FAILED(Create_Behavior(pAction_Change_Invinclble)))
			throw TEXT("Failed Create_Behavior pAction_Change_Invinclble");
		CAction* pAction_Hover_Loop = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover_Loop)))
			throw TEXT("Failed Create_Behavior pAction_Hover_Loop");

		CLookAt* pTsk_LookAt = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt");

		/* Set Decorators */
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float* pInvincibleGauge = { nullptr };
				_bool* pIsInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("fInvincibleGauge", pInvincibleGauge)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isInvincible", pIsInvincible)))
					return false;

				return (100.f <= *pInvincibleGauge) && false == *pIsInvincible;
			});
		pSequence->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_float* pInvincibleGauge = { nullptr };
				_bool* pIsInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("fInvincibleGauge", pInvincibleGauge)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isInvincible", pIsInvincible)))
					return false;

				*pInvincibleGauge = 0.f;
				*pIsInvincible = true;

				return true;
			});

		/* Set Options */
		pAction_Change_Invinclble->Set_Options(TEXT("Fire_Burst_Recovery"), m_pModelCom);
		pTsk_LookAt->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Change_Invinclble"), pAction_Change_Invinclble)))
			throw TEXT("Failed Assemble_Behavior Action_Change_Invinclble");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Hover_Loop"), pAction_Hover_Loop)))
			throw TEXT("Failed Assemble_Behavior Action_Hover_Loop");

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");
		if (FAILED(Make_Air_Hover(pAction_Hover_Loop)))
			throw TEXT("Failed Make_Air_Hover");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Change_Invincible : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Attacks(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		/* Create Child Behaviors */
		CSequence* pSequence_Attack_Fireball = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fireball)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fireball");
		CSequence_Attack* pSequence_Attack_Purse = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Purse)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Purse");
		CSequence_Attack* pSequence_Attack_Breath = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Breath)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Breath");

		/* Set Decorators */
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pSequence_Attack_Purse->Set_Attack_Action_Options(TEXT("Attack_Pulse"), m_pModelCom);
		pSequence_Attack_Purse->Set_Attack_Option(100.f);
		pSequence_Attack_Breath->Set_Attack_Action_Options(TEXT("Attack_Fire_Sweep_Breath"), m_pModelCom);
		pSequence_Attack_Breath->Set_Attack_Option(100.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Fireball"), pSequence_Attack_Fireball, 0.4f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Breath"), pSequence_Attack_Breath, 0.35f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Breath");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Purse"), pSequence_Attack_Purse, 0.25f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Purse");

		if (FAILED(Make_Attack_Fireball(pSequence_Attack_Fireball)))
			throw TEXT("Failed Make_Attack_Fireball");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Air_Attacks : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Attack_Fireball(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		/* Create Child Behaviors */
		CSequence_Attack* pSequence_Attack_Fireball_1 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fireball_1)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fireball_1");
		CSequence_Attack* pSequence_Attack_Fireball_2 = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Fireball_2)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Fireball_2");

		/* Set Options */
		pSequence_Attack_Fireball_1->Set_Attack_Action_Options(TEXT("Air_Attack_Fireball_2"), m_pModelCom);
		pSequence_Attack_Fireball_1->Set_Attack_Option(100.f);
		pSequence_Attack_Fireball_2->Set_Attack_Action_Options(TEXT("Air_Attack_Fireball_2"), m_pModelCom);
		pSequence_Attack_Fireball_2->Set_Attack_Option(100.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_1"), pSequence_Attack_Fireball_1)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_2"), pSequence_Attack_Fireball_2)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_2");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Attack_Fireball : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Idle_Breaks(_Inout_ CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Idle_Break_1 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Idle_Break_1)))
			throw TEXT("Failed Create_Behavior pAction_Idle_Break_1");
		CAction* pAction_Idle_Break_2 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Idle_Break_2)))
			throw TEXT("Failed Create_Behavior pAction_Idle_Break_2");
		CSelector* pSelector_Dash_LR = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Dash_LR)))
			throw TEXT("Failed Create_Behavior pSelector_Dash_LR");

		CAction* pAction_Hover_Dash_Left = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover_Dash_Left)))
			throw TEXT("Failed Create_Behavior pAction_Hover_Dash_Left");
		CAction* pAction_Hover_Dash_Right = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover_Dash_Right)))
			throw TEXT("Failed Create_Behavior pAction_Hover_Dash_Right");

		CLookAt* pTsk_LookAt_1 = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt_1)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_1");
		CLookAt* pTsk_LookAt_2 = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt_2)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_2");
		CLookAt* pTsk_LookAt_3 = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt_3)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_3");
		CLookAt* pTsk_LookAt_4 = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt_4)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt_4");
		CRigidMove* pTsk_RigidMove_Left = { nullptr };
		if (FAILED(Create_Behavior(pTsk_RigidMove_Left)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove_Left");
		CRigidMove* pTsk_RigidMove_Right = { nullptr };
		if (FAILED(Create_Behavior(pTsk_RigidMove_Right)))
			throw TEXT("Failed Create_Behavior pTsk_RigidMove_Right");

		/* Set Decorators */
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});
		pAction_Hover_Dash_Left->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSettingBalance = { nullptr };
				_bool* pIsMoveLeft = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSettingBalance", pIsSettingBalance)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isMoveLeft", pIsMoveLeft)))
					return false;

				*pIsSettingBalance = false;

				return false == *pIsMoveLeft;
			});
		pAction_Hover_Dash_Left->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSettingBalance = { nullptr };
				_bool* pIsMoveLeft = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSettingBalance", pIsSettingBalance)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isMoveLeft", pIsMoveLeft)))
					return false;

				*pIsSettingBalance = true;
				*pIsMoveLeft = true;

				return true;
			});
		pAction_Hover_Dash_Right->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSettingBalance = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSettingBalance", pIsSettingBalance)))
					return false;

				*pIsSettingBalance = false;

				return true;
			});
		pAction_Hover_Dash_Right->Add_End_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSettingBalance = { nullptr };
				_bool* pIsMoveLeft = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSettingBalance", pIsSettingBalance)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isMoveLeft", pIsMoveLeft)))
					return false;

				*pIsSettingBalance = true;
				*pIsMoveLeft = false;

				return true;
			});

		/* Set Options */
		pTsk_LookAt_1->Set_Option(m_pTransform);
		pTsk_LookAt_2->Set_Option(m_pTransform);
		pTsk_LookAt_3->Set_Option(m_pTransform);
		pTsk_LookAt_4->Set_Option(m_pTransform);
		pAction_Idle_Break_1->Set_Options(TEXT("Hover_Idle_Break_1"), m_pModelCom);
		pAction_Idle_Break_2->Set_Options(TEXT("Hover_Idle_Break_2"), m_pModelCom);
		pAction_Hover_Dash_Left->Set_Options(TEXT("Hover_Dash_Left"), m_pModelCom);
		pAction_Hover_Dash_Right->Set_Options(TEXT("Hover_Dash_Right"), m_pModelCom);
		pTsk_RigidMove_Left->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_RIGHT, -20.f, 2.f);
		pTsk_RigidMove_Right->Set_Option(m_pRigidBody, m_pTransform, CRigidMove::DIR_RIGHT, 20.f, 2.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Idle_Break_1"), pAction_Idle_Break_1, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Idle_Break_1");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Idle_Break_2"), pAction_Idle_Break_2, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Idle_Break_2");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Selector_Dash_LR"), pSelector_Dash_LR, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Selector_Dash_LR");

		if (FAILED(pSelector_Dash_LR->Assemble_Behavior(TEXT("Action_Hover_Dash_Left"), pAction_Hover_Dash_Left)))
			throw TEXT("Failed Assemble_Behavior Action_Hover_Dash_Left");
		if (FAILED(pSelector_Dash_LR->Assemble_Behavior(TEXT("Action_Hover_Dash_Right"), pAction_Hover_Dash_Right)))
			throw TEXT("Failed Assemble_Behavior Action_Hover_Dash_Right");

		if (FAILED(pAction_Idle_Break_1->Assemble_Behavior(TEXT("Tsk_LookAt_1"), pTsk_LookAt_1)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_1");
		if (FAILED(pAction_Idle_Break_2->Assemble_Behavior(TEXT("Tsk_LookAt_2"), pTsk_LookAt_2)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_2");
		if (FAILED(pAction_Hover_Dash_Left->Assemble_Behavior(TEXT("Tsk_LookAt_3"), pTsk_LookAt_3)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_3");
		if (FAILED(pAction_Hover_Dash_Right->Assemble_Behavior(TEXT("Tsk_LookAt_4"), pTsk_LookAt_4)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt_4");
		if (FAILED(pAction_Hover_Dash_Left->Assemble_Behavior(TEXT("Tsk_RigidMove_Left"), pTsk_RigidMove_Left)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Left");
		if (FAILED(pAction_Hover_Dash_Right->Assemble_Behavior(TEXT("Tsk_RigidMove_Right"), pTsk_RigidMove_Right)))
			throw TEXT("Failed Assemble_Behavior Tsk_RigidMove_Right");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Air_Idle_Breaks : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Make_Air_Hover(_Inout_ CAction* pAction)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pAction)
			throw TEXT("Parameter pAction is nullptr");

		/* Create Child Behaviors */
		CLookAt* pTsk_LookAt = { nullptr };
		if (FAILED(Create_Behavior(pTsk_LookAt)))
			throw TEXT("Failed Create_Behavior pTsk_LookAt");
		CWait* pTsk_Wait = { nullptr };
		if (FAILED(Create_Behavior(pTsk_Wait)))
			throw TEXT("Failed Create_Behavior pTsk_Wait");

		/* Set Decorators */

		/* Set Options */
		pAction->Set_Options(TEXT("Hover_Loop"), m_pModelCom, true);
		pTsk_LookAt->Set_Option(m_pTransform);
		pTsk_Wait->Set_Timer(1.f);

		/* Assemble Behaviors */
		if (FAILED(pAction->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");
		if (FAILED(pAction->Assemble_Behavior(TEXT("Tsk_Wait"), pTsk_Wait)))
			throw TEXT("Failed Assemble_Behavior Tsk_Wait");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Air_Hover : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		ENDINSTANCE;

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CConjuredDragon::Shot_Fireball()
{
}

CConjuredDragon* CConjuredDragon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CConjuredDragon* pInstance = New CConjuredDragon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CConjuredDragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CConjuredDragon* CConjuredDragon::Clone(void* pArg)
{
	CConjuredDragon* pInstance = New CConjuredDragon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CConjuredDragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConjuredDragon::Free()
{
	__super::Free();
}
