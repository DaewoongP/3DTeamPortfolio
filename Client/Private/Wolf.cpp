#include "..\Public\Wolf.h"
#include "GameInstance.h"

#include "MagicBall.h"
#include "UI_Group_Enemy_HP.h"

CWolf::CWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CWolf::CWolf(const CWolf& rhs)
	: CEnemy(rhs)
{
}

HRESULT CWolf::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolf::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolf::Initialize_Level(_uint iCurrentLevelIndex)
{
	if (FAILED(Add_Components_Level(iCurrentLevelIndex)))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(-1.f);
	m_pTransform->Set_RotationSpeed(-5.f);

	m_eCurrentAnim = WF_IDLE;
	m_ePreAnim = m_eCurrentAnim;
	m_pModelCom->Change_Animation(m_eCurrentAnim);
	//m_isSpawn = true;

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	m_pTransform->Set_Scale(_float3(0.5f, 0.5f, 0.5f));

	return S_OK;
}

void CWolf::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	BEGININSTANCE;
	if (true == m_isSound0 && WF_HOWL == m_eCurrentAnim)
	{
		pGameInstance->Play_Sound(TEXT("WolfHowl.wav"), 1.f);
		m_isSound0 = false;
	}
	if (true == m_isSound1 && WF_BARK == m_eCurrentAnim)
	{
		pGameInstance->Play_Sound(TEXT("Wolfmm.wav"), 1.f);
		m_isSound1 = false;
	}
	if (true == m_isSound2 && WF_ATTACK_START == m_eCurrentAnim)
	{
		pGameInstance->Play_Sound(TEXT("WolfAttack.wav"), 1.f);
		m_isSound2 = false;
	}
	ENDINSTANCE;

	if (true == m_isDissolveStart)
		m_fDissolveAmount += fTimeDelta; // 디졸브 값 증가

	Set_Current_Target();
	Wolf_Animation();
	Wolf_Levioso(fTimeDelta);
	Wolf_Decendo(fTimeDelta);

	// 점프하면서 플레이어에 방향을 맞춤
	if (WF_JUMP_R == m_eCurrentAnim || WF_RUN_START == m_eCurrentAnim)
		Wolf_Turn(fTimeDelta);

	// 공격 사거리 증가
	if (WF_ATTACK == m_eCurrentAnim)
		m_pTransform->Go_Straight(fTimeDelta * 1.5f);

	Tick_Spells();

	// 이전 애니메이션과 달라졌다면 애니메이션 변경
	if (m_eCurrentAnim != m_ePreAnim)
	{
		m_ePreAnim = m_eCurrentAnim;
		m_pModelCom->Change_Animation(m_eCurrentAnim);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta,&m_SoundChannel, CModel::UPPERBODY, m_pTransform);
}

void CWolf::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	// 체력이 0 이하로 떨어지면
	if (0.f >= m_pHealth->Get_Current_HP())
	{
		m_isDissolveStart = true;
		m_isDead = true;

		m_eCurrentAnim = WF_DIE;
		m_fDeathTimeAcc += fTimeDelta;

		if (3.f <= m_fDeathTimeAcc)
			Set_ObjEvent(OBJ_DEAD);
	}
}

void CWolf::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
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

		Print_Damage_Font(iDamage);

		m_pHealth->Damaged(iDamage);

		auto iter = m_CurrentTickSpells.find(eBuff);
		if (iter == m_CurrentTickSpells.end() &&
			BUFF_LEVIOSO & eBuff)
		{
			// 레비오소 피격
			m_iCurrentSpell ^= BUFF_LEVIOSO;
			m_CurrentTickSpells.emplace(eBuff, Action);
			m_eCurrentAnim = WF_LEVIOSO_START;
		}

		if (iter == m_CurrentTickSpells.end() &&
			BUFF_DESCENDO & eBuff)
		{
			// 디센도 피격
			m_CurrentTickSpells.emplace(eBuff, Action);
			m_eCurrentAnim = WF_DIE; // 원래는 디센도 애니메이션인데 죽는 애니메이션도 겸함
			m_isDecendo = true;
			//디센도 카메라 쉐이크
		}

		// 레비오소 피격중일 때 평타 반응
		if (WF_LEVIOSO_LOOP == m_eCurrentAnim)
		{
			Off_Gravity();

			int iRandValue = rand() % 3;

			if (0 == iRandValue)
				m_eCurrentAnim = WF_2SPIN;
			else if (1 == iRandValue)
				m_eCurrentAnim = WF2SPIN_2;
			else if (2 == iRandValue)
				m_eCurrentAnim = WF_1SPIN;
		}
	}

	/* Collision Player Fig */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (false == IsEnemy(wstrObjectTag))
			return;

		if (WF_IDLE == m_eCurrentAnim) // 적 발견 전 누워있는 상태
			m_eCurrentAnim = WF_IDLE_END;

		m_isSpawn = true;
		auto iter = m_RangeInEnemies.find(wstrObjectTag);
		if (iter == m_RangeInEnemies.end())
			m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	}

	/* Collision Protego */
	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Attack")) &&
		//wstring::npos != wstrOtherCollisionTag.find(TEXT("Protego")))
		wstring::npos != wstrOtherCollisionTag.find(TEXT("Magic_Ball")))
	{
		m_eCurrentAnim = WF_PROTEGO;
	}

	//	m_isSpawn = true;
	//	auto iter = m_RangeInEnemies.find(wstrObjectTag);
	//	if (iter == m_RangeInEnemies.end())
	//		m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	//}
}

