﻿#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Player_Camera.h"
#include "MagicSlot.h"
#include "Weapon_Player_Wand.h"
#include "StateContext.h"
#include "IdleState.h"

#include "Player_Information.h"

#include "ProtegoState.h"
#include "MagicCastingState.h"
#include "RollState.h"
#include "UseItemState.h"

#include "Armored_Troll.h"
#include "MagicBall.h"
#include "Enemy.h"
#include "UI_Group_Enemy_HP.h"

#include "Magic.h"

#include "UI_Group_Skill.h"
#include "UI_Group_SkillTap.h"

#include "MaximaPotion.h"
#include "FocusPotion.h"
#include "EdurusPotion.h"
#include "InvisibilityPotion.h"
#include "WiggenweldPotion.h"

#include "CoolTime.h"	
#include "Defence.h"

#include "WiggenweldPotion.h"
#include "Inventory.h"
#include "PotionTap.h"
#include "Tool.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

void CPlayer::Set_Protego_Collision(CTransform* _pTransform, CEnemy::ATTACKTYPE _eAttackType) const
{
	if (false == m_isUseProtego && m_pStateContext->Is_Current_State(TEXT("Protego")))
	{
		m_ProtegoStateDesc.isHit = true;

		switch (_eAttackType)
		{
		case CEnemy::ATTACK_NONE:
		{	}
		break;
		case CEnemy::ATTACK_LIGHT:
		{
			m_ProtegoStateDesc.iHitType = CProtegoState::HIT_LIGHT;
		}
		break;
		case CEnemy::ATTACK_HEAVY:
		{
			m_ProtegoStateDesc.iHitType = CProtegoState::HIT_HEABY;
		}
		break;
		case CEnemy::ATTACKTYPE_END:
		{	}
		break;

		default:
			break;
		}

		m_ProtegoStateDesc.pTransform = _pTransform;

		m_isCollisionEnterProtego = true;

		m_isUseProtego = true;
	}
}

void CPlayer::Set_Spell_Botton(_uint _Button, SPELL _eSpell)
{
	if (SKILLINPUT_1 > _Button || SKILLINPUT_4 < _Button || ACCIO > _eSpell || WINGARDIUMLEVIOSA < _eSpell)
	{
#ifdef _DEBUG
		MSG_BOX("Failed Set_Spell_Botton");
		return;
#endif // _DEBUG
	}

	m_pMagicSlot->Add_Magic_To_Skill_Slot(_Button, _eSpell);

	m_UI_Group_Skill_01->Set_SpellTexture((CUI_Group_Skill::KEYLIST)_Button, _eSpell);

	m_vecSpellCheck[_Button] = _eSpell;

	return;
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		MSG_BOX("Failed Player Initialize");

		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		MSG_BOX("Failed Player Add_Components");

		return E_FAIL;
	}

	m_vecCoolTimeRatio.resize(SKILLINPUT_END);
	m_vecSpellCheck.resize(SKILLINPUT_END);

	if (FAILED(Add_Magic()))
	{
		MSG_BOX("Failed Ready Player Magic");

		return E_FAIL;
	}

	if (FAILED(Ready_MeshParts()))
	{
		MSG_BOX("Failed Ready Player Mesh Parts");

		return E_FAIL;
	}

	if (FAILED(Ready_MagicDesc()))
	{
		MSG_BOX("Failed Ready _MagicDesc");

		return E_FAIL;
	}

	if (FAILED(Ready_StateMachine()))
	{
		MSG_BOX("Failed Ready_StateMachine");

		return E_FAIL;
	}

	if (FAILED(Ready_Camera()))
	{
		MSG_BOX("Failed Ready Player Camera");

		return E_FAIL;
	}

	m_pTransform->Set_Speed(1.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(180.f));
	m_pTransform->Set_RigidBody(m_pRigidBody);


	Bind_Notify();

	m_fClothPower = 3.0f;
	m_fClothPowerPlus = 1.0f;

	m_UI_Group_Skill_01->Set_SpellTexture(CUI_Group_Skill::FIRST, CONFRINGO);
	m_UI_Group_Skill_01->Set_SpellTexture(CUI_Group_Skill::SECOND, LEVIOSO);
	m_UI_Group_Skill_01->Set_SpellTexture(CUI_Group_Skill::THIRD, NCENDIO);
	m_UI_Group_Skill_01->Set_SpellTexture(CUI_Group_Skill::FOURTH, DIFFINDO);

	m_fRotationSpeed = 2.0f;

	m_iMoveType = (_uint)MOVETYPE_NONE;

	m_iActionType = (_uint)ACTION_NONE;

	m_pCustomModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"));
	m_pCustomModel->Change_Animation(TEXT("Hu_BM_RF_Idle_anm"), CModel::OTHERBODY);

	m_vLevelInitPosition[LEVEL_CLIFFSIDE] = _float3(25.f, 3.f, 22.5f);
	m_vLevelInitPosition[LEVEL_VAULT] = _float3(7.0f, 0.02f, 7.5f);
	m_vLevelInitPosition[LEVEL_SMITH] = _float3(30.f, 3.f, 15.f); // 占썩본 占쏙옙치
	//m_vLevelInitPosition[LEVEL_SMITH] = _float3(94.5f, 7.2f, 78.f); // 占쏙옙占쏙옙 占쏙옙占쏙옙占싱쇽옙 占쌕뤄옙 占쏙옙
	m_vLevelInitPosition[LEVEL_SKY] = _float3(88.8f, 12.5f, 69.8f); // 占쏙옙占쏙옙 占쏙옙占쏙옙占싱쇽옙 占쌕뤄옙 占쏙옙

	m_fTargetViewRange = 1.0f;


	//m_vecEaseList.resize(CEase::EASE_END);

	//m_vecEaseList[CEase::IN_QUAD] = New ("IN_QUAD");

	//IN_QUAD, OUT_QUAD, INOUT_QUAD_,
	//	IN_SINE, OUT_SINE, INOUT_SINE,
	//	IN_QUINT, OUT_QUINT, INOUT_QUINT,
	//	IN_QUART, OUT_QUART, INOUT_QUART,
	//	NONE_LINEAR, IN_LINEAR, OUT_LINEAR, INOUT_LINEAR,
	//	IN_EXPO, OUT_EXPO, INOUT_EXPO,
	//	IN_ELASTIC, OUT_ELASTIC, INOUT_ELASTIC,
	//	IN_CUBIC, OUT_CUBIC, INOUT_CUBIC,
	//	IN_CIRC, OUT_CIRC, INOUT_CIRC,
	//	IN_BOUNCE, OUT_BOUNCE, INOUT_BOUNCE,
	//	IN_BACK, OUT_BACK, INOUT_BACK,
	
	return S_OK;
}

HRESULT CPlayer::Initialize_Level(_uint iCurrentLevelIndex)
{
	m_pTransform->Set_Position(m_vLevelInitPosition[iCurrentLevelIndex]);
	m_eLevelID = (LEVELID)iCurrentLevelIndex;
	m_pRigidBody->Clear_Force(PxForceMode::eVELOCITY_CHANGE);
	/*if (FAILED(__super::Initialize_Level(iCurrentLevelIndex)))
		return E_FAIL;*/

	if (nullptr != m_pTarget)
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}

	if (nullptr != m_pTargetTransform)
	{
		Safe_Release(m_pTargetTransform);
		m_pTargetTransform = nullptr;
	}


	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	BEGININSTANCE;

	//占시뤄옙占싱억옙 카占쌨띰옙 占싣니띰옙占?
	if (false == pGameInstance->Is_Current_Camera(TEXT("Player_Camera")))
	{
		ENDINSTANCE;

		return;
	}

	ENDINSTANCE;

	if (false == m_pStateContext->Is_Current_State(TEXT("Protego")))
	{
		m_isUseProtego = false;
	}

	//스킬 쿨 갱신용도
	Update_Skill_CoolTime();

	//타겟을 초기화시키는 용도
	if (nullptr != m_pTarget && m_pTarget->isDead())
	{
		Clear_Target();
	}

	//타겟을 잡는 용도
	Update_Target_Angle();

	__super::Tick(fTimeDelta);

	Prepare_Protego();

	UpdateLookAngle();

	Key_Input(fTimeDelta);
	//m_pStateContext->Tick(fTimeDelta)
	Go_Protego(&m_ProtegoStateDesc);

	Fix_Mouse();
	Update_Cloth(fTimeDelta);

	m_pCustomModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
	m_pCustomModel->Play_Animation(fTimeDelta, CModel::UNDERBODY);
	m_pCustomModel->Play_Animation(fTimeDelta, CModel::OTHERBODY);


	//占쏙옙占?占쏙옙占쏙옙占쏙옙트
	if (nullptr != m_pFrncSpellToggle)
	{
		m_pFrncSpellToggle(nullptr);
	}

	if (true == m_isPreLumos && m_isPreLumos != m_isLumosOn)
	{
		Lumos();
		Next_Spell_Action();
	}

	m_isPreLumos = m_isLumosOn;

	//m_pCooltime->Tick(fTimeDelta);
	//Potion_Duration(fTimeDelta);


