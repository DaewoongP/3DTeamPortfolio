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
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(-5.f);

	m_eCurrentAnim = WF_IDLE;
	m_ePreAnim = m_eCurrentAnim;
	m_pModelCom->Change_Animation(m_eCurrentAnim);
	//m_isSpawn = true;

	if (FAILED(Bind_HitMatrices()))
		return E_FAIL;

	if (FAILED(Make_Notifies()))
		return E_FAIL;

	return S_OK;
}

void CWolf::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// ü���� 0 ���Ϸ� ��������
	if (0.f >= m_pHealth->Get_Current_HP())
		m_eCurrentAnim = WF_DIE;

	// m_pHitMatrix = m_HitMatrices[rand() % 2];

	Set_Current_Target();
	Wolf_Animation();

	// �����ϸ鼭 �÷��̾ ������ ����
	if (WF_JUMP_R == m_eCurrentAnim)
		Wolf_Turn(fTimeDelta);

	// ���� �ִϸ��̼ǰ� �޶����ٸ� �ִϸ��̼� ����
	if (m_eCurrentAnim != m_ePreAnim)
	{
		m_ePreAnim = m_eCurrentAnim;
		m_pModelCom->Change_Animation(m_eCurrentAnim);
	}

	if (nullptr != m_pModelCom)
		m_pModelCom->Play_Animation(fTimeDelta, CModel::UPPERBODY, m_pTransform);
}

void CWolf::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CWolf::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);

	wstring wstrObjectTag = CollisionEventDesc.pOtherObjectTag;
	wstring wstrMyCollisionTag = CollisionEventDesc.pThisCollisionTag;
	wstring wstrOtherCollisionTag = CollisionEventDesc.pOtherCollisionTag;

	wstring wsPlayer(TEXT("Player_Default"));

	/* Collision Magic */
	if (wstring::npos != wstrObjectTag.find(TEXT("MagicBall")))
	{
		CMagicBall::COLLSIONREQUESTDESC* pCollisionMagicBallDesc = static_cast<CMagicBall::COLLSIONREQUESTDESC*>(CollisionEventDesc.pArg);
		_int iDamage = pCollisionMagicBallDesc->iDamage;

		m_pHealth->Damaged(iDamage);
	}

	// �÷��̾ ���� �ȿ� ���Դٸ�
	if (0 == lstrcmp(wstrOtherCollisionTag.c_str(), wsPlayer.c_str()))
	{
		if (WF_IDLE == m_eCurrentAnim) // �� �߰� �� �����ִ� ����
			m_eCurrentAnim = WF_IDLE_END;
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

	///* Collision Player Fig */
	//if (wstring::npos != wstrMyCollisionTag.find(TEXT("Range")))
	//{
	//	if (false == IsEnemy(wstrObjectTag))
	//		return;

	//	m_isSpawn = true;
	//	auto iter = m_RangeInEnemies.find(wstrObjectTag);
	//	if (iter == m_RangeInEnemies.end())
	//		m_RangeInEnemies.emplace(wstrObjectTag, CollisionEventDesc.pOtherOwner);
	//}

	///* Collision Protego */
	//if (wstring::npos != wstrMyCollisionTag.find(TEXT("Attack")) &&
	//	wstring::npos != wstrOtherCollisionTag.find(TEXT("Protego")))
	//{
	//	m_isParring = true;
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
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	/*if (FAILED(__super::Render()))
	{
		MSG_BOX("[CWolf] Failed Render");
		return E_FAIL;
	}*/

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint iMeshCount = 0; iMeshCount < iNumMeshes; ++iMeshCount)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", iMeshCount);

		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", iMeshCount, DIFFUSE);
		m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", iMeshCount, NORMALS);

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
		RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.1f, -0.3f);
		PxSphereGeometry pSphereGeomatry2 = PxSphereGeometry(0.25f);
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
		lstrcpy(Desc.wszObjectName, TEXT("����"));

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

void CWolf::Wolf_Attack()
{
}

void CWolf::Wolf_Animation()
{
	switch (m_eCurrentAnim)
	{
	case WF_IDLE_END: // �� �߰� �� �Ͼ
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_HOWL;
		}
		break;
	case WF_HOWL: // �Ͼ �� �Ͽ︵
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_BARK;
		}
		break;
	case WF_BARK: // ���� �� ¢��
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_JUMP_R;
		}
		break;
	case WF_JUMP_R: // ���������� �� �� ��¦ ��
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_START;
		}
		break;
	case WF_ATTACK_START: // ���� ���� �ڼ� ���
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK;
		}
		break;
	case WF_ATTACK: // ����
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_AFTER_180_TURN;
		}
		break;
	case WF_ATTACK_AFTER_180_TURN: // ���� �����ߴٸ� 180�� ȸ���ϸ� ���
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_RUN_START;
		}
		break;
	case WF_RUN_START: // ���� �� ��� ��
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_ATTACK_END;
		}
		break;
	case WF_ATTACK_END: // �ٽ� ���� ���� �غ�
		if (true == m_pModelCom->Is_Finish_Animation())
		{
			m_eCurrentAnim = WF_BARK;
		}
		break;
	}
}

void CWolf::Wolf_Turn(_float fTimeDelta)
{
	m_pTransform->LookAt_Lerp(m_pTarget->Get_Transform()->Get_Position(), fTimeDelta, true);
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
}
