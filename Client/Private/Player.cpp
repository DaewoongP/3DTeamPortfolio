#include "..\Public\Player.h"
#include "GameInstance.h"
#include "Player_Camera.h"
#include "MagicSlot.h"
#include "Weapon_Player_Wand.h"
#include "StateContext.h"
#include "IdleState.h"

#include "Player_Information.h"

#include "ProtegoState.h"
#include "MagicCastingState.h"

#include "Armored_Troll.h"
#include "MagicBall.h"
#include "Enemy.h"
#include "UI_Group_Enemy_HP.h"

#include "Magic.h"

#include "UI_Group_Skill.h"

#include "AccPotion.h"
#include "CoolTime.h"	

#include "WiggenweldPotion.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	PLAYERDESC* pPlayerDesc = static_cast<PLAYERDESC*>(pArg);

	m_eLevelID = pPlayerDesc->eLevelID;

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

	m_pTransform->Set_Position(pPlayerDesc->vPosition);

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

	if (FAILED(Ready_Camera()))
	{
		MSG_BOX("Failed Ready Player Camera");

		return E_FAIL;
	}

	if (FAILED(Ready_MagicDesc()))
	{
		MSG_BOX("Failed Ready _MagicDesc");

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

	m_vLevelInitPosition[LEVEL_CLIFFSIDE] = _float3(25.f, 3.f, 22.5f);

	return S_OK;
}

HRESULT CPlayer::Initialize_Level(_uint iCurrentLevelIndex)
{
	m_pTransform->Set_Position(m_vLevelInitPosition[iCurrentLevelIndex]);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_G, CInput_Device::KEY_DOWN))
	{
		m_pPlayer_Information->fix_HP(-10);
		m_pPlayer_Information->Using_Fnisher();
	}
	if (pGameInstance->Get_DIKeyState(DIK_F5, CInput_Device::KEY_DOWN))
	{
		m_pAccPotion->Use();
	}


	ENDINSTANCE;

	Update_Target_Angle();

	__super::Tick(fTimeDelta);

	Key_Input(fTimeDelta);

	Fix_Mouse();

	UpdateLookAngle();


	//m_pStateContext->Tick(fTimeDelta);

	Update_Cloth(fTimeDelta);

	Shot_Magic_Spell();
	
	m_pCustomModel->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
	m_pCustomModel->Play_Animation(fTimeDelta, CModel::UNDERBODY);


	//루모스 업데이트
	if (nullptr != m_pFrncSpellToggle)
	{
		m_pFrncSpellToggle(nullptr);
	}
	m_pCooltime->Tick(fTimeDelta);


	
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
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

#ifdef _DEBUG
	Tick_ImGui();
#endif // _DEBUG

	if (nullptr != m_pTarget)
	{
		if (true == m_pTarget->isDead())
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
			return;
		}

		CEnemy* pEnemy = static_cast<CEnemy*>(m_pTarget);
		if (nullptr != pEnemy)
			pEnemy->Get_UI_Enemy_HP()->Late_Tick(fTimeDelta);
	}
}

void CPlayer::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	if (wstring::npos != wstrCollisionTag.find(TEXT("Attack")) ||
		wstring::npos != wstrCollisionTag.find(TEXT("Enemy_Body")))
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
			CProtegoState::PROTEGOSTATEDESC ProtegoStateDesc;

			ProtegoStateDesc.isHit = true;

			switch (pDesc->eType)
			{
			case CEnemy::ATTACK_NONE:
			{	}
			break;
			case CEnemy::ATTACK_LIGHT:
			{
				ProtegoStateDesc.iHitType = CProtegoState::HIT_LIGHT;
			}
			break;
			case CEnemy::ATTACK_HEAVY:
			{
				ProtegoStateDesc.iHitType = CProtegoState::HIT_HEABY;
			}
			break;
			case CEnemy::ATTACKTYPE_END:
			{	}
			break;

			default:
				break;
			}

			ProtegoStateDesc.pTransform = pDesc->pEnemyTransform;

			m_pStateContext->Set_StateMachine(TEXT("Protego"), &ProtegoStateDesc);
		}
		//회피시 무시
		else if (m_pStateContext->Is_Current_State(TEXT("Roll")))
		{

		}
		//피격중인 상태일 경우 무시
		else if (m_pStateContext->Is_Current_State(TEXT("Hit")))
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
				HitStateDesc.iHitType = CHitState::HIT_LIGHT;
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

			m_pStateContext->Set_StateMachine(TEXT("Hit"), &HitStateDesc);

			//체력 수정
			m_pPlayer_Information->fix_HP((pDesc->iDamage)*-1);
		}

	}
}