#ifdef _DEBUG
	ADD_IMGUI([&] { this->Tick_TestShake(); });
#endif // _DEBUG
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	BEGININSTANCE;

	//占시뤄옙占싱억옙 카占쌨띰옙 占싣니띰옙占?
	if (false == pGameInstance->Is_Current_Camera(TEXT("Player_Camera")))
	{
		ENDINSTANCE;

		return;
	}

	ENDINSTANCE;

	__super::Late_Tick(fTimeDelta);

	//m_pStateContext->Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_DEPTH, this);

#ifdef _DEBUG
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
#endif // _DEBUG
	}

	if (nullptr != m_pTarget)
	{
		if (true == m_pTarget->isDead())
		{
			Safe_Release(m_pTargetTransform);
			Safe_Release(m_pTarget);
			m_pTargetTransform = nullptr;
			m_pTarget = nullptr;
			return;
		}

		CEnemy* pEnemy = dynamic_cast<CEnemy*>(m_pTarget);
		if (nullptr != pEnemy)
			pEnemy->Get_UI_Enemy_HP()->Late_Tick(fTimeDelta);
	}
}

void CPlayer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("Attack")))
	{
		CEnemy::COLLISIONREQUESTDESC* pDesc = static_cast<CEnemy::COLLISIONREQUESTDESC*>(CollisionEventDesc.pArg);

		if (nullptr == pDesc ||
			CEnemy::ATTACK_NONE == pDesc->eType)
		{
			return;
		}

		//Protego
		if (m_pStateContext->Is_Current_State(TEXT("Protego")))
		{
		}
		//회피시 무시
		else if (m_pStateContext->Is_Current_State(TEXT("Roll")))
		{
		}
		//피격중인 상태일 경우 무시
		else if (m_pStateContext->Is_Current_State(TEXT("Hit")))
		{
		}
		//에두루스 마법약 상태일때 슈퍼아머 혹은 무적
		else if (m_isDefUp)
		{
		}
		//Hit
		else
		{
			CHitState::HITSTATEDESC HitStateDesc;

			switch (pDesc->eType)
			{
			case CEnemy::ATTACK_NONE:
			{	}
			break;
			case CEnemy::ATTACK_LIGHT:
			{
				if (m_pStateContext->Is_Current_State(TEXT("Jump")))
				{
					HitStateDesc.iHitType = CHitState::HIT_HEABY;
				}
				else
				{
					HitStateDesc.iHitType = CHitState::HIT_LIGHT;
				}
			}
			break;
			case CEnemy::ATTACK_HEAVY:
			{
				HitStateDesc.iHitType = CHitState::HIT_HEABY;
			}
			break;
			case CEnemy::ATTACKTYPE_END:
			{	}
			break;

			default:
				break;
			}

			HitStateDesc.pTransform = pDesc->pEnemyTransform;

			Go_Hit(&HitStateDesc);

			//체占쏙옙 占쏙옙占쏙옙
			m_pPlayer_Information->fix_HP((pDesc->iDamage) * -1);
		}
	}
	else if (wstring::npos != wstrCollisionTag.find(TEXT("Magic_Ball")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);

		//Protego
		if (m_pStateContext->Is_Current_State(TEXT("Protego")))
		{
		}
		//회피시 무시
		else if (m_pStateContext->Is_Current_State(TEXT("Roll")))
		{
		}
		//피격중인 상태일 경우 무시
		//else if (m_pStateContext->Is_Current_State(TEXT("Hit")))
		//{
		//}
		//에두루스 마법약 상태일때 슈퍼아머 혹은 무적
		else if (m_isDefUp)
		{
		}
		//Hit
		else
		{
			CHitState::HITSTATEDESC HitStateDesc;

			HitStateDesc.iHitType = CHitState::HIT_LIGHT;
			if (nullptr == pDesc ||
				nullptr == pDesc->pTransform)
				return;

			HitStateDesc.pTransform = pDesc->pTransform;

			Go_Hit(&HitStateDesc);

			//체력 수정
			m_pPlayer_Information->fix_HP((pDesc->iDamage) * -1);
		}
	}
}

void CPlayer::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
}

void CPlayer::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
}

HRESULT CPlayer::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		if (CCustomModel::HAIR == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShader, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShader->Begin("HairMesh");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else if (CCustomModel::ROBE == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShader, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShader->Begin("AnimMeshNonCull");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else if (CCustomModel::HEAD == iPartsIndex ||
			CCustomModel::ARM == iPartsIndex)
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShader, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShader->Begin("AnimMesh");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
		else
		{
			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				m_pCustomModel->Bind_BoneMatrices(m_pShader, "g_BoneMatrices", iPartsIndex, i);

				m_pCustomModel->Bind_Material(m_pShader, "g_DiffuseTexture", iPartsIndex, i, DIFFUSE);
				m_pCustomModel->Bind_Material(m_pShader, "g_NormalTexture", iPartsIndex, i, NORMALS);

				m_pShader->Begin("AnimMeshColor");

				m_pCustomModel->Render(iPartsIndex, i);
			}
		}
	}

	//if(m_isInvisible) 占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 ShaderPass 占쌕뀐옙占쌍몌옙 占심것곤옙占쏙옙.

	return S_OK;
}

HRESULT CPlayer::Render_Depth(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	if (FAILED(SetUp_ShadowShaderResources(LightViewMatrix, LightProjMatrix)))
		return E_FAIL;

	for (_uint iPartsIndex = 0; iPartsIndex < CCustomModel::MESH_END; ++iPartsIndex)
	{
		_uint		iNumMeshes = m_pCustomModel->Get_NumMeshes(iPartsIndex);

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			m_pCustomModel->Bind_BoneMatrices(m_pShadowShader, "g_BoneMatrices", iPartsIndex, i);

			m_pShadowShader->Begin("Shadow");

			m_pCustomModel->Render(iPartsIndex, i);
		}
	}

	return S_OK;
}

void CPlayer::Potion_Duration(_float fTimeDelta)
{
	//if (m_isPowerUp)
	//{
	//	m_pMaximaPotion->Duration(fTimeDelta);
	//	if(!m_isPowerUp)
	//	Safe_Release(m_pMaximaPotion);

	//}

	//if (m_isDefUp)
	//{
	//	m_pEdurusPotion->Duration(fTimeDelta);
	//	if (!m_isDefUp)
	//		Safe_Release(m_pEdurusPotion);
	//}
	//if (m_isInvisible)
	//{
	//	m_pInvisibilityPotion->Duration(fTimeDelta);
	//	if (!m_isInvisible)
	//		Safe_Release(m_pInvisibilityPotion);
	//}
	/*if (m_isFocusOn)
	{
		m_pFocusPotion->Duration(fTimeDelta);
		if(!m_isFocusOn)
		Safe_Release(m_pFocusPotion);

	}*/

}

void CPlayer::On_Maigc_Throw_Data(void* data) const
{
	if (static_cast<CMagicBall::COLLSIONREQUESTDESC*>(data)->eMagicTag == LUMOS)
	{
		m_pFrncSpellToggle = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(data)->Action;
	}
}

