#include "Pensive.h"
#include "GameInstance.h"
#include "StateContext_Enemy.h"
#include "Weapon_Pensive.h"
#include "UI_Group_Enemy_HP.h"
#include "MagicSlot.h"
#include "Weapon_Dragon_Head.h"
#include "MagicBall.h"
#include "Vault_Torch.h"
#include "Layer.h"
#include "ParticleSystem.h"

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

	m_SwordOffsetMatrix[0] = _float4x4().MatrixTranslation(_float3(1, 5, 0));
	m_SwordOffsetMatrix[1] = _float4x4().MatrixTranslation(_float3(1, -5, 0));
	m_SwordOffsetMatrix[2] = _float4x4().MatrixTranslation(_float3(3, 0, 0));

	const CBone* pBone = m_pModelCom->Get_Bone_Index(7);
	if (nullptr == pBone)
		return E_FAIL;
	m_HitMatrices[0] = pBone->Get_CombinedTransformationMatrixPtr();
	m_AttackPosition = *m_HitMatrices[0] * m_pModelCom->Get_PivotFloat4x4();
	m_pHitMatrix = &m_AttackPosition;

	BEGININSTANCE
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = 10.f;
	m_vLightColor = LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
	pGameInstance->Add_Light(LightDesc, &m_pLight);
	m_pModelCom->Strong_Change_Animation(TEXT("Spawn"));
	
	//ÅäÄ¡¸¦ Ã£¾Æ¿ÍÁÝ½Ã´Ù.
	CLayer* pLayer = pGameInstance->Find_Layer(iCurrentLevelIndex,TEXT("Layer_BackGround"));
	ENDINSTANCE;
	Safe_AddRef(pLayer);
	_umap<const _tchar*, class CComponent*>* pComponent_Map = pLayer->Get_Components();
	for (auto it = pComponent_Map->begin(); it != pComponent_Map->end(); ++it)
	{
		if (wcsstr((it->first), TEXT("GameObject_Vault_Torch")) != nullptr)
		{
			if (it->second != nullptr &&
				static_cast<CVault_Torch*>(it->second)->Get_TorchIndex() > 23 &&
				static_cast<CVault_Torch*>(it->second)->Get_TorchIndex() < 36)
			{
				m_pTorch.push_back(static_cast<CVault_Torch*>(it->second));
				Safe_AddRef(it->second);
			}
				
		}
	}
	PensiveSwitchOff();
	Safe_Release(pLayer);
	m_pSkillDistotionParticle->Play(m_pTransform->Get_Position());
	return S_OK;
}

void CPensive::Tick(_float fTimeDelta)
{
	if (!m_isSpawn)
		return;
	m_AttackPosition = *m_HitMatrices[0] * m_pModelCom->Get_PivotFloat4x4();
	__super::Tick(fTimeDelta);

	if (m_isTurnAble)
	{
		m_pTransform->LookAt_Lerp((m_pPlayer)->Get_Transform()->Get_Position(), fTimeDelta * 2.f, true);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);

	if (m_isEmmisivePowerAdd)
	{
		_float fMultify = 0;
		switch (m_iAttackType)
		{
		case ATTACK_HAMMER:
			fMultify = 0.7f;
			break;
		case ATTACK_GROUND:
			fMultify = 0.7f;
			break;
		case ATTACK_SWORD:
			fMultify = 1.0f;
			break;
		case ATTACK_ORB:
			fMultify = 0.24f;
			break;
		case ATTACK_SCREAM:
			fMultify = 0.24f;
			break;
		default:
			fMultify = 0.2f;
			break;
		}
		m_fEmissivePower += fTimeDelta * fMultify;
	}
	else 
	{
		m_fEmissivePower -= fTimeDelta;
		if (m_fEmissivePower < 0)
			m_fEmissivePower = 0;
	}
	m_pSkillDistotionParticle->Get_EmissionModuleRef().fRateOverTime = m_fEmissivePower * 10;
	m_pSkillDistotionParticle->Get_MainModuleRef().vStartSizeRange.x = m_fEmissivePower * 20;
	m_pSkillDistotionParticle->Get_MainModuleRef().vStartSizeRange.y = m_fEmissivePower * 40;
}

void CPensive::Late_Tick(_float fTimeDelta)
{
	if (!m_isSpawn)
		return;
	__super::Late_Tick(fTimeDelta);
}

void CPensive::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		BUFF_TYPE eBuff = pCollisionMagicBallDesc->eBuffType;
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		Print_Damage_Font(iDamage);

		Do_Damage(iDamage);
		_float3 vDir = CollisionEventDesc.pOtherTransform->Get_Position() - m_pTransform->Get_Position();
		vDir.Normalize();
		m_iCurrentSpell |= eBuff;
	}
}