void CPlayer::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	// 터레인 판단 구문입니다.
	//if (wcswcs(CollisionEventDesc.pOtherCollisionTag, L"Terrain"))
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

	return S_OK;
}

HRESULT CPlayer::Render_Depth()
{
	if (FAILED(SetUp_ShadowShaderResources()))
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

void CPlayer::On_Maigc_Throw_Data(void* data) const
{
	if (static_cast<CMagicBall::COLLSIONREQUESTDESC*>(data)->eMagicTag == LUMOS)
	{
		m_pFrncSpellToggle = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(data)->Action;
	}
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
	{
		__debugbreak();
		return E_FAIL;
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

	CStateContext::STATECONTEXTDESC StateContextDesc{};

	StateContextDesc.pOwnerModel = m_pCustomModel;
	StateContextDesc.pOwnerLookAngle = &m_fLookAngle;
	StateContextDesc.pIsDirectionPressed = &m_isDirectionKeyPressed;
	StateContextDesc.pPlayerTransform = m_pTransform;
	StateContextDesc.pTargetAngle = &m_fTargetAngle;

	/* For.Com_StateContext */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateContext"),
		TEXT("Com_StateContext"), reinterpret_cast<CComponent**>(&m_pStateContext), &StateContextDesc)))
	{
		__debugbreak();
		return E_FAIL;
	}

	const CBone* pBone = m_pCustomModel->Get_Bone(TEXT("SKT_RightHand"));
	if (nullptr == pBone)
		throw TEXT("pBone is nullptr");

	CWeapon_Player_Wand::PARENTMATRIXDESC ParentMatrixDesc;
	ParentMatrixDesc.OffsetMatrix = pBone->Get_OffsetMatrix();
	ParentMatrixDesc.PivotMatrix = m_pCustomModel->Get_PivotFloat4x4();
	ParentMatrixDesc.pCombindTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
	ParentMatrixDesc.pParentWorldMatrix = m_pTransform->Get_WorldMatrixPtr();

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
	RigidBodyDesc.vInitPosition = _float3(2.f, 2.f, 2.f);
	RigidBodyDesc.vInitRotation = m_pTransform->Get_Quaternion();
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxCapsuleGeometry MyGeometry = PxCapsuleGeometry(0.5f, 0.5f);
	RigidBodyDesc.pGeometry = &MyGeometry;
	RigidBodyDesc.vOffsetPosition = _float3(0.f, MyGeometry.halfHeight + MyGeometry.radius, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotZ;
	RigidBodyDesc.vDebugColor = _float4(1.f, 105 / 255.f, 180 / 255.f, 1.f); // hot pink
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_PLAYER;
	RigidBodyDesc.eCollisionFlag = COL_ENEMY_RANGE | COL_WEAPON | COL_ENEMY | COL_TRIGGER;
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
		TEXT("Com_UI_Group_Skill_1"), reinterpret_cast<CComponent**>(&m_UI_Group_Skill_01))))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_AccPotion"),
		TEXT("Com_AccPotion"), reinterpret_cast<CComponent**>(&m_pAccPotion))))
	{
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CoolTime"),
		TEXT("Com_CoolTime"), reinterpret_cast<CComponent**>(&m_pCooltime))))
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

HRESULT CPlayer::SetUp_ShadowShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShadowShader->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_LightTransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShadowShader->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	ENDINSTANCE;

	return S_OK;
}

