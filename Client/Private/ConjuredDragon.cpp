#include "ConjuredDragon.h"

#include "Client_GameInstance_Functions.h"

#include "Camera_Shake.h"

#include "Wait.h"
#include "Death.h"
#include "Pulse.h"
#include "LookAt.h"
#include "Breath.h"
#include "Action.h"
#include "Selector.h"
#include "Sequence.h"
#include "MagicBall.h"
#include "RigidMove.h"
#include "MagicSlot.h"
#include "EnergyBall.h"
#include "Check_Degree.h"
#include "Breath_Effect.h"
#include "ParticleSystem.h"
#include "Check_Distance.h"
#include "Sequence_Attack.h"
#include "Selector_Degree.h"
#include "UI_Group_Enemy_HP.h"
#include "Weapon_Dragon_Head.h"
#include "Weapon_Dragon_Left_Wing.h"
#include "Weapon_Dragon_Right_Wing.h"
#include "Weapon_Dragon_Tail.h"
#include "ImpulseSphere_Effect.h"

#include "Quest_Manager.h"
#include "UI_Damage.h"

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
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_BlackSmokeIdle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_BlackSmokeIdle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/BlackSmokeIdle/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	// Pulse
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Charge")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Charge")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/Charge/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_CircleEmit")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_CircleEmit")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/CircleEmit/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Rock")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Rock")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/Rock/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/SplashWater/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_BoomWispy")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_BoomWispy")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/Pulse/BoomWispy/"), LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_ImpulseSphere_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_ImpulseSphere_Effect"),
			CImpulseSphere_Effect::Create(m_pDevice, m_pContext, LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	// Breath_Effect
	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Breath_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Breath_Effect"),
			CBreath_Effect::Create(m_pDevice, m_pContext, LEVEL_SANCTUM))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	pGameInstance->Read_CutSceneCamera(TEXT("Dragon_Enter"),TEXT("../../Resources/GameData/CutScene/Sanctum_Dragon_Enter.cut"));

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == m_pTarget)
		m_pTarget = m_pPlayer;

	m_vTargetPosition = m_pTarget->Get_Transform()->Get_Position();

	if (FAILED(Add_Components()))
		return E_FAIL;
	if (FAILED(Add_Effects()))
		return E_FAIL;
	
	m_pEffect_BlackSmokeIdle->Play(m_pTransform->Get_Position());

	m_CollisionRequestDesc.eType = ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 20;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

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

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Magics()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
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
		{
			m_isSpawn = true;
			//pGameInstance->Add_CutScene(TEXT("Dragon_Enter"));
		}
		if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN))
			;
	}
	ENDINSTANCE;
	/* ========================= */

	if (true == m_isPhaseTwo &&
		false == m_isSpawn)
	{
		_float3 vPosition = _float3(-17.f, -40.f, 220.f);
		m_pTransform->Set_Position(vPosition);
		m_pModelCom->Change_Animation(TEXT("Hover_Loop"));
		return;
	}

	Check_Phase();

	__super::Tick(fTimeDelta);

	m_pHitMatrix = m_HitMatrices[rand() % 3];
	Update_Bones();
	Update_Invincible(fTimeDelta);
	Check_Air_Balance(fTimeDelta);
	EnergyBall_PhaseOne(fTimeDelta);
	EnergyBall_PhaseFinal(fTimeDelta);
	Update_Breath(fTimeDelta);

	// 이펙트에 필요한 정보들 업데이트.
	m_vOffsetPos = m_pTransform->Get_Position();
	m_vOffsetPos.y += 10.f;
	m_pEffect_BlackSmokeIdle->Get_Transform()->Set_Position(m_vOffsetPos);

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CConjuredDragon::Late_Tick(_float fTimeDelta)
{
	if (true == m_isPhaseTwo &&
		false == m_isSpawn)
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

		Print_Damage_Font(iDamage);

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
		//3 wing 4 body
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

void CConjuredDragon::EnergyBall_PhaseOne(const _float& fTimeDelta)
{
	if (false == m_isPhaseOne ||
		false == m_isInvincible ||
		true == m_pEnergyBall->isEnable())
		return;

	m_fSpawnBallTimeAcc += fTimeDelta;
	if (3.f > m_fSpawnBallTimeAcc)
		return;

	_float3 vPosition = m_pTransform->Get_Position();
	_float3 vLook = m_pTransform->Get_Look();
	_float3 vRight = m_pTransform->Get_Right();

	vPosition += vLook * 20.f;
	vPosition += vRight * GetRandomFloat(-20.f, 20.f);
	vPosition.y += GetRandomFloat(-5.f, 5.f);

	CEnergyBall::ENERGYBALLINITDESC InitDesc;
	InitDesc.vPosition = vPosition;
	InitDesc.fActionProtegoTime = 5.f;
	InitDesc.DeathFunction = [&](const _float& fTimeDelta)->_bool { return this->Break_Invincible(fTimeDelta); };

	m_pEnergyBall->Reset(InitDesc);
	m_fSpawnBallTimeAcc = 0.f;
}

_bool CConjuredDragon::Break_Invincible(const _float& fTimeDelta)
{
	m_isBreakInvincible = true;

	return true;
}

void CConjuredDragon::EnergyBall_PhaseFinal(const _float& fTimeDelta)
{
	if (false == m_isPhaseFinal ||
		true == m_isFinish ||
		true == m_pEnergyBall->isEnable())
		return;

	m_fSpawnBallTimeAcc += fTimeDelta;
	if (3.f > m_fSpawnBallTimeAcc)
		return;

	_float3 vPosition = m_pTransform->Get_Position();
	_float3 vLook = m_pTransform->Get_Look();
	_float3 vRight = m_pTransform->Get_Right();

	_float fDirection = _float(rand() % 2 - 1);
	_float fData = 5.f * fDirection;
	vPosition += vLook * fData + vLook * GetRandomFloat(0.f, 2.f) * fData;

	fDirection = _float(rand() % 2 - 1);
	fData = 5.f * fDirection;
	vPosition += vRight * fData + vRight * GetRandomFloat(0.f, 2.f) * fData;

	vPosition.y += 5.f;

	CEnergyBall::ENERGYBALLINITDESC InitDesc;
	InitDesc.vPosition = vPosition;
	InitDesc.fActionProtegoTime = 2.f;
	InitDesc.DeathFunction = [&](const _float& fTimeDelta)->_bool { return this->UnSeal(fTimeDelta); };

	m_pEnergyBall->Reset(InitDesc);
	m_fSpawnBallTimeAcc = 0.f;
}

_bool CConjuredDragon::UnSeal(const _float& fTimeDelta)
{
	m_iDeathCount += 1;

	if (3 == m_iDeathCount)
		m_isFinish = true;

	return true;
}

void CConjuredDragon::Update_Breath(const _float& fTimeDelta)
{
	_float4x4 HeadWorldMatrix = *m_pHeadMatrix * m_pModelCom->Get_PivotFloat4x4() * m_pTransform->Get_WorldMatrix();
	m_vHeadPosition = HeadWorldMatrix.Translation();
	_float3 vLook = m_pTransform->Get_Look();
	_float3 vTargetPosition = _float3();
	_float3 vBreathPosition = _float3();
	_float3 vDirection = _float3();
	_float fRadian = { 0.f };
	_float fDistance = { 0.f };

	m_vHeadPosition += vLook;
	vTargetPosition = m_pTarget->Get_Transform()->Get_Position();
	m_vTargetPosition = _float3::Lerp(m_vTargetPosition, vTargetPosition, fTimeDelta);
	vBreathPosition = (m_vHeadPosition + m_vTargetPosition) * 0.5f;

	vDirection = m_vTargetPosition - m_vHeadPosition;
	vDirection.Normalize();

	fRadian = acosf(vDirection.Dot(_float3(0.f, 1.f, 0.f)));
	fDistance = _float3::Distance(m_vHeadPosition, m_vTargetPosition) * 0.4f;

	CBreath::BREATHINITDESC BreathInitDesc;
	BreathInitDesc.vPosition = vBreathPosition;
	BreathInitDesc.vRight = m_pTransform->Get_Right();
	BreathInitDesc.fRadian = fRadian;
	BreathInitDesc.fDistance = fDistance;

	m_pBreath->Reset(BreathInitDesc);
}

void CConjuredDragon::DeathBehavior(const _float& fTimeDelta)
{
	if (false == m_isEnterDeath)
	{
		CQuest_Manager* pQuest_Manager = CQuest_Manager::GetInstance();
		Safe_AddRef(pQuest_Manager);
		pQuest_Manager->Clear_Quest(TEXT("Quest_Bone"));
		Safe_Release(pQuest_Manager);
		m_isEnterDeath = true;
		m_isDead = true;
	}

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

void CConjuredDragon::Update_Bones()
{
	for (_uint i = 0; i < BONE_TYPE_END; ++i)
	{
		for (auto Desc : m_Bones[i])
		{
			_float4x4 BoneWorldMatrix = *Desc.pCombinedTransformationMatrix * m_pModelCom->Get_PivotFloat4x4() * m_pTransform->Get_WorldMatrix();
			Desc.vPosition = BoneWorldMatrix.Translation();
		}
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

			Off_Breath();
		}
	}

	if (true == m_isPhaseTwo &&
		true == m_pHealth->isDead())
	{
		m_isPhaseTwo = false;
		m_isPhaseFinal = true;
	}
}

HRESULT CConjuredDragon::Add_Components_for_Shake()
{
	try
	{
		/*CCamera_Shake::CAMERA_SHAKE_DESC Camera_Shake_Desc = { CCamera_Shake::CAMERA_SHAKE_DESC() };

		_float fMaxDistance = { 30.0f };
		_float fMinDistance = { 2.0f };


		Camera_Shake_Desc.eShake_Priority = CCamera_Manager::SHAKE_PRIORITY_1;
		Camera_Shake_Desc.isDistanceOption = true;
		Camera_Shake_Desc.pTransform = m_pTransform;
		Camera_Shake_Desc.Shake_Info_Desc.eEase = CEase::IN_EXPO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Axis = CCamera_Manager::SHAKE_AXIS_UP;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Power = CCamera_Manager::SHAKE_POWER_DECRECENDO;
		Camera_Shake_Desc.Shake_Info_Desc.eShake_Type = CCamera_Manager::SHAKE_TYPE_TRANSLATION;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeDuration = 0.2f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakePower = 0.05f;
		Camera_Shake_Desc.Shake_Info_Desc.fShakeSpeed = 10.0f;
		Camera_Shake_Desc.Shake_Info_Desc.vShake_Axis_Set = _float3();

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Enemy_Camera_Shake"),
			TEXT("Com_Step_Shake"), reinterpret_cast<CComponent**>(&m_pStep_Shake), &Camera_Shake_Desc)))
			throw TEXT("Com_Step_Shake");

		m_pStep_Shake->Ready_Shake(fMaxDistance, fMinDistance, Camera_Shake_Desc.Shake_Info_Desc.fShakePower);*/

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

HRESULT CConjuredDragon::Make_Notifies_for_Shake()
{
	return E_NOTIMPL;
}

HRESULT CConjuredDragon::Make_AI()
{
	BEGININSTANCE;

	try /* Failed Check Make_AI */
	{
		if (FAILED(__super::Make_AI()))
			throw TEXT("Failed Enemy Make_AI");

		if (FAILED(m_pRootBehavior->Add_Type("pBreath", m_pBreath)))
			throw TEXT("Failed Add_Type pBreath");

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
		if (FAILED(m_pRootBehavior->Add_Type("isPhaseFinal", &m_isPhaseFinal)))
			throw TEXT("Failed Add_Type isPhaseFinal");
		if (FAILED(m_pRootBehavior->Add_Type("isFinish", &m_isFinish)))
			throw TEXT("Failed Add_Type isFinish");

		/* Make Child Behaviors */
		CSelector* pSelector = nullptr;
		if (FAILED(Create_Behavior(pSelector)))
			throw TEXT("Failed Create_Behavior pSelector");

		CRandomChoose* pRandom_NonSpawn = { nullptr };
		if (FAILED(Create_Behavior(pRandom_NonSpawn)))
			throw TEXT("Failed Create_Behavior pRandom_NonSpawn");
		CSequence* pSequence_Death = nullptr;
		if (FAILED(Create_Behavior(pSequence_Death)))
			throw TEXT("Failed Create_Behavior pSequence_Death");
		CSelector* pSelector_Alive = nullptr;
		if (FAILED(Create_Behavior(pSelector_Alive)))
			throw TEXT("Failed Create_Behavior pSelector_Alive");
		CSelector* pSelector_Final = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Final)))
			throw TEXT("Failed Create_Behavior pSelector_Final");

		/* Set Decorations */

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(m_pRootBehavior->Assemble_Behavior(TEXT("Selector"), pSelector)))
			throw TEXT("Failed Assemble_Behavior Selector");

		if (FAILED(pSelector->Assemble_Behavior(TEXT("Random_NonSpawn"), pRandom_NonSpawn)))
			throw TEXT("Failed Assemble_Behavior Random_NonSpawn");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Death"), pSequence_Death)))
			throw TEXT("Failed Assemble_Behavior Sequence_Death");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Alive"), pSelector_Alive)))
			throw TEXT("Failed Assemble_Behavior Selector_Alive");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Final"), pSelector_Final)))
			throw TEXT("Failed Assemble_Behavior Selector_Final");

		if (FAILED(Make_NonSpawn(pRandom_NonSpawn)))
			throw TEXT("Failed Make_NonSpawn");
		if (FAILED(Make_Death(pSequence_Death)))
			throw TEXT("Failed Make_Death");
		if (FAILED(Make_Alive(pSelector_Alive)))
			throw TEXT("FAiled Make_Alive");
		if (FAILED(Make_Final(pSelector_Final)))
			throw TEXT("FAiled Make_Final");
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

HRESULT CConjuredDragon::Make_Magics()
{
	// Skill Magic PROJECTILE_WHITE
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_ATTACK_LIGHT;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = PROJECTILE_WHITE;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 1.0f;
		magicInitDesc.isChase = false;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// Skill Magic PROJECTILE_BLACK
	{
		CMagic::MAGICDESC magicInitDesc;
		magicInitDesc.eBuffType = BUFF_ATTACK_LIGHT;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = PROJECTILE_BLACK;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.fLifeTime = 1.0f;
		magicInitDesc.isChase = false;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, PROJECTILE_BLACK);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, PROJECTILE_WHITE);

	return S_OK;
}