void CWolf::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);

	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;

	if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	{
		if (false == IsEnemy(wstrObjectTag))
			return;

		Remove_GameObject(wstrObjectTag);
	}
}

HRESULT CWolf::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iMeshCount);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", iMeshCount, NORMALS);

		if (true == m_isDissolveStart)
		{
			if (FAILED(m_pShaderCom->Begin("AnimMesh_Dissolve")))
				return E_FAIL;
		}

		else
			m_pShaderCom->Begin("AnimMesh");

		if (FAILED(m_pModelCom->Render(iMeshCount)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolf::Make_Notifies()
{
	function<void()> Func = [&] { this->Wolf_Attack(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Wolf_Attack"), Func)))
		return E_FAIL;

	Func = [&] { this->Wolf_ChangeAnim(); };
	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Wolf_ChangeAnim1"), Func)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Wolf_ChangeAnim2"), Func)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_Notifies(TEXT("Wolf_ChangeAnim3"), Func)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWolf::Add_Components()
{
	try /* Check Add_Components */
	{
		if (FAILED(__super::Add_Components()))
			throw TEXT("Failed Enemy Add_Components");

		/* For.Com_RigidBody */
		CRigidBody::RIGIDBODYDESC RigidBodyDesc;
		RigidBodyDesc.isStatic = false;
		RigidBodyDesc.isTrigger = false;
		RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.5f, 0.f);
		RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f);
		RigidBodyDesc.fStaticFriction = 0.f;
		RigidBodyDesc.fDynamicFriction = 1.f;
		RigidBodyDesc.fRestitution = 0.f;
		PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(0.45f, 0.8f);
		RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
		RigidBodyDesc.eConstraintFlag = CRigidBody::RotX | CRigidBody::RotZ;
		RigidBodyDesc.vDebugColor = _float4(1.f, 1.f, 0.f, 1.f);
		RigidBodyDesc.pOwnerObject = this;
		RigidBodyDesc.eThisCollsion = COL_ENEMY;
		RigidBodyDesc.eCollisionFlag = COL_NPC_RANGE | COL_MAGIC | COL_STATIC;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Body");

		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
			TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
			throw TEXT("Com_RigidBody");

		m_OffsetMatrix = XMMatrixTranslation(RigidBodyDesc.vOffsetPosition.x, RigidBodyDesc.vOffsetPosition.y, RigidBodyDesc.vOffsetPosition.z);

		/* For.Collider Enemy_Range */
		RigidBodyDesc.isStatic = true;
		RigidBodyDesc.isTrigger = true;
		PxSphereGeometry pSphereGeomatry1 = PxSphereGeometry(12.5f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry1;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Range");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_RANGE;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC;

		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		/* For.Collider Enemy_Attack */
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.3f, -0.5f);
		PxSphereGeometry pSphereGeomatry2 = PxSphereGeometry(0.5f);
		RigidBodyDesc.pGeometry = &pSphereGeomatry2;
		strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Enemy_Attack");
		RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
		RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_NPC | COL_SHIELD;

		if (FAILED(m_pRigidBody->Create_Collider(&RigidBodyDesc)))
			throw TEXT("Failed Create_Collider");

		/* For.Com_Health */
		CHealth::HEALTHDESC HealthDesc;
		HealthDesc.iMaxHP = 200;
		if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Health"),
			TEXT("Com_Health"), reinterpret_cast<CComponent**>(&m_pHealth), &HealthDesc)))
			throw TEXT("Com_Health");

		// UI
		CUI_Group_Enemy_HP::ENEMYHPDESC  Desc;

		Desc.eType = CUI_Group_Enemy_HP::ENEMYTYPE::MONSTER;
		Desc.pHealth = m_pHealth;
		lstrcpy(Desc.wszObjectLevel, TEXT("1"));
		lstrcpy(Desc.wszObjectName, TEXT("늑대"));

		BEGININSTANCE;
		m_pUI_HP = static_cast<CUI_Group_Enemy_HP*>(pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Group_Enemy_HP"), &Desc));
		ENDINSTANCE;
		if (nullptr == m_pUI_HP)
			throw TEXT("m_pUI_HP is nullptr");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWolf] Failed Add_Components : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	m_pDissolveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/UI/Game/VFX/Textures/Noises/VFX_T_NoiseGreypack02_D.png"));
	if (nullptr == m_pDissolveTexture)
	{
		MSG_BOX("Failed CGatherer Add_Texture : (m_pDissolveTexture)");
		__debugbreak();
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolf::Add_Components_Level(_uint iCurrentLevelIndex)
{
	try
	{
		/* For.Com_Model */
		if (FAILED(CComposite::Add_Component(iCurrentLevelIndex, TEXT("Prototype_Component_Model_Wolf"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			throw TEXT("Com_Model");
	}
	catch (const _tchar* pErrorTag)
	{
		wstring wstrErrorMSG = TEXT("[CWolf] Failed Add_Components_Level : \n");
		wstrErrorMSG += pErrorTag;
		MSG_BOX(wstrErrorMSG.c_str());
		__debugbreak();

		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWolf::Bind_HitMatrices()
{
	return S_OK;
}

HRESULT CWolf::SetUp_ShaderResources()
{
	BEGININSTANCE;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransform->Get_WorldMatrixPtr())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", pGameInstance->Get_CamFar(), sizeof(_float))))
		return E_FAIL;

	if (nullptr != m_pDissolveTexture)
	{
		if (FAILED(m_pDissolveTexture->Bind_ShaderResources(m_pShaderCom, "g_DissolveTexture")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	ENDINSTANCE;

	return S_OK;
}

void CWolf::Wolf_Attack()
{
}

void CWolf::Wolf_ChangeAnim()
{
	m_eCurrentAnim = WF_LEVIOSO_LOOP;
}

void CWolf::Wolf_Animation()
{
	switch (m_eCurrentAnim)
	{
	case WF_IDLE_END: // 적 발견 후 일어남
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_HOWL;
			m_isSound0 = true;
		}
		break;
	case WF_HOWL: // 일어난 후 하울링
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_BARK;
			m_isSound1 = true;
		}
		break;
	case WF_BARK: // 공격 전 짖기
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_JUMP_R;
		}
		break;
	case WF_JUMP_R: // 오른쪽으로 한 번 폴짝 뜀
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_START;
		}
		break;
	case WF_ATTACK_START: // 공격 직전 자세 잡기
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_isSound2 = true;

			m_CollisionRequestDesc.eType = ATTACK_LIGHT;
			m_CollisionRequestDesc.iDamage = 5;
			m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

			m_pRigidBody->Enable_Collision("Enemy_Attack", this, &m_CollisionRequestDesc);

			m_eCurrentAnim = WF_ATTACK;
		}
		break;
	case WF_ATTACK: // 공격
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_AFTER_180_TURN;
		}
		break;
	case WF_ATTACK_AFTER_180_TURN: // 공격 실패했다면 180도 회전하며 들기
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_pRigidBody->Disable_Collision("Enemy_Attack");

			m_eCurrentAnim = WF_RUN_START;
		}
		break;
	case WF_RUN_START: // 공격 후 잠깐 텀
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_END;
		}
		break;
	case WF_ATTACK_END: // 다시 공격 시작 준비
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_BARK;
		}
		break;
	case WF_LEVIOSO_START: // 레비오소 시작
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_LEVIOSO_LOOP;
		}
		break;
	case WF_LEVIOSO_END: // 레비오소 끝
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			On_Gravity();
			Remove_Fly_Spells();
			m_eCurrentAnim = WF_BARK;
		}
		break;
	case WF_2SPIN: // 레비오소 중 피격 모션 1
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_LEVIOSO_LOOP;
		}
		break;
	case WF2SPIN_2: // 레비오소 중 피격 모션 2
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_LEVIOSO_LOOP;
		}
		break;
	case WF_1SPIN: // 레비오소 중 피격 모션 3
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_LEVIOSO_LOOP;
		}
		break;
	case WF_GETUP: // 디센도 혹은 프로테고 피격 후 일어나기
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_JUMP_R;
		}
		break;
	case WF_PROTEGO: // 프로테고에 튕겨나감
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_GETUP;
		}
		break;
	}
}