HRESULT CPlayer::Add_Magic()
{
	CMagic::MAGICDESC magicInitDesc;
	// 레비오소
	{
		magicInitDesc.eBuffType = BUFF_LEVIOSO;
		magicInitDesc.eMagicGroup = CMagic::MG_CONTROL;
		magicInitDesc.eMagicType = CMagic::MT_YELLOW;
		magicInitDesc.eMagicTag = LEVIOSO;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 콘프링고
	{
		magicInitDesc.eBuffType = BUFF_FIRE;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = CONFRINGO;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 50;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 피니셔
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_ALL;
		magicInitDesc.eMagicTag = FINISHER;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 500;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 3.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}
	
	// 인센디오
	{
		magicInitDesc.eBuffType = BUFF_FIRE;
		magicInitDesc.eMagicGroup = CMagic::MG_DAMAGE;
		magicInitDesc.eMagicType = CMagic::MT_RED;
		magicInitDesc.eMagicTag = NCENDIO;
		magicInitDesc.fInitCoolTime = 1.5f;
		magicInitDesc.iDamage = 300;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 루모스
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = LUMOS;
		magicInitDesc.fInitCoolTime = 1.5f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 30.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 아리스토모멘텀
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = ARRESTOMOMENTUM;
		magicInitDesc.fInitCoolTime = 1.5f;
		magicInitDesc.iDamage = 0;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 30.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 아씨오
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = ACCIO;
		magicInitDesc.fInitCoolTime = 1.5f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 디센도
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = DESCENDO;
		magicInitDesc.fInitCoolTime = 1.5f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 플리펜도
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = FLIPENDO;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.3f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 엑스펠리아르무스
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = EXPELLIARMUS;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 1.2f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	// 임페리오
	{
		magicInitDesc.eBuffType = BUFF_NONE;
		magicInitDesc.eMagicGroup = CMagic::MG_ESSENTIAL;
		magicInitDesc.eMagicType = CMagic::MT_NOTHING;
		magicInitDesc.eMagicTag = IMPERIO;
		magicInitDesc.fInitCoolTime = 1.f;
		magicInitDesc.iDamage = 10;
		magicInitDesc.isChase = true;
		magicInitDesc.fLifeTime = 0.8f;
		m_pMagicSlot->Add_Magics(magicInitDesc);
	}

	/*m_pMagicSlot->Add_Magic_To_Skill_Slot(0, CONFRINGO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, LEVIOSO);*/
	m_pMagicSlot->Add_Magic_To_Skill_Slot(0, DESCENDO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(1, FLIPENDO);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(2, EXPELLIARMUS);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(2, FINISHER);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, NCENDIO);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, ARRESTOMOMENTUM);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, ACCIO);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, DESCENDO);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, FLIPENDO);
	//m_pMagicSlot->Add_Magic_To_Skill_Slot(3, EXPELLIARMUS);
	m_pMagicSlot->Add_Magic_To_Skill_Slot(3, IMPERIO);
	m_pMagicSlot->Add_Magic_To_Basic_Slot(2, LUMOS);


	
	return S_OK;
}