_bool CPlayer::Is_Action_Camera_Playing()
{
	return !m_pPlayer_Camera->Is_Finish_Animation();
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
	}

	/* For.Com_Shader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* For.Com_ShadowShader */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ShadowAnimMesh"),
		TEXT("Com_ShadowShader"), reinterpret_cast<CComponent**>(&m_pShadowShader))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* For.Com_Model_CustomModel_Player */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_CustomModel_Player"),
		TEXT("Com_Model_CustomModel_Player"), reinterpret_cast<CComponent**>(&m_pCustomModel))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* For.Com_StateContext */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateContext"),
		TEXT("Com_StateContext"), reinterpret_cast<CComponent**>(&m_pStateContext))))
	{
		__debugbreak();
		return E_FAIL;
	}

	const CBone* pBone = m_pCustomModel->Get_Bone(TEXT("SKT_RightHand"));
	if (nullptr == pBone)
		throw TEXT("pBone is nullptr");

	CWeapon_Player_Wand::CWEAPON_PLAYER_WAND_DESC ParentMatrixDesc;
	ParentMatrixDesc.ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.ParentMatrixDesc.PivotMatrix = m_pCustomModel->Get_PivotFloat4x4();
	ParentMatrixDesc.ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();
	ParentMatrixDesc.pisLightOn = &m_isLumosOn;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Weapon_Player_Wand"),
		TEXT("Com_Weapon"), reinterpret_cast<CComponent**>(&m_pWeapon), &ParentMatrixDesc)))
		throw TEXT("Com_Weapon");

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MagicSlot"),
		TEXT("Com_MagicSlot"), reinterpret_cast<CComponent**>(&m_pMagicSlot))))
	{
		__debugbreak();
		return E_FAIL;
	}

	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = false;
	RigidBodyDesc.isGravity = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vInitRotation = m_pTransform->Get_Quaternion();
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxCapsuleGeometry MyGeometry = PxCapsuleGeometry(0.5f, 0.5f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.vOffsetPosition = _float3(0.f, MyGeometry.halfHeight + MyGeometry.radius, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 105 / 255.f, 180 / 255.f, 1.f); // hot pink
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_PLAYER;
	RigidBodyDesc.eCollisionFlag = COL_ENEMY_RANGE | COL_ENEMY_ATTACK | COL_TRIGGER | COL_STATIC | COL_MAGIC;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Player_Default");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		__debugbreak();
		return E_FAIL;
	}

	m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);
	m_pRigidBody->Get_RigidBodyActor()->setAngularDamping(1.f);

	/* Com_Player_Information */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Player_Information"),
		TEXT("Com_Player_Information"), reinterpret_cast<CComponent**>(&m_pPlayer_Information))))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* Com_UI_Group_Skill_1 */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Skill"),
		TEXT("Com_UI_Group_Skill_1"), reinterpret_cast<CComponent**>(&m_UI_Group_Skill_01), &m_vecCoolTimeRatio)))
	{
		__debugbreak();
		return E_FAIL;
	}

	/* Com_UI_Group_SkillTap */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_SkillTap"),
		TEXT("Com_UI_Group_SkillTap"), reinterpret_cast<CComponent**>(&m_UI_Group_SkillTap))))
	{
		__debugbreak();
		return E_FAIL;
	}


	//_int DefValue = 15;
	//if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_EndurusPotion"),
	//	TEXT("Com_EndurusPotion"), reinterpret_cast<CComponent**>(&m_pEndurusPotion),&DefValue)))
	//{
	//	__debugbreak();
	//	return E_FAIL;
	//}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime))))
	{
		__debugbreak();
		return E_FAIL;
	}
	CDefence::DEFFENCEDESC Def;
	Def.iDeffence = 0;

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Defence"),
		TEXT("Com_Defence"), reinterpret_cast<CComponent**>(&m_pDefence), &Def)))
	{
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	_float3 vHairColor = { 0.2f, 0.2f, 0.2f };
	if (FAILED(m_pShader->Bind_RawValue("g_vHairColor", &vHairColor, sizeof(_float3))))
		return E_FAIL;

	_float4 vColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShadowShaderResources(_float4x4 LightViewMatrix, _float4x4 LightProjMatrix)
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", &LightViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", &LightProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPlayer::Add_Magic()
{
	CMagic::MAGICDESC magicInitDesc;

	// 占쏙옙占쏙옙占쏙옙占?
	{
		magicInitDesc.eBuffType = BUFF_LEVIOSO;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = LEVIOSO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占쏙옙占쏙옙占쏙옙占쏙옙
	{
		magicInitDesc.eBuffType = BUFF_CONFRINGO;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = CONFRINGO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占실니쇽옙
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = FINISHER;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 500;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 3.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占싸쇽옙占쏙옙占?
	{
		magicInitDesc.eBuffType = BUFF_NCENDIO;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = NCENDIO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 300;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占쏙옙占?
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = LUMOS;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 30.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占싣몌옙占쏙옙占쏙옙占쏙옙占쏙옙
	// 占쌈듸옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쏙옙占쏙옙占싹댐옙.
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = ARRESTOMOMENTUM;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 30.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占싣억옙占쏙옙
	// 占쏙옙占쌩울옙 占쏙옙占시?占쏙옙占쏙옙占쏙옙占?占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쏙옙占쏙옙占싹댐옙.
	{
		magicInitDesc.eBuffType = BUFF_ACCIO;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
		magicInitDesc.eMagicTag = ACCIO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占쏜센듸옙
	// 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占싹댐옙 占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쏙옙占쌌니댐옙.
	{
		magicInitDesc.eBuffType = BUFF_DESCENDO;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
		magicInitDesc.eMagicTag = DESCENDO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 30;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占시몌옙占썸도
	// 占쏙옙占쌩울옙 占쏙옙占?占싼뱄옙占쏙옙 x占쏙옙占쏙옙占쏙옙 회占쏙옙占쏙옙키占쏙옙 占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쏙옙占쏙옙占싹댐옙.
	{
		magicInitDesc.eBuffType = BUFF_FLIPENDO;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_PURPLE;
		magicInitDesc.eMagicTag = FLIPENDO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.3f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占쏙옙占쏙옙占썹리占싣몌옙占쏙옙占쏙옙
	// 占쏙옙占썩를 占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쌩곤옙占쌉니댐옙.
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = EXPELLIARMUS;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.6f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 占쏙옙占썰리占쏙옙
	// 占쏙옙占쏙옙占쏙옙 占쏙옙占쌍몌옙 占심억옙 占쌕몌옙 占쏙옙占쏙옙 占싸울옙占?占쏙옙占쏙옙占?占쏙옙占쏙옙占쌉니댐옙.
	// 占쏙옙占쏙옙 占쌩곤옙占쌉니댐옙.
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_CURSE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = IMPERIO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 100;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 크占쏙옙첼占?
	// 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쌕몌옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙탄占실댐옙 占쏙옙占쏙옙체占쏙옙 占쌩삼옙占쌌니댐옙.
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_CURSE;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = CRUCIO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//占쏙옙占쏙옙占쏙옙占쏙옙占쏙옙
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = STUPEFY;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//占쏙옙占심듸옙
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = DIFFINDO;
		magicInitDesc.fInitCoolTime = 5.f;
		magicInitDesc.iDamage = 200;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	//占쏙옙占쌕몌옙占쏙옙
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_POWER;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = BOMBARDA;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 500;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}


	m_pMagicSlot->Add_Magic_To_Basic_Slot(2, LUMOS);
	m_pMagicSlot->Add_Magic_To_Basic_Slot(3, FINISHER);
	m_pMagicSlot->Add_Magic_To_Basic_Slot(4, STUPEFY);

	Set_Spell_Botton(0, BOMBARDA);
	Set_Spell_Botton(1, LEVIOSO);
	Set_Spell_Botton(2, DESCENDO);
	Set_Spell_Botton(3, CRUCIO);

	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

#pragma region 카占쌨띰옙 占쏙옙占쏙옙 占쌓쏙옙트
	//	if (pGameInstance->Get_DIKeyState(DIK_J, CInput_Device::KEY_DOWN))
	//{
	//	pGameInstance->Set_Camera(TEXT("Player_Camera"),2.0f);
	//}
	//
	//
	//if (pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	//{
	//	pGameInstance->Set_Camera(TEXT("Other_Camera"), 2.0f);
	//}  
#pragma endregion


#ifdef _DEBUG
	//카占쌨띰옙 占쏙옙占쏙옙크 占쌓쏙옙트
	if (pGameInstance->Get_DIKeyState(DIK_K, CInput_Device::KEY_DOWN))
	{
		_float3 vAxis = _float3(m_fx, m_fy, m_fz);

		vAxis.Normalize();

		pGameInstance->Set_Shake(
			(CCamera_Manager::SHAKE_TYPE)m_iShake_Type,
			(CCamera_Manager::SHAKE_AXIS)m_iShake_Axis,
			(CEase::EASE)m_iEase,
			m_fShakeSpeed,
			m_fShakeDuration,
			m_fShakePower,
			(CCamera_Manager::SHAKE_POWER)m_iShakePower,
			vAxis);
	}
#endif // _DEBUG

	//占쏙옙占쏙옙
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
	{
		Find_Target_For_ViewSpace();
	}

	//눌렀을 때
	if (pGameInstance->Get_DIKeyState(DIK_TAB, CInput_Device::KEY_DOWN))
	{
		Go_Use_Item();
		/*switch (m_pPlayer_Information->Get_PotionTap()->Get_CurTool()->Get_ItemID())
		{
		case Client::ITEM_ID_EDURUS_POTION:
		case Client::ITEM_ID_FOCUS_POTION:
		case Client::ITEM_ID_MAXIMA_POTION:
		case Client::ITEM_ID_INVISIBILITY_POTION:
		case Client::ITEM_ID_THUNDERBEW_POTION:
		case Client::ITEM_ID_FELIX_FELICIS_POTION:
		{
			m_pCustomModel->Change_Animation(TEXT("Drink_Potion_Throw"), CModel::OTHERBODY2);
		}
			break;
		case Client::ITEM_ID_CHINESE_CHOMPING_CABBAGE:
		{
			m_pCustomModel->Change_Animation(TEXT("Throw_Plant"), CModel::OTHERBODY2);
		}
			break;
		default:
			break;
		}*/
	}

	//case Client::ITEM_ID_WIGGENWELD_POTION:

#pragma region 占쏙옙占쏙옙占쏙옙트 占쏙옙占쏙옙 키 占쌉뤄옙

	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL, CInput_Device::KEY_DOWN))
	{
		CRollState::tagRollStateDesc RollStateDesc;

		RollStateDesc.IsBlink = false;

		Go_Roll(&RollStateDesc);
	}


	if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		CRollState::tagRollStateDesc RollStateDesc;

		RollStateDesc.IsBlink = true;

		Go_Roll(&RollStateDesc);
		//Go_Jump();
	}

	if (true == m_isReadySpell)
	{
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc = CMagicCastingState::MAGICCASTINGSTATEDESC();

		MagicCastingStateDesc.pisReadySpell = &m_isReadySpell;

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_BASIC;

		// 占쏙옙타 : 타占쌉곤옙 占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占싼곤옙占쌔댐옙.
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			Go_MagicCast(&MagicCastingStateDesc);
		}

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_NORMAL;

		//占썩본 占쏙옙占쏙옙
		/*if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(SKILLINPUT_1))
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_1(); };

			Go_MagicCast(&MagicCastingStateDesc);
		}
		if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(SKILLINPUT_2))
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_2(); };

			Go_MagicCast(&MagicCastingStateDesc);
		}
		if (pGameInstance->Get_DIKeyState(DIK_3, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(SKILLINPUT_3))
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_3(); };

			Go_MagicCast(&MagicCastingStateDesc);
		}
		if (pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(SKILLINPUT_4))
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_4(); };

			Go_MagicCast(&MagicCastingStateDesc);
		}*/

		for (size_t i = 0; i < SKILLINPUT_END; i++)
		{
			if (pGameInstance->Get_DIKeyState(DIK_1 + i, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(i))
			{
				//占쌓쇽옙
				MagicCastingStateDesc.iSpecialAction = Special_Action(i);

				//占쌉쇽옙 占쏙옙占쏙옙占쏙옙
				switch (i)
				{
				case Client::CPlayer::SKILLINPUT_1:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_1(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_2:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_2(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_3:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_3(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_4:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_4(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_END:
					break;
				default:
					break;
				}








				Go_MagicCast(&MagicCastingStateDesc);
			}
		}




		/*if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN))
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Lumos(); };

			Go_MagicCast(&MagicCastingStateDesc);
		}*/

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_FINISHER;

		//占쏙옙占쏙옙 占쌩곤옙占쏙옙
		if (pGameInstance->Get_DIKeyState(DIK_X, CInput_Device::KEY_DOWN) && m_pPlayer_Information->Is_Use_Fnisher() && nullptr != m_pTarget)
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Finisher(); };

			static_cast<CEnemy*>(m_pTarget)->Ready_Hit_Finisher();

			m_pPlayer_Information->Using_Fnisher();

			Go_MagicCast(&MagicCastingStateDesc);

			m_pPlayer_Camera->Change_Animation(TEXT("Cam_Finisher_Lightning_01_anm"));
		}

		if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN) && false == m_pStateContext->Is_Current_State(TEXT("Protego")))
		{
			m_isCollisionEnterProtego = true;
			Go_Protego(nullptr);
		}
	}
#pragma endregion

	//루모스
	if (pGameInstance->Get_DIKeyState(DIK_Z, CInput_Device::KEY_DOWN) &&
		(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
			m_pStateContext->Is_Current_State(TEXT("Jump")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Loop"))))
	{
		//켜기
		if (false == m_isLumosOn)
		{
			m_pCustomModel->Change_Animation(TEXT("Lumos_Start"), CModel::OTHERBODY);
			m_isLumosOn = true;
		}
		//끄기
		else if (true == m_isLumosOn)
		{
			m_pCustomModel->Change_Animation(TEXT("Lumos_Stop"), CModel::OTHERBODY);
			m_isLumosOn = false;
		}
	}


	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isFixMouse)
			m_isFixMouse = false;
		else
			m_isFixMouse = true;
	}

	//스킬탭
	if (pGameInstance->Get_DIKeyState(DIK_T, CInput_Device::KEY_DOWN))
	{
		m_isOpenSkillTap = !m_isOpenSkillTap;
		m_UI_Group_SkillTap->Set_isOpen(m_isOpenSkillTap);
	}

	if (pGameInstance->Get_DIKeyState(DIK_G, CInput_Device::KEY_DOWN))
	{
		Go_Use_Potion();
	}

	/*if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 10.f, PxForceMode::eIMPULSE);
	}*/

#ifdef _DEBUG
	if (pGameInstance->Get_DIKeyState(DIK_UP))
	{
		m_pTransform->Go_Straight(fTimeDelta * 100);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN))
	{
		m_pTransform->Go_Backward(fTimeDelta * 100);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT))
	{
		//m_pTransform->Turn(_float3(0.f, -1.f, 0.f), fTimeDelta);
		m_pTransform->Go_Left(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT))
	{
		////m_pTransform->Turn(_float3(0.f, 1.f, 0.f), fTimeDelta);
		m_pTransform->Go_Right(fTimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_K, CInput_Device::KEY_DOWN))
	{
		m_pPlayer_Information->Get_PotionTap()->Add_Potion(POTIONTAP::FOCUS_POTION);
		m_pPlayer_Information->Get_PotionTap()->Add_Potion(POTIONTAP::ENDURUS_POTION);
		m_pPlayer_Information->Get_PotionTap()->Add_Potion(POTIONTAP::INVISIBILITY_POTION);
		m_pPlayer_Information->Get_PotionTap()->Add_Potion(POTIONTAP::MAXIMA_POTION);
		m_pPlayer_Information->Get_PotionTap()->Add_Potion(POTIONTAP::THUNDERBREW_POTION);
	}
#endif //_DEBUG
	if (pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	{
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_Dust01"), m_pTransform->Get_Position());
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_Dust02"), m_pTransform->Get_Position());
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_RockChunksRough"), m_pTransform->Get_Position());

		//m_pPlayer_Information->Get_Inventory()->Add_Item(pItem, pItem->Get_Type());
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_ASHWINDER_EGGS);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_DITTANY_LEAVES);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_DUGBOG_TONGUE);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LEAPING_TOADSTOOL_CAPS);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LACEWING_FLIES);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_KNOTGRASS);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_HORKLUMP_JUICE);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_LEECH_JUICE);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_MALLOWSWEET);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_MOONSTONE);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_FLUXWEED_STEM);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_SPIDER_FANG);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_TROLL_BOGEYS);
		m_pPlayer_Information->Get_Inventory()->Add_Item(ITEM_ID::ITEM_ID_SHRIVELFIG);
	}

	ENDINSTANCE;
}