HRESULT CConjuredDragon::Make_Notifies()
{
	function<void()> Func = [&] { this->Change_Animation(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Change_Animation"), Func)))
		return E_FAIL;

	Func = [&] { m_pWeapon_Left_Wing->On_Collider_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Left_Wing_Attack"), Func)))
		return E_FAIL;

	Func = [&] { m_pWeapon_Right_Wing->On_Collider_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Right_Wing_Attack"), Func)))
		return E_FAIL;
	
	Func = [&] { m_pWeapon_Tail->On_Collider_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Tail_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->Enter_Charge_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Enter_Charge_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->Exit_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Exit_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->On_Breath(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("On_Breath"), Func)))
		return E_FAIL;

	Func = [&] { this->Off_Breath(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Off_Breath"), Func)))
		return E_FAIL;

	Func = [&] { this->Action_Pulse(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Action_Pulse"), Func)))
		return E_FAIL;

	Func = [&] { this->Shot_Fireball_White(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Fireball_White"), Func)))
		return E_FAIL;

	Func = [&] { this->Shot_Fireball_Black(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shot_Fireball_Black"), Func)))
		return E_FAIL;
	
	Func = [&] { this->Pulse_Charge(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Pulse_Charge"), Func)))
		return E_FAIL;

	Func = [&] { this->Pulse_Stop_Charge(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Pulse_Stop_Charge"), Func)))
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
		HealthDesc.iMaxHP = 1000;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.isGravity = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 6.f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxSphereGeometry pSphereGeometry = PxSphereGeometry(6.f);
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

		/* For.Collider_Attack_Body */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 6.f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		pSphereGeometry = PxSphereGeometry(6.1f);
		RigidBodyDesc.pGeometry = &pSphereGeometry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotY | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Attack_Body");

		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Collider_Attack_Body");

		/* For.MagicSlot */
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
			TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
		{
			MSG_BOX("Failed CTest_Player Add_Component : (Com_MagicSlot)");
			return E_FAIL;
		}

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
	BEGININSTANCE;

	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_ConjuredDragon"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");

		/* For.EnergyBall */
		CEnergyBall::ENERGYBALLINITDESC EnergyBallInitDesc;
		EnergyBallInitDesc.DeathFunction = [&](const _float& fTimeDelta)->_bool { return this->Break_Invincible(fTimeDelta); };
		EnergyBallInitDesc.fActionProtegoTime = 2.f;

		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_EnergyBall"),
			TEXT("Com_EnergyBall"), reinterpret_cast<CComponent**>(&m_pEnergyBall), &EnergyBallInitDesc)))
			throw TEXT("Com_EnergyBall");

		/* For.Breath */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Breath"),
			TEXT("Com_Breath"), reinterpret_cast<CComponent**>(&m_pBreath))))
			throw TEXT("Com_Breath");

		/* For.Pulse */
		CPulse::PULSEINITDESC PulseInitDesc;
		PulseInitDesc.fLifeTime = 5.f;
		PulseInitDesc.fSpeed = 2.f;
		PulseInitDesc.pTarget = m_pTarget;
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Pulse"),
			TEXT("Com_Pulse"), reinterpret_cast<CComponent**>(&m_pPulse), &PulseInitDesc)))
			throw TEXT("Com_Pulse");

		/* For.Com_Weapon_Head */
		const CBone* pBone = m_pModelCom->Get_Bone(TEXT("head"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		CWeapon_Dragon_Head::PARENTMATRIXDESC ParentMatrixDesc;
		ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Dragon_Head"),
			TEXT("Com_Weapon_Head"), reinterpret_cast<CComponent**>(&m_pWeapon_Head), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon_Head");

		/* For.Com_Weapon_Left_Wing */
		pBone = m_pModelCom->Get_Bone(TEXT("middleringwing_04_left"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		ParentMatrixDesc.OffsetMatrix = _float4x4();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Weapon_Dragon_Left_Wing"),
			TEXT("Com_Weapon_Left_Wing"), reinterpret_cast<CComponent**>(&m_pWeapon_Left_Wing), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon_Left_Wing");

		/* For.Com_Weapon_Right_Wing */
		pBone = m_pModelCom->Get_Bone(TEXT("middleringwing_04_right"));
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		ParentMatrixDesc.OffsetMatrix = _float4x4();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Weapon_Dragon_Right_Wing"),
			TEXT("Com_Weapon_Right_Wing"), reinterpret_cast<CComponent**>(&m_pWeapon_Right_Wing), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon_Right_Wing");

		/* For.Com_Weapon_Tail */
		pBone = m_pModelCom->Get_Bone_Index(178);
		if (nullptr == pBone)
			throw TEXT("pBone is nullptr");

		ParentMatrixDesc.OffsetMatrix = _float4x4();
		ParentMatrixDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

		if (FAILED(Add_Component(iCurrentLevelIndex, TEXT("Prototype_GameObject_Weapon_Dragon_Tail"),
			TEXT("Com_Weapon_Tail"), reinterpret_cast<CComponent**>(&m_pWeapon_Tail), &ParentMatrixDesc)))
			throw TEXT("Com_Weapon_Tail");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		ENDINSTANCE;
		__debugbreak();

		return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

HRESULT CConjuredDragon::Bind_HitMatrices()
{
	const CBone* pBone = m_pModelCom->Get_Bone(TEXT("head"));
	if (nullptr == pBone)
		return E_FAIL;
	m_pHeadMatrix = m_HitMatrices[0] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("hip"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[1] = pBone->Get_CombinedTransformationMatrixPtr();

	pBone = m_pModelCom->Get_Bone(TEXT("spine_03"));
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[2] = pBone->Get_CombinedTransformationMatrixPtr();

	/* Setting Weapon Offset Matrix */
	pBone = m_pModelCom->Get_Bone(TEXT("jaw"));
	if (nullptr == pBone)
		return E_FAIL;
	_float4x4 WeaponOffsetMatrix = pBone->Get_OffsetMatrix() * m_pModelCom->Get_PivotFloat4x4();
	m_pWeapon_Head->Set_Offset_Matrix(WeaponOffsetMatrix);
	
	/* Setting Wing, Tail Bone Matrices */
	DRAGONBONEDATA BoneData;
	pBone = m_pModelCom->Get_Bone_Index(150);
	if (nullptr == pBone)
		return E_FAIL;
	BoneData.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	m_Bones[LEFT_WING].push_back(BoneData);

	pBone = m_pModelCom->Get_Bone_Index(151);
	if (nullptr == pBone)
		return E_FAIL;
	BoneData.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	m_Bones[RIGHT_WING].push_back(BoneData);

	for (_uint i = 171; i < 185; ++i)
	{
		pBone = m_pModelCom->Get_Bone_Index(i);
		if (nullptr == pBone)
			return E_FAIL;
		BoneData.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		m_Bones[TAIL].push_back(BoneData);
	}

	return S_OK;
}

HRESULT CConjuredDragon::Add_Effects()
{
	try /* Check Add_Components */
	{
		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_ImpulseSphere_Effect"),
			TEXT("Com_ImpulseSphere_Effect"), reinterpret_cast<CComponent**>(&m_pEffect_ImpulseSphere))))
			throw TEXT("Com_ImpulseSphere_Effect");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Breath_Effect"),
			TEXT("Com_Breath_Effect"), reinterpret_cast<CComponent**>(&m_pEffect_Breath))))
			throw TEXT("Com_Breath_Effect");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_BlackSmokeIdle"),
			TEXT("Com_BlackSmokeIdle"), reinterpret_cast<CComponent**>(&m_pEffect_BlackSmokeIdle))))
			throw TEXT("Com_BlackSmokeIdle");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Charge"),
			TEXT("Com_Pulse_Charge"), reinterpret_cast<CComponent**>(&m_pEffect_Pulse_Charge))))
			throw TEXT("Com_Pulse_Charge");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_SplashWater"),
			TEXT("Com_Pulse_SplashWater"), reinterpret_cast<CComponent**>(&m_pEffect_Pulse_SplashWater))))
			throw TEXT("Com_Pulse_SplashWater");
		
		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_Rock"),
			TEXT("Com_Pulse_Rock"), reinterpret_cast<CComponent**>(&m_pEffect_Pulse_Rock))))
			throw TEXT("Com_Pulse_Rock");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_CircleEmit"),
			TEXT("Com_Pulse_CircleEmit"), reinterpret_cast<CComponent**>(&m_pEffect_Pulse_CircleEmit))))
			throw TEXT("Com_Pulse_CircleEmit");

		if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_Pulse_BoomWispy"),
			TEXT("Com_Pulse_BoomWispy"), reinterpret_cast<CComponent**>(&m_pEffect_Pulse_BoomWispy))))
			throw TEXT("Com_Pulse_BoomWispy");

		m_pEffect_Pulse_Charge->Disable();
		m_pEffect_Pulse_SplashWater->Disable();
		m_pEffect_Pulse_Rock->Disable();
		m_pEffect_Pulse_CircleEmit->Disable();
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