void CPlayer::Key_Input(_float fTimeDelta)
{
	BEGININSTANCE;

	if (pGameInstance->Get_DIKeyState(DIK_GRAVE, CInput_Device::KEY_DOWN))
	{
		if (true == m_isFixMouse)
			m_isFixMouse = false;
		else
			m_isFixMouse = true;
	}

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
#endif //_DEBUG

	/*if (pGameInstance->Get_DIKeyState(DIK_SPACE, CInput_Device::KEY_DOWN))
	{
		m_pRigidBody->Add_Force(m_pTransform->Get_Up() * 10.f, PxForceMode::eIMPULSE);
	}*/
	
	if (pGameInstance->Get_DIKeyState(DIK_L, CInput_Device::KEY_DOWN))
	{
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_Dust01"), m_pTransform->Get_Position());
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_Dust02"), m_pTransform->Get_Position());
		//CGameInstance::GetInstance()->Play_Particle(TEXT("Particle_RockChunksRough"), m_pTransform->Get_Position());
		_int iHp = 1;
		m_pPlayer_Information->Get_Health()->Set_HP(iHp);
	}

	if (pGameInstance->Get_DIKeyState(DIK_K, CInput_Device::KEY_DOWN))
	{
		CWiggenweldPotion::INIT_DESC initDesc;
		initDesc.pHealthCom = m_pPlayer_Information->Get_Health();
		CWiggenweldPotion* pWiggenweldPotion = static_cast<CWiggenweldPotion*>(
			pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_WiggenweldPotion"), &initDesc));
		pWiggenweldPotion->Use(_float3());
		Safe_Release(pWiggenweldPotion);
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

	m_pPlayer_Camera = CPlayer_Camera::Create(m_pDevice,m_pContext, &PlayerCameraDesc);

	Safe_AddRef(m_pPlayer_Camera);

	NULL_CHECK_RETURN_MSG(m_pPlayer_Camera, E_FAIL, TEXT("Failed Create Player Camera"));

	BEGININSTANCE;

	pGameInstance->Add_Camera(TEXT("Player_Camera"), (CCamera*)m_pPlayer_Camera);

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

#ifdef _DEBUG

void CPlayer::Tick_ImGui()
{
	ImGui::Begin("Player");
	
	if (ImGui::Checkbox("Gravity", &m_isGravity))
	{
		m_pRigidBody->Set_Gravity(m_isGravity);
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (ImGui::Button("Go to Level Init Position"))
	{
		m_pTransform->Set_Position(m_vLevelInitPosition[pGameInstance->Get_CurrentLevelIndex()]);
	}
	Safe_Release(pGameInstance);
	

	_float3 vVelocity = m_pTransform->Get_Velocity();
	ImGui::InputFloat3("Velocity", reinterpret_cast<_float*>(&vVelocity));

	ImGui::End();
}

#endif // _DEBUG

void CPlayer::UpdateLookAngle()
{
	BEGININSTANCE;
	
	m_isDirectionKeyPressed = false;

	_float3 vNextLook{};

	if (pGameInstance->Get_DIKeyState(DIK_W, CInput_Device::KEY_PRESSING) || 
		pGameInstance->Get_DIKeyState(DIK_W,CInput_Device::KEY_DOWN))
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

void CPlayer::Update_Target_Angle()
{
	//나에서 타겟을 향한 벡터
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
	
	//내 룩 벡터
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
	Find_Target_For_Distance();
	m_pMagicSlot->Add_Magics(m_BasicDesc_Light);
	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CPlayer::Shot_Basic_Last_Spell()
{
	Find_Target_For_Distance();
	m_pMagicSlot->Add_Magics(m_BasicDesc_Heavy);
	m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CPlayer::Protego()
{
	m_pMagicSlot->Action_Magic_Basic(1, this, m_pWeapon, COL_MAGIC);
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

	return S_OK;
}

void CPlayer::Update_Cloth(_float fTimeDelta)
{
	// 현재 y값이 반대임
	_float3 vVelocity = m_pTransform->Get_Velocity();
	vVelocity.y *= -1.f;
	m_pCustomModel->Set_WindVelocity(XMVector3TransformCoord(m_fClothPower * vVelocity,
		XMMatrixInverse(nullptr, XMMatrixRotationQuaternion(m_pTransform->Get_Quaternion()))));

	m_pCustomModel->Tick(CCustomModel::ROBE, 2, fTimeDelta);
}

void CPlayer::Find_Target_For_Distance()
{
	BEGININSTANCE;

	unordered_map<const _tchar*, CComponent*>* pLayer = pGameInstance->Find_Components_In_Layer(m_eLevelID, TEXT("Layer_Monster"));

	if (nullptr == pLayer)
	{
		ENDINSTANCE;
		return;
	}

	_float fMinDistance = { 10.0f };


	//거리가 낮은 놈을 저장
	CGameObject* pTarget = { nullptr };
	
	for (unordered_map<const _tchar*, CComponent*>::iterator iter = pLayer->begin(); iter != pLayer->end(); iter++)
	{
		if (true == static_cast<CGameObject*>(iter->second)->isDead())
			continue;

		//플레이어와
		_float3 vPlayerPos = m_pTransform->Get_Position();
		
		//몬스터의 
		_float3 vMonsterPos = dynamic_cast<CGameObject*>(iter->second)->Get_Transform()->Get_Position();

		//거리를 구하고
		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));

		//기존 값보다 작다면
		if (fMinDistance > fDistance)
		{
			//거리를 갱신하고
			fMinDistance = fDistance;
			//객체도 갱신한다.
			pTarget = dynamic_cast<CGameObject*>(iter->second);
		}
	}

	// 객체가 있다면
	if (nullptr != pTarget)
	{
		//기존 객체는 지워주고
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
		}

		//타겟으로 한다.
		m_pTargetTransform = pTarget->Get_Transform();

		Safe_AddRef(m_pTargetTransform);


		//기존 객체는 지워주고
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
		}

		//타겟으로 한다.
		m_pTarget = pTarget;

		Safe_AddRef(m_pTarget);
	}

	//객체가 없다면 
	else if (nullptr == pTarget)
	{
		//기존 객체는 지워주고
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
			m_pTargetTransform = nullptr;
		}

		//기존 객체는 지워주고
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
		}
	}
	ENDINSTANCE;
}

void CPlayer::Shot_Magic_Spell()
{
	BEGININSTANCE;
	//입력 되면 안되는 스테이트
	if (pGameInstance->Get_DIKeyState(DIK_1, CInput_Device::KEY_DOWN) &&
		false == m_pStateContext->Is_Current_State(TEXT("Standing")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hit")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Protego")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hard Land")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Jump")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Roll"))
		)
	{
		Find_Target_For_Distance();
		//마법 시전 스테이트
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc;

		MagicCastingStateDesc.pFuncSpell = [&] { (*this).Shot_Confringo(); };

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), &MagicCastingStateDesc);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_2, CInput_Device::KEY_DOWN) &&
		false == m_pStateContext->Is_Current_State(TEXT("Standing")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hit")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Protego")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hard Land")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Jump")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Roll")))
	{
		Find_Target_For_Distance();
		//마법 시전 스테이트
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc;

		MagicCastingStateDesc.pFuncSpell = [&] { (*this).Shot_Levioso(); };

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), &MagicCastingStateDesc);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_3, CInput_Device::KEY_DOWN) &&
		false == m_pStateContext->Is_Current_State(TEXT("Standing")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hit")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Protego")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hard Land")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Jump")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Roll")))
	{
		Find_Target_For_Distance();
		//마법 시전 스테이트
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc;

		MagicCastingStateDesc.pFuncSpell = [&] { (*this).Shot_NCENDIO(); };

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), &MagicCastingStateDesc);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_4, CInput_Device::KEY_DOWN) &&
		false == m_pStateContext->Is_Current_State(TEXT("Standing")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hit")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Protego")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hard Land")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Jump")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Roll")))
	{
		Find_Target_For_Distance();
		//마법 시전 스테이트
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc;

		MagicCastingStateDesc.pFuncSpell = [&] { (*this).Shot_Finisher(); };

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), &MagicCastingStateDesc);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_F, CInput_Device::KEY_DOWN) &&
		false == m_pStateContext->Is_Current_State(TEXT("Standing")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hit")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Protego")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Hard Land")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Jump")) &&
		false == m_pStateContext->Is_Current_State(TEXT("Roll")))
	{
		Find_Target_For_Distance();
		//마법 시전 스테이트
		CMagicCastingState::MAGICCASTINGSTATEDESC MagicCastingStateDesc;

		MagicCastingStateDesc.pFuncSpell = [&] { (*this).Lumos(); };

		m_pStateContext->Set_StateMachine(TEXT("Magic_Cast"), &MagicCastingStateDesc);
	}


	ENDINSTANCE;
}