HRESULT CPensive::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		try /* Failed Render */
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
				throw TEXT("Bind_BoneMatrices");

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, DIFFUSE)))
				throw TEXT("Bind_Material Diffuse");
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, NORMALS)))
				throw TEXT("Bind_Material Normal");

			if (true == m_isDissolve)
			{
				if (FAILED(m_pShaderCom->Begin("AnimMesh_Dissolve")))
					return E_FAIL;
			}
			else if (0 == i)
			{
				if (FAILED(m_pEmissiveTexture_1->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin("Pensive")))
					throw TEXT("Shader Begin Pensive");
			}
			else
			{
				if (FAILED(m_pEmissiveTexture_2->Bind_ShaderResource(m_pShaderCom, "g_EmissiveTexture")))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin("Pensive")))
					throw TEXT("Shader Begin Pensive");
			}

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

HRESULT CPensive::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmissivePower", &m_fEmissivePower, sizeof(_float))))
		throw TEXT("Failed Bind_RawValue : m_fEmissivePower");
	return S_OK;
}

void CPensive::Set_Protego_Collision(CTransform* pTransform, ATTACKTYPE eType) const
{
	if (eType == ATTACK_BREAK ||
		eType == ATTACK_SUPERBREAK)
	{
		if (m_pMagicBall_Attack != nullptr)
			m_pMagicBall_Attack->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_DYING);
		m_pStateContext->Set_StateMachine(TEXT("Hit"));
		m_pModelCom->Change_Animation(TEXT("Attack_Orb_Hit"));
	}
}

void CPensive::Do_Damage(_int iDmg)
{
	m_pHealth->Damaged(iDmg);
	if(m_isSpawn && !m_isDead)
	{
		if (m_pHealth->Get_Current_HP() <= 0 && !m_pStateContext->Is_Current_State(TEXT("Death")))
		{
			DieMagicBall();

			m_isDead = true;

			m_pStateContext->Set_StateMachine(TEXT("Death"));
			return;
		}

		if (m_pHealth->Get_MaxHP() * 0.5f > m_pHealth->Get_Current_HP())
		{
			m_iPhase = 2;
		}
		if (m_isAttackAble)
		{
			m_iGroogyStack += iDmg;
			if (m_iGroogyStack > 400)
			{
				m_iGroogyStack = 0;
				m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.2f);
				ADD_DECREASE_LIGHT(m_pTransform->Get_Position(), 100.f, 0.6f, m_vLightColor);
				DieMagicBall();
				m_pStateContext->Set_StateMachine(TEXT("Groogy"));
				m_pModelCom->Change_Animation(TEXT("Stun_Start"));
			}
		}
	}
}