HRESULT CConjuredDragon::Make_NonSpawn(CRandomChoose* pRandomChoose)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pRandomChoose)
			throw TEXT("Parameter pRandomChoose is nullptr");

		/* Create Child Behaviors */
		CAction* pAction_Intro_1 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_1)))
			throw TEXT("Failed Create_Behavior pAction_Intro_1");
		CAction* pAction_Intro_2 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_2)))
			throw TEXT("Failed Create_Behavior pAction_Intro_2");
		CAction* pAction_Intro_3 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro_3)))
			throw TEXT("Failed Create_Behavior pAction_Intro_3");

		/* Set Decorator */
		pRandomChoose->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				return true != *pIsSpawn;
			});
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pAction_Intro_1->Set_Options(TEXT("Intro_1_1"), m_pModelCom);
		pAction_Intro_2->Set_Options(TEXT("Intro_1_2"), m_pModelCom);
		pAction_Intro_3->Set_Options(TEXT("Intro_1_3"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Intro_1"), pAction_Intro_1, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_1");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Intro_2"), pAction_Intro_2, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_2");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Action_Intro_3"), pAction_Intro_3, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Action_Intro_3");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_NonSpawn : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	ENDINSTANCE;
	
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
				_bool* pIsFinish = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isFinish", pIsFinish)))
					return false;

				return *pIsFinish;
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
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Create Child Behavior */
		CAction* pAction_Intro = { nullptr };
		if (FAILED(Create_Behavior(pAction_Intro)))
			throw TEXT("Failed Create_Behavior pAction_Intro");
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
		pAction_Intro->Add_Success_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				CTransform* pTransform = { nullptr };
				_bool* pIsPhaseOne = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("pTransform", pTransform)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isPhaseOne", pIsPhaseOne)))
					return false;

				_float4x4 WorldMatrix = XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixTranslation(-2.f, -22.f, 130.f);
				pTransform->Set_WorldMatrix(WorldMatrix);
				*pIsPhaseOne = true;

				return true;
			});

		/* Set Options */
		pAction_Intro->Set_Options(TEXT("Intro_2"), m_pModelCom, false, 0.f, true);

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Action_Intro"), pAction_Intro)))
			throw TEXT("Failed Assemble_Behavior Action_Intro");
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