void CPlayer::Shot_Levioso()
{
	//Find_Target_For_Distance();

	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(1, m_pTarget, m_pWeapon, COL_ENEMY);
}
 

void CPlayer::Shot_Confringo()
{
	//Find_Target_For_Distance();

	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(0, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CPlayer::Shot_NCENDIO()
{
	//Find_Target_For_Distance();

	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(3, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CPlayer::Shot_Finisher()
{
	//Find_Target_For_Distance();

	_float4x4 OffSetMatrix = XMMatrixIdentity();

	if (nullptr != m_pTarget)
	{
		OffSetMatrix = m_pTarget->Get_Offset_Matrix();
	}

	m_pMagicBall = m_pMagicSlot->Action_Magic_Skill(2, m_pTarget, m_pWeapon, COL_ENEMY);
}

void CPlayer::Lumos()
{
	if (nullptr == m_pFrncSpellToggle)
	{
		m_pMagicBall = m_pMagicSlot->Action_Magic_Basic(2, this, m_pWeapon, COL_NONE);
	}
	else
	{
		m_pFrncSpellToggle = nullptr;
	}
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
		Safe_Release(m_pAccPotion);
		Safe_Release(m_pCooltime);
		
		if (nullptr != m_pTargetTransform)
		{
			Safe_Release(m_pTargetTransform);
		}
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
		}
	}
}