HRESULT CPensive::Ready_StateMachine(_uint iCurrentLevelIndex)
{
	m_StateMachineDesc.pOwnerModel = m_pModelCom;
	m_StateMachineDesc.pPhase = &m_iPhase;
	m_StateMachineDesc.pAttackType = &m_iAttackType;
	m_StateMachineDesc.pisAttackable = &m_isAttackAble;
	m_StateMachineDesc.pisTurnable = &m_isTurnAble;

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

	Func = [&] {(*this).Next_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("ShootOrb"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Shouting(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Shouting_Cast"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Next_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Explosion"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Throw_Sword(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Create"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Throw_Sword(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Create2"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Throw_Sword(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Create3"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Next_SwordAttack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Throw"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Next_SwordAttack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Throw2"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Next_SwordAttack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Weapon_Sword_Throw3"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Attack_Mace(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Cast_Mace"), Func)))
		return E_FAIL;
	Func = [&] {(*this).Next_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Next_Mace"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_Turnable_True(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("TurnAble"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_Turnable_False(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("TurnDisable"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_AttackAble_True(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("AttackAble"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_AttackAble_False(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("AttackDisable"), Func)))
		return E_FAIL;

	Func = [&] {(*this).PensiveSwitchOn(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Torch_On"), Func)))
		return E_FAIL;

	Func = [&] {(*this).PensiveSwitchOff(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Torch_Off"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_EmmisivePower_Add(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("EmmisiverPower_Add"), Func)))
		return E_FAIL;

	Func = [&] {(*this).Set_EmmisivePower_Minus(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("EmmisiverPower_Minus"), Func)))
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
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PENSIVE_SHOUTING;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = false;
		magicInitDesc.fLifeTime = 0.6f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PENSIVE_SWORD_THROW;
		magicInitDesc.fInitCoolTime = 0.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = false;
		magicInitDesc.fLifeTime = 0.6f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = PENSIVE_MACE_ATTACK;
		magicInitDesc.fInitCoolTime = 0.f;
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
	m_pMagicSlot->Add_Magic_To_Skill_Slot(3, PENSIVE_SHOUTING); 

	m_pMagicSlot->Add_Magic_To_Basic_Slot(0, PENSIVE_SWORD_THROW); 
	m_pMagicSlot->Add_Magic_To_Basic_Slot(1, PENSIVE_MACE_ATTACK);
	return S_OK;
}

void CPensive::DieMagicBall()
{
	if (m_pMagicBall_Attack != nullptr)
		m_pMagicBall_Attack->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_END);
	if (m_pMagicBall_Protego != nullptr)
		m_pMagicBall_Protego->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_END);
	if (m_pMagicBall_Sword[0] != nullptr)
		m_pMagicBall_Sword[0]->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_END);
	if (m_pMagicBall_Sword[1] != nullptr)
		m_pMagicBall_Sword[1]->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_END);
	if (m_pMagicBall_Sword[2] != nullptr)
		m_pMagicBall_Sword[2]->Set_MagicBallState(CMagicBall::MAGICBALL_STATE_END);
}

void CPensive::PensiveSwitchOn()
{
	for (_int i = 0; i < m_pTorch.size(); i++)
	{
		m_pTorch[i]->Switch_OnOff(true);
	}
}

void CPensive::PensiveSwitchOff()
{
	for (_int i = 0; i < m_pTorch.size(); i++)
	{
		m_pTorch[i]->Switch_OnOff(false);
	}
}

void CPensive::Attack_Ground()
{
	m_pMagicSlot->Action_Magic_Skill((_uint)0, m_pPlayer, m_pDragonHead[0], COLLISIONFLAG(COL_PLAYER | COL_SHIELD));
	_float3 vAxis = _float3(1, 1, 1);

	vAxis.Normalize();
	BEGININSTANCE;
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_PRIORITY)0,
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
	m_pMagicBall_Attack = m_pMagicSlot->Action_Magic_Skill((_uint)1, m_pPlayer, m_pDragonHead[1], COLLISIONFLAG(COL_PLAYER | COL_SHIELD | COL_STATIC));
	m_pMagicSlot->Add_Magics(m_ProtegoInitDesc[rand()%3]);
	m_pMagicBall_Protego = m_pMagicSlot->Action_Magic_Skill((_uint)2, m_pDragonHead[1], m_pDragonHead[1], COLLISIONFLAG(COL_MAGIC), COL_SHIELD_ENEMY);
}

void CPensive::Attack_Shouting()
{
	m_pMagicBall_Attack = m_pMagicSlot->Action_Magic_Skill((_uint)3, m_pPlayer, m_pDragonHead[0], COLLISIONFLAG(COL_PLAYER | COL_SHIELD | COL_STATIC));
}

void CPensive::Attack_Throw_Sword()
{
	m_pDragonHead[2]->Set_Offset_Matrix(m_SwordOffsetMatrix[m_iSwordIndex]);
	m_pMagicBall_Sword[m_iSwordIndex++] = m_pMagicSlot->Action_Magic_Basic((_uint)0, m_pPlayer, m_pDragonHead[2], COLLISIONFLAG(COL_PLAYER | COL_SHIELD | COL_STATIC));
}

void CPensive::Attack_Mace()
{
	m_pMagicBall_Attack = m_pMagicSlot->Action_Magic_Basic((_uint)1, m_pPlayer, m_pDragonHead[1], COLLISIONFLAG(COL_PLAYER | COL_SHIELD | COL_STATIC));
}

void CPensive::Next_Attack()
{
	if (m_pMagicBall_Attack != nullptr)
	{
		m_pMagicBall_Attack->Do_MagicBallState_To_Next();
	}
}

void CPensive::Next_SwordAttack()
{
	if (m_pMagicBall_Sword[--m_iSwordIndex] != nullptr)
	{
		m_pMagicBall_Sword[m_iSwordIndex]->Do_MagicBallState_To_Next();
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

		m_pEmissiveTexture_1 = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/PensivePaladin/T_GOL_TombProtector_1001_E.dds"));
		if (nullptr == m_pEmissiveTexture_1)
			throw TEXT("Emissive_1");
		m_pEmissiveTexture_2 = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/Anims/PensivePaladin/T_GOL_TombProtector_1002_E.dds"));
		if (nullptr == m_pEmissiveTexture_2)
			throw TEXT("Emissive_1");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 4000;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		/* Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 5.8f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(2.f, 4.f);
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
		lstrcpy(Desc.wszObjectLevel, TEXT("98"));
		lstrcpy(Desc.wszObjectName, TEXT("°­Ã¶ °ñ·½ Ææ½Ãºê"));

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
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
		const CBone* pBone = m_pModelCom->Get_Bone_Index(29);
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

		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Particle_Pensive_Skill_Distotion"),
			TEXT("Com_Pensive_Skill_Distotion"), reinterpret_cast<CComponent**>(&m_pSkillDistotionParticle))))
		{
			wstring wstrErrorMSG = TEXT("[CPensive] Failed Add_Components_Level : ");
			wstrErrorMSG += TEXT("Com_Pensive_Skill_Distotion");
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
		for (_int i = 0; i < m_pTorch.size(); i++)
		{
			Safe_Release(m_pTorch[i]);
		}
		Safe_Release(m_pDragonHead[0]);
		Safe_Release(m_pDragonHead[1]);
		Safe_Release(m_pDragonHead[2]);

		Safe_Release(m_pStateContext);
		Safe_Release(m_pEmissiveTexture_1);
		Safe_Release(m_pEmissiveTexture_2);
		Safe_Release(m_pMagicSlot);

		Safe_Release(m_pSkillDistotionParticle);

		Safe_Release(m_StateMachineDesc.pOwnerModel);
		Safe_Release(m_StateMachineDesc.pTarget);
		Safe_Release(m_pLight);
	}
}