HRESULT CConjuredDragon::Make_Final(_Inout_ CSelector* pSelector)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSelector)
			throw TEXT("Parameter pSelector is nullptr");

		/* Create Child Behavior */
		CSequence* pSequence_Enter_Final = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Enter_Final)))
			throw TEXT("Failed Create_Behavior pSequence_Enter_Final");
		CSelector* pSelector_Ground_Pattern = { nullptr };
		if (FAILED(Create_Behavior(pSelector_Ground_Pattern)))
			throw TEXT("Failed Create_Behavior pSelector_Ground_Pattern");

		/* Set Decorators */
		pSelector->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseFinal = { nullptr };
				_bool* pIsFinish = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseFinal", pIsPhaseFinal)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isFinish", pIsFinish)))
					return false;

				return false == *pIsFinish && true == *pIsPhaseFinal;
			});

		/* Set Options */

		/* Assemble Behaviors */
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Sequence_Enter_Final"), pSequence_Enter_Final)))
			throw TEXT("Failed Assemble_Behavior Sequence_Enter_Final");
		if (FAILED(pSelector->Assemble_Behavior(TEXT("Selector_Ground_Pattern"), pSelector_Ground_Pattern)))
			throw TEXT("Failed Assemble_Behavior Selector_Ground_Pattern");

		if (FAILED(Make_Enter_Final(pSequence_Enter_Final)))
			throw TEXT("Failed Make_Enter_Final");
		if (FAILED(Make_Ground_Pattern(pSelector_Ground_Pattern)))
			throw TEXT("Failed Make_Ground_Pattern");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Final : \n");
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
		pSequence->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseTwo = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseTwo", pIsPhaseTwo)))
					return false;

				return false == *pIsPhaseTwo;
			});
		pAction_Enter_Next_Phase->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsPhaseOne = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseOne", pIsPhaseOne)))
					return false;

				return false == *pIsPhaseOne;
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
				_bool* pIsSpawn = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseTwo", pIsPhaseTwo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isSpawn", pIsSpawn)))
					return false;

				*pIsPhaseTwo = true;
				*pIsSpawn = false;

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
				CRigidBody* pRigidBody = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isPhaseTwo", pIsPhaseTwo)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("pRigidBody", pRigidBody)))
					return false;

				if (true == *pIsPhaseTwo)
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