void CPlayer::Fix_Mouse()
{
	if (false == m_isFixMouse)
	{
		return;
	}


	POINT	ptMouse{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

HRESULT CPlayer::Ready_MeshParts()
{
	//Hair
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Hair"),
		CCustomModel::HAIR)))
	{
		MSG_BOX("Failed Add MeshPart Hair");

		return E_FAIL;
	}

	//Head
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Head"),
		CCustomModel::HEAD)))
	{
		MSG_BOX("Failed Add MeshPart Head");

		return E_FAIL;
	}

	//Arm
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Arm"),
		CCustomModel::ARM)))
	{
		MSG_BOX("Failed Add MeshPart Arm");

		return E_FAIL;
	}

	//Robe
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Robe01"),
		CCustomModel::ROBE, TEXT("../../Resources/GameData/ClothData/Test.cloth"))))
	{
		MSG_BOX("Failed Add MeshPart Robe");

		return E_FAIL;
	}

	//Top
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Top"),
		CCustomModel::TOP)))
	{
		MSG_BOX("Failed Add MeshPart Top");

		return E_FAIL;
	}

	//Pants
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Pants"),
		CCustomModel::PANTS)))
	{
		MSG_BOX("Failed Add MeshPart Pants");

		return E_FAIL;
	}

	//Socks
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Socks"),
		CCustomModel::SOCKS)))
	{
		MSG_BOX("Failed Add MeshPart Socks");

		return E_FAIL;
	}

	//Shoes
	if (FAILED(m_pCustomModel->Add_MeshParts(
		LEVEL_STATIC,
		TEXT("Prototype_Component_MeshPart_Player_Shoes"),
		CCustomModel::SHOES)))
	{
		MSG_BOX("Failed Add MeshPart Shoes");

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Ready_Camera()
{
	_uint iBoneIndex{ 0 };

	m_pCustomModel->Find_BoneIndex(TEXT("SKT_HeadCamera"), &iBoneIndex);

	CCamera::CAMERADESC CameraDesc;

	CameraDesc.m_fAspect = _float(g_iWinSizeX) / _float(g_iWinSizeY);
	CameraDesc.m_fFovY = XMConvertToRadians(90.f);
	CameraDesc.m_fNear = 0.1f;
	CameraDesc.m_fFar = 1000.f;

	CPlayer_Camera::PLAYERCAMERADESC PlayerCameraDesc;

	PlayerCameraDesc.CameraDesc = CameraDesc;
	PlayerCameraDesc.pPlayerTransform = m_pTransform;
	PlayerCameraDesc.ppTargetTransform = &m_pTargetTransform;
	PlayerCameraDesc.IsMove = &m_isFixMouse;

	m_pPlayer_Camera = CPlayer_Camera::Create(m_pDevice, m_pContext, &PlayerCameraDesc);

	Safe_AddRef(m_pPlayer_Camera);

	NULL_CHECK_RETURN_MSG(m_pPlayer_Camera, E_FAIL, TEXT("Failed Create Player Camera"));

	BEGININSTANCE;

	pGameInstance->Add_Camera(TEXT("Player_Camera"), (CCamera*)m_pPlayer_Camera);

	pGameInstance->Add_Camera(TEXT("Other_Camera"), CPlayer_Camera::Create(m_pDevice, m_pContext, &PlayerCameraDesc));

	pGameInstance->Set_Camera(TEXT("Player_Camera"));

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPlayer::Ready_MagicDesc()
{
	CMagic::MAGICDESC magicInitDesc;
	magicInitDesc.eBuffType = BUFF_ATTACK_LIGHT;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_NOTHING;
	magicInitDesc.eMagicTag = BASICCAST;
	magicInitDesc.fInitCoolTime = 0.f;
	magicInitDesc.iDamage = 10;
	magicInitDesc.fLifeTime = 0.6f;

	m_BasicDesc_Light = magicInitDesc;

	magicInitDesc.eBuffType = BUFF_ATTACK_HEAVY;
	magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
	magicInitDesc.eMagicType = CMagic::MT_NOTHING;
	magicInitDesc.eMagicTag = BASICCAST;
	magicInitDesc.fInitCoolTime = 0.f;
	magicInitDesc.iDamage = 10;
	magicInitDesc.fLifeTime = 0.6f;

	m_BasicDesc_Heavy = magicInitDesc;

	return S_OK;
}

void CPlayer::MagicTestTextOutput()
{
}

void CPlayer::UpdateLookAngle()
{
	BEGININSTANCE;

	m_isDirectionKeyPressed = false;

	_float3 vNextLook{};

	if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_DOWN))
	{
		vNextLook = m_pPlayer_Camera->Get_CamLookXZ();
		m_isDirectionKeyPressed = true;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_S, CInput_Device::KEY_DOWN))
	{
		vNextLook = -m_pPlayer_Camera->Get_CamLookXZ();
		m_isDirectionKeyPressed = true;
	}

	else if (pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_D, CInput_Device::KEY_DOWN))
	{
		vNextLook = m_pPlayer_Camera->Get_CamRightXZ();
		m_isDirectionKeyPressed = true;
	}

	else if (pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_PRESSING) ||
		pGameInstance->Get_DIKeyState(DIK_A, CInput_Device::KEY_DOWN))
	{
		vNextLook = -m_pPlayer_Camera->Get_CamRightXZ();
		m_isDirectionKeyPressed = true;
	}

	_float3 vPlayerLook = m_pTransform->Get_Look();

	vPlayerLook = XMVectorSetY(vPlayerLook, 0.0f);

	vPlayerLook.Normalize();

	_float fLookAngle = vPlayerLook.Dot(vNextLook);

	if (1.0f < fLookAngle)
	{
		fLookAngle = 1.0f;
	}

	m_fLookAngle = acosf(fLookAngle);

	if (0.0f > vPlayerLook.Cross(vNextLook).y)
	{
		m_fLookAngle *= -1;
	}

	ENDINSTANCE;
}

