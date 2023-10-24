#include "Weapon_Dragon_Tail.h"
#include "GameInstance.h"

CWeapon_Dragon_Tail::CWeapon_Dragon_Tail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Dragon_Tail::CWeapon_Dragon_Tail(const CWeapon_Dragon_Tail& rhs)
	: CParts(rhs)
{
}

HRESULT CWeapon_Dragon_Tail::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDarkCloud")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDarkCloud")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/WingAttack/TraceDarkCloud/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDust")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDust")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/WingAttack/TraceDust/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

void CWeapon_Dragon_Tail::On_Collider_Attack()
{
	m_pEffect_WingAttack_TraceDarkCloud->Play(m_Bones[11].vPosition);
	m_pEffect_WingAttack_TraceDust->Play(m_Bones[12].vPosition);
	m_pRigidBody->Enable_Collision("Attack", this, &m_CollisionRequestDesc);
}

void CWeapon_Dragon_Tail::Off_Collider_Attack()
{
	m_pEffect_WingAttack_TraceDarkCloud->Stop();
	m_pEffect_WingAttack_TraceDust->Stop();
	m_pRigidBody->Disable_Collision("Attack");
}

HRESULT CWeapon_Dragon_Tail::Set_Bone_Data(CModel* pModel)
{
	for (_uint i = 171; i < 185; ++i)
	{
		const CBone* pBone = pModel->Get_Bone_Index(i);
		if (nullptr == pBone)
			return E_FAIL;
		CConjuredDragon::DRAGONBONEDATA BoneData;
		BoneData.pCombinedTransformationMatrix = pBone->Get_CombinedTransformationMatrixPtr();
		m_Bones.push_back(BoneData);
	}

	return S_OK;
}

HRESULT CWeapon_Dragon_Tail::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_CollisionRequestDesc.eType = CEnemy::ATTACK_HEAVY;
	m_CollisionRequestDesc.iDamage = 20;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;
	m_pEffect_WingAttack_TraceDarkCloud->Disable();
	m_pEffect_WingAttack_TraceDust->Disable();
	return S_OK;
}

void CWeapon_Dragon_Tail::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	ADD_IMGUI([&] { Tick_Imgui(fTimeDelta); });
#endif // _DEBUG
	__super::Tick(fTimeDelta);

	for (auto& Desc : m_Bones)
	{
		_float4x4 BoneWorldMatrix = *Desc.pCombinedTransformationMatrix *
			m_ParentMatrixDesc.PivotMatrix *
			*m_ParentMatrixDesc.pParentWorldMatrix;
		Desc.vPosition = BoneWorldMatrix.Translation();
	}
	//cout_float3("Pos", m_Bones[11].vPosition);
	m_pEffect_WingAttack_TraceDarkCloud->Get_Transform()->Set_Position(m_Bones[11].vPosition);
	m_pEffect_WingAttack_TraceDust->Get_Transform()->Set_Position(m_Bones[12].vPosition);
}

#ifdef _DEBUG
void CWeapon_Dragon_Tail::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
}
void CWeapon_Dragon_Tail::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	ImGui::Begin("zscvxcvsd");
	ImGui::DragInt("sdvwef", &test, 0.1f, 0, 13);
	ImGui::End();
}
#endif // _DEBUG

HRESULT CWeapon_Dragon_Tail::Add_Components()
{
	/* For.Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 1.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxCapsuleGeometry pCapsuleGeomatry = PxCapsuleGeometry(2.f, 3.5f);
	RigidBodyDesc.pGeometry = &pCapsuleGeomatry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::None;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 1.f, 1.f);
	RigidBodyDesc.pOwnerObject = this;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Attack");
	RigidBodyDesc.eThisCollsion = COL_ENEMY_ATTACK;
	RigidBodyDesc.eCollisionFlag = COL_PLAYER | COL_SHIELD;
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("[CWeapon_Dragon_Tail] Failed Initialize : Com_RigidBody");
		return E_FAIL;
	}

#ifdef _DEBUG
	/* For.Com_Renderer */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRenderer))))
		throw TEXT("Com_Renderer");
#endif // _DEBUG

	if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDarkCloud"),
		TEXT("Com_Particle_WingAttack_TraceDarkCloud"), reinterpret_cast<CComponent**>(&m_pEffect_WingAttack_TraceDarkCloud))))
		throw TEXT("Com_Particle_WingAttack");

	if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceDust"),
		TEXT("Com_Particle_WingAttack_TraceDust"), reinterpret_cast<CComponent**>(&m_pEffect_WingAttack_TraceDust))))
		throw TEXT("Com_Particle_WingAttack_TraceDust");

	return S_OK;
}

CWeapon_Dragon_Tail* CWeapon_Dragon_Tail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Dragon_Tail* pInstance = New CWeapon_Dragon_Tail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Dragon_Tail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Dragon_Tail* CWeapon_Dragon_Tail::Clone(void* pArg)
{
	CWeapon_Dragon_Tail* pInstance = New CWeapon_Dragon_Tail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Dragon_Tail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Dragon_Tail::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pEffect_WingAttack_TraceDarkCloud);
#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG
}