HRESULT CConjuredDragon::Make_Enter_Final(_Inout_ CSequence* pSequence)
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
		CSequence* pSequence_Change_Invincible = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Change_Invincible)))
			throw TEXT("Failed Create_Behavior pSequence_Change_Invincible");

		/* Set Decorators */

		/* Set Options */
		pAction_Ground_To_Fly->Set_Options(TEXT("Ground_To_Fly"), m_pModelCom, false, 0.f, true);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Ground_To_Fly"), pAction_Ground_To_Fly)))
			throw TEXT("Failed Assemble_Behavior Action_Ground_To_Fly");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Sequence_Change_Invincible"), pSequence_Change_Invincible)))
			throw TEXT("Failed Assemble_Behavior Sequence_Change_Invincible");

		if (FAILED(Make_Air_Change_Invincible(pSequence_Change_Invincible)))
			throw TEXT("Failed Assemble_Behavior Make_Air_Change_Invincible");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Enter_Final : \n");
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
		CSequence* pSequence_Attack_AOE = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_AOE)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_AOE");
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
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_AOE"), pSequence_Attack_AOE, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_AOE");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Range"), pSequence_Attack_Range, 0.7f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Range");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Melee"), pSequence_Attack_Melee, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Melee");

		if (FAILED(Make_Ground_Attack_Pulse(pSequence_Attack_Pulse)))
			throw TEXT("Failed Make_Ground_Attack_Pulse");
		if (FAILED(Make_Ground_Attack_AOE(pSequence_Attack_AOE)))
			throw TEXT("Failed Make_Ground_Attack_AOE");
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
		pSequence_Attack_Swipe_Left->Set_Attack_Option(50.f);
		pSequence_Attack_Swipe_Right->Set_Attack_Action_Options(TEXT("Ground_Attack_Swipe_Right"), m_pModelCom);
		pSequence_Attack_Swipe_Right->Set_Attack_Option(50.f);
		pSequence_Attack_Tail_360->Set_Attack_Action_Options(TEXT("Ground_Attack_Tail_360"), m_pModelCom);
		pSequence_Attack_Tail_360->Set_Attack_Option(50.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Check_Distance"), pCheck_Distance)))
			throw TEXT("Failed Assemble_Behavior Check_Distance");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swipe_Left"), pSequence_Attack_Swipe_Left, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swipe_Left"); // 이거요.
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Swipe_Right"), pSequence_Attack_Swipe_Right, 0.33f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Swipe_Right"); // 이거요.
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Tail_360"), pSequence_Attack_Tail_360, 0.34f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Tail_360"); // 이거요.
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
		pSequence_Attack_Breath->Set_Attack_Action_Options(TEXT("Ground_Attack_Breath"), m_pModelCom);
		pSequence_Attack_Breath->Set_Attack_Option(100.f);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Random_Attacks"), pRandom_Attacks)))
			throw TEXT("Failed Assemble_Behavior Random_Attacks");

		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_1"), pSequence_Attack_Fireball_1, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_1");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Fireball_2"), pSequence_Attack_Fireball_2, 0.3f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball_2");
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("Sequence_Attack_Charge"), pSequence_Attack_Charge, 0.1f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Charge"); // 이거요.
		if (FAILED(pRandom_Attacks->Assemble_Behavior(TEXT("pSequence_Attack_Breath"), pSequence_Attack_Breath, 0.3f)))
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