HRESULT CPlayer::Ready_StateMachine()
{
	//채占쏙옙占쏙옙 占쌍다몌옙 占싫되니깍옙 E_Fail
	if (true == m_StateMachineDesc.IsValid())
	{
		MSG_BOX("Failed Ready_StateMachine");
		return E_FAIL;
	}

	m_StateMachineDesc.pOwnerModel = m_pCustomModel;
	m_StateMachineDesc.pPlayerTransform = m_pTransform;
	m_StateMachineDesc.pfRotaionSpeed = &m_fRotationSpeed;
	m_StateMachineDesc.pfTargetAngle = &m_fTargetAngle;
	m_StateMachineDesc.pisDirectionPressed = &m_isDirectionKeyPressed;

	m_StateMachineDesc.pisFinishAnimation = &m_isFinishAnimation;
	m_StateMachineDesc.piActionType = &m_iActionType;
	m_StateMachineDesc.piMoveType = &m_iMoveType;
	m_StateMachineDesc.pOwnerLookAngle = &m_fLookAngle;
	m_StateMachineDesc.pfuncFinishAnimation = [&] { (*this).Finish_Animation(); };
	m_StateMachineDesc.pLumosOn = &m_isLumosOn;
	m_StateMachineDesc.ppTarget = &m_pTarget;
	m_StateMachineDesc.pIsFlying = &m_isFlying;

	Safe_AddRef(m_StateMachineDesc.pOwnerModel);
	Safe_AddRef(m_StateMachineDesc.pPlayerTransform);


	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Idle_State"),
		TEXT("Idle"),
		TEXT("Prototype_Component_State_Idle"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Idle State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Move_Turn_State"),
		TEXT("Move Turn"),
		TEXT("Prototype_Component_State_Move_Turn"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Move Turn State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Move_Start_State"),
		TEXT("Move Start"),
		TEXT("Prototype_Component_State_Move_Start"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Move Start State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Move_Loop_State"),
		TEXT("Move Loop"),
		TEXT("Prototype_Component_State_Move_Loop"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Move Loop State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Roll_State"),
		TEXT("Roll"),
		TEXT("Prototype_Component_State_Roll"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Roll State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Jump_State"),
		TEXT("Jump"),
		TEXT("Prototype_Component_State_Jump"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Jump State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Hard_Land_State"),
		TEXT("Hard Land"),
		TEXT("Prototype_Component_State_Hard_Land"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Hard Land State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Magic_Cast_State"),
		TEXT("Magic_Cast"),
		TEXT("Prototype_Component_State_Magic_Casting"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Magic_Cast State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Protego_State"),
		TEXT("Protego"),
		TEXT("Prototype_Component_State_ProtegoState"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Protego State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Hit_State"),
		TEXT("Hit"),
		TEXT("Prototype_Component_State_Hit"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Hit State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_Standing_State"),
		TEXT("Standing"),
		TEXT("Prototype_Component_State_Standing"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add Standing State");

		return E_FAIL;
	}

	if (FAILED(m_pStateContext->Add_StateMachine(
		LEVEL_STATIC,
		TEXT("Com_Player_UseItem_State"),
		TEXT("UseItem"),
		TEXT("Prototype_Component_State_UseItem"),
		&m_StateMachineDesc)))
	{
		MSG_BOX("Failed Add UseItem State");

		return E_FAIL;
	}

#pragma region Flying

	//try
	//{
	//	//지팡이 탑승 스테이트
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Broom_Begin"),
	//		TEXT("Broom_Begin"),
	//		TEXT("Prototype_Component_State_Broom_Begin"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Broom_Begin State");

	//	//호버 idle
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Hover_Idle"),
	//		TEXT("Hover_Idle"),
	//		TEXT("Prototype_Component_State_Hover_Idle"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Hover_Idle State");

	//	//호버 move
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Hover_Move"),
	//		TEXT("Hover_Move"),
	//		TEXT("Prototype_Component_State_Hover_Move"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Hover_Move State");

	//	//호버 turn
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Hover_Turn"),
	//		TEXT("Hover_Turn"),
	//		TEXT("Prototype_Component_State_Hover_Turn"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Hover_Turn State");

	//	//fly move
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Fly_Move"),
	//		TEXT("Hover_Move"),
	//		TEXT("Prototype_Component_State_Fly_Move"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Fly_Move State");

	//	//Break
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Broom_Break"),
	//		TEXT("Broom_Break"),
	//		TEXT("Prototype_Component_State_Broom_Break"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Broom_Break State");

	//	//지팡이 착지 스테이트
	//	if (FAILED(m_pStateContext->Add_StateMachine(
	//		LEVEL_STATIC,
	//		TEXT("Com_Player_Broom_End"),
	//		TEXT("Broom_End"),
	//		TEXT("Prototype_Component_State_Broom_End"),
	//		&m_StateMachineDesc)))
	//		throw("Failed Add Broom_End State");
	//}
	//catch (const _tchar* pErrorTag)
	//{
	//	wstring wstrErrorMSG = {};
	//	wstrErrorMSG += pErrorTag;
	//	MessageBox(nullptr, wstrErrorMSG.c_str(), TEXT("System Message"), MB_OK);
	//	__debugbreak();
	//	return E_FAIL;
	//}

#pragma endregion
	m_pStateContext->Set_StateMachine(TEXT("Idle"));

	return S_OK;
}

void CPlayer::Update_Target_Angle()
{
	//占쏙옙占쏙옙占쏙옙 타占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	_float3 vDirTarget{};

	if (nullptr == m_pTargetTransform)
	{
		vDirTarget = m_pPlayer_Camera->Get_CamLookXZ();
	}
	else
	{
		vDirTarget = m_pTargetTransform->Get_Position() - m_pTransform->Get_Position();

		vDirTarget = XMVectorSetY(vDirTarget, 0.0f);

		vDirTarget.Normalize();
	}

	//占쏙옙 占쏙옙 占쏙옙占쏙옙
	_float3 vLook{};

	vLook = m_pTransform->Get_Look();

	vLook = XMVectorSetY(vLook, 0.0f);

	vLook.Normalize();

	_float fTargetAngle = vLook.Dot(vDirTarget);

	if (1.0f < fTargetAngle)
	{
		fTargetAngle = 1.0f;
	}

	m_fTargetAngle = acosf(fTargetAngle);

	if (0.0f > vLook.Cross(vDirTarget).y)
	{
		m_fTargetAngle *= -1;
	}

}

void CPlayer::Next_Spell_Action()
{
	if (nullptr == m_pMagicBall)
		return;

	m_pMagicBall->Do_MagicBallState_To_Next();
}

void CPlayer::Shot_Basic_Spell()
{
	//Find_Target_For_Distance();
	m_pMagicSlot->Add_Magics(m_BasicDesc_Light);
	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Shot_Basic_Last_Spell()
{
	//Find_Target_For_Distance();
	m_pMagicSlot->Add_Magics(m_BasicDesc_Heavy);
	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Protego()
{
	m_pMagicSlot->Action_Magic_Basic(1, this, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_ENEMY_ATTACK | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Gravity_On()
{
	m_pRigidBody->Set_Gravity(true);
}

void CPlayer::Gravity_Off()
{
	m_pRigidBody->Set_Gravity(false);
	//m_pRigidBody->Add_Force(m_pTransform->Get_Up()/* * 20.f*/, PxForceMode::eIMPULSE);
}

HRESULT CPlayer::Bind_Notify()
{
	function<void()> funcNotify = [&] {(*this).Shot_Basic_Spell(); };

	//Ready_Spell
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Shot_Basic_Last_Spell(); };

	//Basic_Last
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Finisher(); };

	//Finisher
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Finisher_Lightning"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Next_Spell_Action(); };

	//next_action_Spell
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_01_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_02_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_03_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_RF_Cast_Casual_Fwd_04_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_01_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_02_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_03_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_01_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_02_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_03_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_frmLft_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Lht_StepBwd_04_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Spell_Action_01"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Spell_Action_02"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Spell_Action_03"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Finisher_Lightning"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Avada_Kedvra"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Crucio"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Imperio"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("DIFFINDO"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm"), TEXT("Shot_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Lumos_Start"), TEXT("Shot_Lumos"), funcNotify, CModel::OTHERBODY)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}




	funcNotify = [&] {(*this).Protego(); };

	//Protego
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Protego_Start_anm"), TEXT("Protego"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	funcNotify = [&] {(*this).Gravity_On(); };

	//Gravity_On
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_Jump_RF_anm"), TEXT("Gravity_On"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_01_anm"), TEXT("Gravity_On"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_01_anm"), TEXT("Gravity_On"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	funcNotify = [&] {(*this).Gravity_Off(); };

	//Gravity_OFF
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_BM_Jump_RF_anm"), TEXT("Gravity_Off"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_Fwd_01_anm"), TEXT("Gravity_Off"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Rct_KnckDn_Hvy_01_anm"), TEXT("Gravity_Off"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Blink_Start(); };

	//Blink_Start
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"), TEXT("Blink_Start"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Blink_End(); };

	//Blink_End
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_DdgeRll_Fwd_anm"), TEXT("Blink_End"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}

	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Blink_Start"), TEXT("Blink_End"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] {(*this).Lumos(); };

	//Ready_Lumos
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Lumos_Start"), TEXT("Ready_Lumos"), funcNotify, CModel::OTHERBODY)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}



	funcNotify = [&] { (*this).Drink_Potion(); };
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("Drink_Potion"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] { (*this).Add_Layer_Item(); };
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("Add_Layer_Item"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	funcNotify = [&] { (*this).Stupefy(); };
	if (FAILED(m_pCustomModel->Bind_Notify(TEXT("Hu_Cmbt_Atk_Cast_Fwd_Hvy_01_Spin_anm"), TEXT("Ready_Spell"), funcNotify)))
	{
		MSG_BOX("Failed Bind_Notify");

		return E_FAIL;
	}


	return S_OK;
}

void CPlayer::Update_Cloth(_float fTimeDelta)
{
	// 占쏙옙占쏙옙 y占쏙옙占쏙옙 占쌥댐옙占쏙옙
	_float3 vVelocity = m_pTransform->Get_Velocity();
	vVelocity.y *= -1.f;
	m_pCustomModel->Set_WindVelocity(XMVector3TransformCoord(m_fClothPower * vVelocity,
		XMMatrixInverse(nullptr, XMMatrixRotationQuaternion(m_pTransform->Get_Quaternion()))));

	m_pCustomModel->Tick(CCustomModel::ROBE, 2, fTimeDelta);
}

void CPlayer::Find_Target_For_Distance()
{
	BEGININSTANCE;

	if (nullptr != m_pTarget || pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
	{
		ENDINSTANCE;
		return;
	}

	unordered_map<const _tchar*, CComponent*>* pLayer = pGameInstance->Find_Components_In_Layer(m_eLevelID, TEXT("Layer_Monster"));

	if (nullptr == pLayer)
	{
		ENDINSTANCE;
		return;
	}

	_float fMinDistance = { 50.0f };


	//占신몌옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	CGameObject* pTarget = { nullptr };

	for (unordered_map<const _tchar*, CComponent*>::iterator iter = pLayer->begin(); iter != pLayer->end(); iter++)
	{
		if (true == static_cast<CGameObject*>(iter->second)->isDead())
			continue;

		//占시뤄옙占싱억옙占?
		_float3 vPlayerPos = m_pTransform->Get_Position();

		//占쏙옙占쏙옙占쏙옙 
		_float3 vMonsterPos = dynamic_cast<CGameObject*>(iter->second)->Get_Transform()->Get_Position();

		//占신몌옙占쏙옙 占쏙옙占싹곤옙
		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));

		//占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙 占쌜다몌옙
		if (fMinDistance > fDistance)
		{
			//占신몌옙占쏙옙 占쏙옙占쏙옙占싹곤옙
			fMinDistance = fDistance;
			//占쏙옙체占쏙옙 占쏙옙占쏙옙占싼댐옙.
			pTarget = dynamic_cast<CGameObject*>(iter->second);
		}
	}

	// 占쏙옙체占쏙옙 占쌍다몌옙
	if (nullptr != pTarget)
	{
		//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
		}

		//타占쏙옙占쏙옙占쏙옙 占싼댐옙.
		m_pTargetTransform = pTarget->Get_Transform();

		Safe_AddRef(m_pTargetTransform);


		//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
		}

		//타占쏙옙占쏙옙占쏙옙 占싼댐옙.
		m_pTarget = pTarget;

		Safe_AddRef(m_pTarget);
	}

	//占쏙옙체占쏙옙 占쏙옙占쌕몌옙 
	else if (nullptr == pTarget)
	{
		//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
			m_pTargetTransform = nullptr;
		}

		//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
		}
	}
	ENDINSTANCE;
}

void CPlayer::Find_Target_For_ViewSpace()
{
	//占쌍는곤옙 占쏙옙占쏙옙占?
	if (nullptr != m_pTarget)
	{
		Clear_Target();
	}

	BEGININSTANCE;

	unordered_map<const _tchar*, CComponent*>* pLayer = pGameInstance->Find_Components_In_Layer(m_eLevelID, TEXT("Layer_Monster"));

	if (nullptr == pLayer)
	{
		ENDINSTANCE;
		return;
	}

	_float fMinDistance = { 50.0f };

	//占신몌옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙
	CGameObject* pTarget = { nullptr };

	//占쏙옙占싶울옙占썲를 占썰스占쏙옙占싱쏙옙占쏙옙 占시몌옙占쏙옙.
	//x, y占쏙옙占쏙옙占쏙옙占쏙옙 +,- 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쌔쇽옙 占싫울옙 占쌍댐옙占쏙옙 확占쏙옙占싼댐옙.

	_float4x4 viewMatrix = *pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	//占쌈쏙옙 占쏙옙占쏙옙 占쏙옙占쏙옙占싱놂옙
	list<pair<CEnemy*, _float>> EnemyList;

	//占쏙옙占쏙옙 占싫울옙 占쌍는것들만 占쏙옙占쏙옙
	for (unordered_map<const _tchar*, CComponent*>::iterator iter = pLayer->begin(); iter != pLayer->end(); iter++)
	{
		CEnemy* pEnemy = static_cast<CEnemy*>((*iter).second);

		//占썰스占쏙옙占싱쏙옙占쏙옙 占시몌옙占쏙옙
		_float4x4 TargetWorldMatrix = pEnemy->Get_Transform()->Get_WorldMatrix();

		_float4x4 TargetViewMatrix = TargetWorldMatrix * viewMatrix;

		_float3 vTargetViewPosition = TargetViewMatrix.Translation();

		//-1 ~ 1 占쏙옙占싱울옙 占쌍댐옙 占싶듸옙占쏙옙 占쌩울옙 카占쌨라보댐옙 占쌌울옙 占쌍는곤옙
		if (-m_fTargetViewRange < vTargetViewPosition.x &&
			m_fTargetViewRange > vTargetViewPosition.x &&
			-m_fTargetViewRange < vTargetViewPosition.y &&
			m_fTargetViewRange > vTargetViewPosition.y &&
			0.0f < vTargetViewPosition.z)
		{
			EnemyList.push_back(pair(pEnemy, vTargetViewPosition.z));
		}
	}

	//占쏙옙占쏙옙摸占?
	if (true == EnemyList.empty())
	{
		ENDINSTANCE;
		return;
	}

	EnemyList.sort(
		[](const pair<CEnemy*, _float>& _pEnemy_1, const pair<CEnemy*, _float>& _pEnemy_2)
		{
			return _pEnemy_1.second < _pEnemy_2.second;
		});

	pTarget = EnemyList.front().first;

	if (nullptr == pTarget)
	{
		EnemyList.clear();

		ENDINSTANCE;
		return;
	}

	//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
	if (nullptr != m_pTargetTransform)
	{
		Safe_Release(m_pTargetTransform);
	}

	//타占쏙옙占쏙옙占쏙옙 占싼댐옙.
	m_pTargetTransform = pTarget->Get_Transform();

	Safe_AddRef(m_pTargetTransform);


	//占쏙옙占쏙옙 占쏙옙체占쏙옙 占쏙옙占쏙옙占쌍곤옙
	if (nullptr != m_pTarget)
	{
		Safe_Release(m_pTarget);
	}

	//타占쏙옙占쏙옙占쏙옙 占싼댐옙.
	m_pTarget = pTarget;

	Safe_AddRef(m_pTarget);

	ENDINSTANCE;
}

void CPlayer::Finisher()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(3, m_pTarget, m_pWeapon, (COLLISIONFLAG)(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Stupefy()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(4, m_pTarget, m_pWeapon, COL_ENEMY, m_isPowerUp);
}


void CPlayer::Lumos()
{
	if (nullptr == m_pFrncSpellToggle)
	{
		m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(2, this, m_pWeapon, COL_NONE, m_isPowerUp);
	}
	else
	{
		m_pFrncSpellToggle = nullptr;
	}
}



void CPlayer::Finish_Animation()
{
	m_isFinishAnimation = true;
}

void CPlayer::Go_Roll(void* _pArg)
{
	if (true == m_isBlink ||
		(true == m_pPlayer_Camera->Is_Finish_Animation() &&
			(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
				m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
				m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
				m_pStateContext->Is_Current_State(TEXT("Move Loop")) ||
				m_pStateContext->Is_Current_State(TEXT("Magic_Cast"))))
		)
	{
		m_pStateContext->Set_StateMachine(TEXT("Roll"), _pArg);
	}
}

void CPlayer::Go_Jump()
{
	if (true == m_pPlayer_Camera->Is_Finish_Animation() &&
		(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Loop")) ||
			m_pStateContext->Is_Current_State(TEXT("Magic_Cast"))))
	{
		m_pStateContext->Set_StateMachine(TEXT("Jump"));
	}
}

#ifdef _DEBUG

void CPlayer::Tick_TestShake()
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	
	ImGui::SetNextWindowPos(ImVec2(_float(rc.right) - 8.f, _float(rc.top)));
	ImGui::SetNextWindowSize(ImVec2(300.f, 600.f));

	ImGui::Begin("TestShake");

	ImGui::Text("Shake_Type");

	ImGui::RadioButton("TRANSLATION", &m_iShake_Type, CCamera_Manager::SHAKE_TYPE_TRANSLATION); ImGui::SameLine();
	ImGui::RadioButton("ROTATION", &m_iShake_Type, CCamera_Manager::SHAKE_TYPE_ROTATION);


	ImGui::Text("Shake_Axis");

	ImGui::RadioButton("RIGHT", &m_iShake_Axis, CCamera_Manager::SHAKE_AXIS_RIGHT); ImGui::SameLine();
	ImGui::RadioButton("UP", &m_iShake_Axis, CCamera_Manager::SHAKE_AXIS_UP);
	ImGui::RadioButton("LOOK", &m_iShake_Axis, CCamera_Manager::SHAKE_AXIS_LOOK); ImGui::SameLine();
	ImGui::RadioButton("SET", &m_iShake_Axis, CCamera_Manager::SHAKE_AXIS_SET);

	ImGui::Text("SHAKE_POWER");

	ImGui::RadioButton("CRECENDO", &m_iShakePower, CCamera_Manager::SHAKE_POWER_CRECENDO); ImGui::SameLine();
	ImGui::RadioButton("DECRECENDO", &m_iShakePower, CCamera_Manager::SHAKE_POWER_DECRECENDO);
	ImGui::RadioButton("CRECENDO_DECRECENDO", &m_iShakePower, CCamera_Manager::SHAKE_POWER_CRECENDO_DECRECENDO);


	ImGui::DragFloat("SHAKE_SPEED", &m_fShakeSpeed, 0.1f, 1.0f, 60.0f);
	ImGui::DragFloat("SHAKE_DURATION", &m_fShakeDuration, 0.001f, 0.001f, 100.0f);
	ImGui::DragFloat("SHAKE_POWER", &m_fShakePower, 0.001f, 0.001f, 1.0f);

	ImGui::DragFloat("Axis_Set.X", &m_fx, 0.001f, 0.001f, 1.0f);
	ImGui::DragFloat("Axis_Set.Y", &m_fy, 0.001f, 0.001f, 1.0f);
	ImGui::DragFloat("Axis_Set.Z", &m_fz, 0.001f, 0.001f, 1.0f);

	ImGui::End();

}
void CPlayer::Key_input_Flying(_float fTimeDelta)
{
	BEGININSTANCE;
	//조준
	if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_RBUTTON, CInput_Device::KEY_PRESSING))
	{
		Find_Target_For_ViewSpace();
	}

#pragma region 스테이트 변경 키 입력

	//if (pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	//{
	//	Go_Landing();
	//}

	if (true == m_isReadySpell)
	{
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc = CMagicCastingState::MAGICCASTINGSTATEDESC();

		MagicCastingStateDesc.pisReadySpell = &m_isReadySpell;

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_BASIC;

		// 평타 : 타입과 레디 변수만 넘겨준다.
		if (pGameInstance->Get_DIMouseState(CInput_Device::DIMK_LBUTTON, CInput_Device::KEY_DOWN))
		{
			Go_MagicCast(&MagicCastingStateDesc);
		}

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_NORMAL;

		for (size_t i = 0; i < SKILLINPUT_END; i++)
		{
			if (pGameInstance->Get_DIKeyState(DIK_1 + i, CInput_Device::KEY_DOWN) && m_pMagicSlot->IsCoolOn_Skill(i))
			{
				//액션
				MagicCastingStateDesc.iSpecialAction = Special_Action(i);

				//함수 포인터
				switch (i)
				{
				case Client::CPlayer::SKILLINPUT_1:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_1(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_2:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_2(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_3:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_3(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_4:
				{
					MagicCastingStateDesc.pFuncSpell = [&] {(*this).Shot_Magic_Spell_Button_4(); };
				}
				break;
				case Client::CPlayer::SKILLINPUT_END:
					break;
				default:
					break;
				}
				Go_MagicCast(&MagicCastingStateDesc);
			}
		}

		MagicCastingStateDesc.iSpellType = CMagicCastingState::SPELL_FINISHER;

		//조건 추가함
		if (pGameInstance->Get_DIKeyState(DIK_X, CInput_Device::KEY_DOWN) && m_pPlayer_Information->Is_Use_Fnisher() && nullptr != m_pTarget)
		{
			MagicCastingStateDesc.pFuncSpell = [&] {(*this).Finisher(); };

			static_cast<CEnemy*>(m_pTarget)->Ready_Hit_Finisher();

			m_pPlayer_Information->Using_Fnisher();

			Go_MagicCast(&MagicCastingStateDesc);

			m_pPlayer_Camera->Change_Animation(TEXT("Cam_Finisher_Lightning_01_anm"));
		}

		if (pGameInstance->Get_DIKeyState(DIK_Q, CInput_Device::KEY_DOWN) && false == m_pStateContext->Is_Current_State(TEXT("Protego")))
		{
			Go_Protego(nullptr);
		}
	}
#pragma endregion

	//루모스
	if (false == m_isLumosOn &&
		pGameInstance->Get_DIKeyState(DIK_Z, CInput_Device::KEY_DOWN) &&
		(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
			m_pStateContext->Is_Current_State(TEXT("Jump")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Loop"))))
	{
		m_pCustomModel->Change_Animation(TEXT("Lumos_Start"), CModel::OTHERBODY);
		m_isLumosOn = true;
	}


	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isFixMouse)
			m_isFixMouse = false;
		else
			m_isFixMouse = true;
	}

	/*if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 10.f, PxForceMode::eIMPULSE);
	}*/

	ENDINSTANCE;
}
#endif // _DEBUG

void CPlayer::Go_MagicCast(void* _pArg)
{
	Find_Target_For_Distance();

	if (m_pStateContext->Is_Current_State(TEXT("Idle")) ||
		m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
		m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
		m_pStateContext->Is_Current_State(TEXT("Move Loop")) ||
		m_pStateContext->Is_Current_State(TEXT("Magic_Cast")))
	{
		if (nullptr != m_pTarget)
		{
			switch (static_cast<CMagicCastingState::MAGICCASTINGSTATEDESC*>(_pArg)->iSpecialAction)
			{
			case CMagicCastingState::SPECIAL_ACTION_AVADA_KEDAVRA:
			{
				m_pPlayer_Camera->Change_Animation(TEXT("Avada_Kedavra_Face"));
			}
			break;
			case CMagicCastingState::SPECIAL_ACTION_IMPERIO:
			{
				m_pPlayer_Camera->Change_Animation(TEXT("Imperio_Down"));
			}
			break;
			case CMagicCastingState::SPECIAL_ACTION_CRUCIO:
			{
				m_pPlayer_Camera->Change_Animation(TEXT("Crucio"));
			}
			break;
			default:
				break;
			}
		}

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), _pArg);
	}
}

void CPlayer::Go_Switch_Start()
{
	if (m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
		m_pStateContext->Is_Current_State(TEXT("Move Start")))
	{
		m_pStateContext->Set_StateMachine(TEXT("Move Start"));
	}
}

void CPlayer::Go_Switch_Loop()
{
	if (m_pStateContext->Is_Current_State(TEXT("Move Loop")))
	{
		m_pStateContext->Set_StateMachine(TEXT("Move Loop"));
	}
}

_uint CPlayer::Special_Action(_uint _iButton)
{
	if (SKILLINPUT_1 > _iButton || SKILLINPUT_4 < _iButton)
	{
#ifdef _DEBUG
		MSG_BOX("Parameter out of range : Special_Action");
#endif // _DEBUG
	}

	_uint iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_END;

	switch (m_vecSpellCheck[_iButton])
	{
	case Client::AVADAKEDAVRA:
	{
		iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_AVADA_KEDAVRA;
	}
	break;
	case Client::CRUCIO:
	{
		iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_CRUCIO;
	}
	break;
	case Client::DIFFINDO:
	{
		iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_DIFFINDO;
	}
	break;
	case Client::IMPERIO:
	{
		iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_IMPERIO;
	}
	break;
	default:
	{
		iSpecial_Action_Spell = CMagicCastingState::SPECIAL_ACTION_NONE;
	}
	break;
	}

	return iSpecial_Action_Spell;
}

void CPlayer::Add_Layer_Item()
{
	CTool* pTool = m_pPlayer_Information->Get_PotionTap()->Get_CurTool();
	if (nullptr == pTool)
		return;
	pTool->CreateTool();
}

void CPlayer::Drink_Potion()
{
	m_pPlayer_Information->Get_PotionTap()->Use_Item(m_pTransform->Get_Position());
}

void CPlayer::Add_Potion()
{
	m_pPlayer_Information->Add_Potion();
}

void CPlayer::Drink_Heal_Potion()
{
	CTool* pTool = m_pPlayer_Information->Get_Healpotion();

	if (nullptr == pTool)
	{
		return;
	}

	pTool->Use(m_pTransform->Get_Position());
}

void CPlayer::Go_Protego(void* _pArg)
{
	if (true == m_isPrepareProtego && true == m_isCollisionEnterProtego)
	{
		Find_Target_For_Distance();

		m_pStateContext->Set_StateMachine(TEXT("Protego"), _pArg);

		m_isCollisionEnterProtego = false;
	}
}

void CPlayer::Go_Hit(void* _pArg)
{
	if (true == m_pPlayer_Camera->Is_Finish_Animation() &&
		(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Loop")) ||
			m_pStateContext->Is_Current_State(TEXT("Magic_Cast")) ||
			m_pStateContext->Is_Current_State(TEXT("Standing")) ||
			m_pStateContext->Is_Current_State(TEXT("Jump"))))
	{
		m_pStateContext->Set_StateMachine(TEXT("Hit"), _pArg);
	}
}

void CPlayer::Update_Skill_CoolTime()
{
	for (size_t i = 0; i < SKILLINPUT_END; i++)
	{
		m_vecCoolTimeRatio[i] = 1.0f - m_pMagicSlot->Get_CoolTimeRatio(i);
	}
}

void CPlayer::Blink_Start()
{
	m_isBlink = true;
}

void CPlayer::Blink_End()
{
	m_isBlink = false;
}

void CPlayer::Healing()
{
	m_pPlayer_Information->fix_HP(40);
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = New CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Prepare_Protego()
{
	if (true == m_pPlayer_Camera->Is_Finish_Animation() &&
		(m_pStateContext->Is_Current_State(TEXT("Idle")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Turn")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Start")) ||
			m_pStateContext->Is_Current_State(TEXT("Move Loop")) ||
			m_pStateContext->Is_Current_State(TEXT("Magic_Cast")) ||
			m_pStateContext->Is_Current_State(TEXT("Protego"))))
	{
		m_isPrepareProtego = true;
	}
	else
	{
		m_isPrepareProtego = false;
	}
}

void CPlayer::Go_Use_Item()
{
	CTool* pTool = m_pPlayer_Information->Get_PotionTap()->Get_CurTool();

	if (nullptr == pTool)
		return;

	CUseItemState::USEITEMDESC UseItemDesc;
	
	UseItemDesc.funcPotion = [&] {(*this).Add_Layer_Item(); };

	m_pCustomModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("Add_Layer_Item"), UseItemDesc.funcPotion);

	UseItemDesc.eItem_Id = pTool->Get_ItemID();

	switch (UseItemDesc.eItem_Id)
	{
	case Client::ITEM_ID_WIGGENWELD_POTION:
	{
		UseItemDesc.funcPotion = [&] {(*this).Healing(); };
	}
	break;
	case Client::ITEM_ID_EDURUS_POTION:
	case Client::ITEM_ID_FOCUS_POTION:
	case Client::ITEM_ID_MAXIMA_POTION:
	case Client::ITEM_ID_INVISIBILITY_POTION:
	case Client::ITEM_ID_THUNDERBEW_POTION:
	case Client::ITEM_ID_FELIX_FELICIS_POTION:
	{
		UseItemDesc.funcPotion = [&] {(*this).Drink_Potion(); };
	}
	break;
	case Client::ITEM_ID_CHINESE_CHOMPING_CABBAGE:
	{
		UseItemDesc.funcPotion = nullptr;
	}
	break;
	default:
		break;
	}

	if (true == m_pPlayer_Camera->Is_Finish_Animation() &&
		(m_pStateContext->Is_Current_State(TEXT("Idle"))))
	{
		m_pStateContext->Set_StateMachine(TEXT("UseItem"), &UseItemDesc);
	}
}

void CPlayer::Go_Use_Potion()
{
	CUseItemState::USEITEMDESC UseItemDesc;

	UseItemDesc.funcPotion = [&] {(*this).Add_Potion(); };

	m_pCustomModel->Bind_Notify(TEXT("Drink_Potion_Throw"), TEXT("Add_Layer_Item"), UseItemDesc.funcPotion);

	UseItemDesc.eItem_Id = ITEM_ID_WIGGENWELD_POTION;

	UseItemDesc.funcPotion = [&] {(*this).Drink_Heal_Potion(); };
	
	if (true == m_pPlayer_Camera->Is_Finish_Animation() &&
		(m_pStateContext->Is_Current_State(TEXT("Idle"))))
	{
		m_pStateContext->Set_StateMachine(TEXT("UseItem"), &UseItemDesc);
	}
}

void CPlayer::Shot_Magic_Spell_Button_1()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(SKILLINPUT_1, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Shot_Magic_Spell_Button_2()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(SKILLINPUT_2, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Shot_Magic_Spell_Button_3()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(SKILLINPUT_3, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}

void CPlayer::Clear_Target()
{
	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTransform);
	m_pTarget = nullptr;
	m_pTargetTransform = nullptr;
}

void CPlayer::Shot_Magic_Spell_Button_4()
{
	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(SKILLINPUT_4, m_pTarget, m_pWeapon, COLLISIONFLAG(COL_ENEMY | COL_SHIELD), m_isPowerUp);
}



CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = New CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	CGameObject::Free();

	if (true == m_isCloned)
	{
		Safe_Release(m_pShader);
		Safe_Release(m_pShadowShader);
		Safe_Release(m_pRenderer);
		Safe_Release(m_pCustomModel);
		Safe_Release(m_pPlayer_Camera);
		Safe_Release(m_pMagicSlot);
		Safe_Release(m_pWeapon);
		Safe_Release(m_pStateContext);
		Safe_Release(m_pRigidBody);
		Safe_Release(m_pPlayer_Information);
		Safe_Release(m_UI_Group_Skill_01);
		Safe_Release(m_UI_Group_SkillTap);
		Safe_Release(m_pCooltime);
		Safe_Release(m_pDefence);

		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
		}
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
		}

		Safe_Release(m_StateMachineDesc.pOwnerModel);
		Safe_Release(m_StateMachineDesc.pPlayerTransform);

		m_vecCoolTimeRatio.clear();
		m_vecSpellCheck.clear();
	}
}