void CWolf::Wolf_Turn(_float fTimeDelta)
{
	// 타겟이 있다면 
	if(nullptr != m_pTarget)
		m_pTransform->LookAt_Lerp(m_pTarget->Get_Transform()->Get_Position(), fTimeDelta, true);
}

void CWolf::Wolf_Levioso(_float fTimeDelta)
{
	// 레비오소 지속시간이 끝나면 원래 애니메이션으로 돌림
	if(WF_LEVIOSO_LOOP == m_eCurrentAnim ||
		WF_2SPIN == m_eCurrentAnim ||
		WF2SPIN_2 == m_eCurrentAnim ||
		WF_1SPIN == m_eCurrentAnim)
		m_fLeviosoDur += fTimeDelta;

	if (6.5f <= m_fLeviosoDur)
	{
		m_fLeviosoDur = 0.f;
		m_eCurrentAnim = WF_LEVIOSO_END;
	}
}

void CWolf::Wolf_Decendo(_float fTimeDelta)
{
	if (false == m_isDecendo)
		return;

	m_fDecendoDur += fTimeDelta;

	if (5.f <= m_fDecendoDur)
	{
		m_isDecendo = false;
		m_fDecendoDur = 0.f;
		m_eCurrentAnim = WF_GETUP;
	}
}

void CWolf::Wolf_Protego()
{
	m_pRigidBody->Disable_Collision("Enemy_Attack");
}

CWolf* CWolf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWolf* pInstance = New CWolf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWolf::Clone(void* pArg)
{
	CWolf* pInstance = New CWolf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWolf::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTexture);
}