HRESULT CConjuredDragon::Make_Ground_Attack_AOE(_Inout_ CSequence* pSequence)
{
	BEGININSTANCE;

	try
	{
		if (nullptr == pSequence)
			throw TEXT("Parameter pSequence is nullptr");

		CAction* pAction_Ground_To_Fly = { nullptr };
		if (FAILED(Create_Behavior(pAction_Ground_To_Fly)))
			throw TEXT("Failed Create_Behavior pAction_Ground_To_Fly");
		CAction* pAction_Attack_AOE_1 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Attack_AOE_1)))
			throw TEXT("Failed Create_Behavior pAction_Attack_AOE_1");
		CAction* pAction_Attack_AOE_2 = { nullptr };
		if (FAILED(Create_Behavior(pAction_Attack_AOE_2)))
			throw TEXT("Failed Create_Behavior pAction_Attack_AOE_2");
		CAction* pAction_Land = { nullptr };
		if (FAILED(Create_Behavior(pAction_Land)))
			throw TEXT("Failed Create_Behavior pAction_Land");

		/* Set Options */
		pAction_Ground_To_Fly->Set_Options(TEXT("Ground_To_Fly"), m_pModelCom);
		pAction_Attack_AOE_1->Set_Options(TEXT("Attack_AOE_1"), m_pModelCom);
		pAction_Attack_AOE_2->Set_Options(TEXT("Attack_AOE_2"), m_pModelCom);
		pAction_Land->Set_Options(TEXT("Landing_To_Cmbt"), m_pModelCom);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Ground_To_Fly"), pAction_Ground_To_Fly)))
			throw TEXT("Failed Assemble_Behavior Action_Ground_To_Fly");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_AOE_1"), pAction_Attack_AOE_1)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_AOE_1");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Attack_AOE_2"), pAction_Attack_AOE_2)))
			throw TEXT("Failed Assemble_Behavior Action_Attack_AOE_2");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Land"), pAction_Land)))
			throw TEXT("Failed Assemble_Behavior Action_Land");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CConjuredDragon] Failed Make_Ground_Attack_AOE : \n");
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
				_bool* pIsBreakInvincible = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("fInvincibleGauge", pInvincibleGauge)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("isBreakInvincible", pIsBreakInvincible)))
					return false;

				*pInvincibleGauge = 0.f;
				*pIsBreakInvincible = false;

				return true;
			});
		pAction_Break_Invinclble->Add_Decorator([&](CBlackBoard* pBlackBoard)->_bool
			{
				_bool* pIsInvincible = { nullptr };
				CBreath* pBreath = { nullptr };
				if (FAILED(pBlackBoard->Get_Type("isInvincible", pIsInvincible)))
					return false;
				if (FAILED(pBlackBoard->Get_Type("pBreath", pBreath)))
					return false;

				*pIsInvincible = false;
				pBreath->Off_Breath();

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
		CAction* pAction_Hover = { nullptr };
		if (FAILED(Create_Behavior(pAction_Hover)))
			throw TEXT("Failed Create_Behavior pAction_Hover");

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
		pAction_Change_Invinclble->Set_Options(TEXT("Fire_Burst_Recovery_1"), m_pModelCom);
		pAction_Hover->Set_Options(TEXT("Fire_Burst_Recovery_2"), m_pModelCom);
		pTsk_LookAt->Set_Option(m_pTransform);

		/* Assemble Behaviors */
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Change_Invinclble"), pAction_Change_Invinclble)))
			throw TEXT("Failed Assemble_Behavior Action_Change_Invinclble");
		if (FAILED(pSequence->Assemble_Behavior(TEXT("Action_Hover"), pAction_Hover)))
			throw TEXT("Failed Assemble_Behavior Action_Hover");

		if (FAILED(pSequence->Assemble_Behavior(TEXT("Tsk_LookAt"), pTsk_LookAt)))
			throw TEXT("Failed Assemble_Behavior Tsk_LookAt");
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
		CSequence* pSequence_Attack_Breath = { nullptr };
		if (FAILED(Create_Behavior(pSequence_Attack_Breath)))
			throw TEXT("Failed Create_Behavior pSequence_Attack_Breath");

		CAction* pAction_Breath_WindUp = { nullptr };
		if (FAILED(Create_Behavior(pAction_Breath_WindUp)))
			throw TEXT("Failed Create_Behavior pAction_Breath_WindUp");
		CSequence_Attack* pAttack_Breath = { nullptr };
		if (FAILED(Create_Behavior(pAttack_Breath)))
			throw TEXT("Failed Create_Behavior pAttack_Breath");

		/* Set Decorators */
		pRandomChoose->Add_Change_Condition(CBehavior::BEHAVIOR_SUCCESS, [&](CBlackBoard* pBlackBoard)->_bool
			{
				return true;
			});

		/* Set Options */
		pSequence_Attack_Purse->Set_Attack_Action_Options(TEXT("Attack_Pulse"), m_pModelCom);
		pSequence_Attack_Purse->Set_Attack_Option(100.f);
		pAction_Breath_WindUp->Set_Options(TEXT("Attack_Fire_Breath_Wind_Up"), m_pModelCom);
		pAttack_Breath->Set_Attack_Action_Options(TEXT("Attack_Fire_Breath"), m_pModelCom);
		pAttack_Breath->Set_Attack_Option(100.f);

		/* Assemble Behaviors */
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Fireball"), pSequence_Attack_Fireball, 0.45f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Fireball");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Breath"), pSequence_Attack_Breath, 0.35f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Breath");
		if (FAILED(pRandomChoose->Assemble_Behavior(TEXT("Sequence_Attack_Purse"), pSequence_Attack_Purse, 0.2f)))
			throw TEXT("Failed Assemble_Behavior Sequence_Attack_Purse");

		if (FAILED(pSequence_Attack_Breath->Assemble_Behavior(TEXT("Action_Breath_WindUp"), pAction_Breath_WindUp)))
			throw TEXT("Failed Assemble_Behavior Action_Breath_WindUp");
		if (FAILED(pSequence_Attack_Breath->Assemble_Behavior(TEXT("Attack_Breath"), pAttack_Breath)))
			throw TEXT("Failed Assemble_Behavior Attack_Breath");

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
		pSequence_Attack_Fireball_1->Set_Attack_Action_Options(TEXT("Air_Attack_Fireball_1"), m_pModelCom);
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

void CConjuredDragon::Exit_Attack()
{
	m_pWeapon_Left_Wing->Off_Collider_Attack();
	m_pWeapon_Right_Wing->Off_Collider_Attack();
	m_pWeapon_Tail->Off_Collider_Attack();
	m_pRigidBody->Disable_Collision("Attack_Body");
}

void CConjuredDragon::Shot_Fireball_Black()
{
	if (nullptr == m_pTarget)
		return;

	m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon_Head, COLLISIONFLAG(COL_PLAYER | COL_SHIELD));
}

void CConjuredDragon::Shot_Fireball_White()
{
	if (nullptr == m_pTarget)
		return;

	m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon_Head, COLLISIONFLAG(COL_PLAYER | COL_SHIELD));
}

void CConjuredDragon::Enter_Charge_Attack()
{
	m_pRigidBody->Enable_Collision("Attack_Body", this, &m_CollisionRequestDesc);
}

void CConjuredDragon::On_Breath()
{
	if (nullptr != m_pBreath)
		m_pBreath->On_Breath();
	
	m_pEffect_Breath->Play(&m_vHeadPosition, &m_vTargetPosition, -34.f);
}

void CConjuredDragon::Off_Breath()
{
	if (nullptr != m_pBreath)
		m_pBreath->Off_Breath();

	m_pEffect_Breath->Stop();
}

void CConjuredDragon::Action_Pulse()
{
	if (nullptr == m_pPulse)
		return;

	CPulse::PULSEINITDESC PulseInitDesc;
	PulseInitDesc.vPosition = m_vOffsetPos;
	PulseInitDesc.fLifeTime = 3.5f;
	PulseInitDesc.fSpeed = 0.35f;
	PulseInitDesc.pTarget = m_pTarget;
	m_pPulse->Reset(PulseInitDesc);

	m_pEffect_ImpulseSphere->Play(m_vOffsetPos);
	m_pEffect_Pulse_BoomWispy->Play(m_vOffsetPos);
	
}

void CConjuredDragon::Pulse_Charge()
{
	m_pEffect_Pulse_Charge->Play(m_vOffsetPos);
}

void CConjuredDragon::Pulse_Stop_Charge()
{
	m_pEffect_Pulse_Charge->Stop();
	m_pEffect_Pulse_CircleEmit->Stop();
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

	Safe_Release(m_pBreath);
	Safe_Release(m_pEnergyBall);
	Safe_Release(m_pPulse);

	Safe_Release(m_pMagicSlot);
	Safe_Release(m_pWeapon_Head);
	Safe_Release(m_pWeapon_Left_Wing);
	Safe_Release(m_pWeapon_Right_Wing);
	Safe_Release(m_pWeapon_Tail);

	Safe_Release(m_pEffect_BlackSmokeIdle);
	Safe_Release(m_pEffect_Breath);
	Safe_Release(m_pEffect_Pulse_Charge);
	Safe_Release(m_pEffect_Pulse_CircleEmit);
	Safe_Release(m_pEffect_Pulse_Rock);
	Safe_Release(m_pEffect_Pulse_SplashWater);
	Safe_Release(m_pEffect_Pulse_BoomWispy);
	Safe_Release(m_pEffect_ImpulseSphere);
}
