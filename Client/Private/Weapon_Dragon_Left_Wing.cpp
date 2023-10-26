#include "Weapon_Dragon_Left_Wing.h"
#include "GameInstance.h"

CWeapon_Dragon_Left_Wing::CWeapon_Dragon_Left_Wing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CParts(pDevice, pContext)
{
}

CWeapon_Dragon_Left_Wing::CWeapon_Dragon_Left_Wing(const CWeapon_Dragon_Left_Wing& rhs)
	: CParts(rhs)
{
}

void CWeapon_Dragon_Left_Wing::On_Collider_Attack()
{
	m_pEffect_WingAttack_TraceDarkCloud->Play(m_Bones[0].vPosition);
	m_pRigidBody->Enable_Collision("Attack", this, &m_CollisionRequestDesc);
}

void CWeapon_Dragon_Left_Wing::Off_Collider_Attack()
{
	m_pEffect_WingAttack_TraceDarkCloud->Stop();
	m_pEffect_WingAttack_TraceRocks->Stop();
	m_pRigidBody->Disable_Collision("Attack");
}

HRESULT CWeapon_Dragon_Left_Wing::Set_Bone_Data(CModel* pModel)
{
	for (_uint i = 50; i < 55; ++i)
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

void CWeapon_Dragon_Left_Wing::Enter_Hit_Terrain()
{
	m_pEffect_WingAttack_TraceRocks->Play(m_Bones[0].vPosition);
}

HRESULT CWeapon_Dragon_Left_Wing::Initialize_Prototype()
{
	std::lock_guard<std::mutex> lock(mtx);

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

	if (nullptr == pGameInstance->Find_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceRocks")))
	{
		if (FAILED(pGameInstance->Add_Prototype(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceRocks")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/BoneDragon/WingAttack/TraceRocks/"), LEVEL_SANCTUM))))
		{
			__debugbreak();
			ENDINSTANCE;
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CWeapon_Dragon_Left_Wing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransform->Set_RigidBody(m_pRigidBody);
	m_pTransform->Set_Speed(10.f);
	m_pTransform->Set_RotationSpeed(XMConvertToRadians(90.f));

	m_CollisionRequestDesc.eType = CEnemy::ATTACK_LIGHT;
	m_CollisionRequestDesc.iDamage = 10;
	m_CollisionRequestDesc.pEnemyTransform = m_pTransform;

	m_pEffect_WingAttack_TraceDarkCloud->Disable();
	m_pEffect_WingAttack_TraceRocks->Disable();

	return S_OK;
}

void CWeapon_Dragon_Left_Wing::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	//ADD_IMGUI([&] { Tick_Imgui(fTimeDelta); });
#endif // _DEBUG
	__super::Tick(fTimeDelta);

	for (auto& Desc : m_Bones)
	{
		_float4x4 BoneWorldMatrix = *Desc.pCombinedTransformationMatrix * 
			m_ParentMatrixDesc.PivotMatrix * 
			*m_ParentMatrixDesc.pParentWorldMatrix;
		Desc.vPosition = BoneWorldMatrix.Translation();
	}

	// 날개 공격 이펙트 달기.
	m_pEffect_WingAttack_TraceDarkCloud->Get_Transform()->Set_Position(m_Bones[0].vPosition);
	m_pEffect_WingAttack_TraceRocks->Get_Transform()->Set_Position(m_Bones[0].vPosition);
}

#ifdef _DEBUG
void CWeapon_Dragon_Left_Wing::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_DebugGroup(m_pRigidBody);
}
void CWeapon_Dragon_Left_Wing::Tick_Imgui(_float fTimeDelta)
{
	CImGui_Manager::NextWindow_LeftBottom();
	ImGui::Begin("testdsf");
	ImGui::DragInt("dskjfsiojcv", &test, 0.01f, 0, 3);
	ImGui::End();
}
#endif // _DEBUG

HRESULT CWeapon_Dragon_Left_Wing::Add_Components()
{
	/* For.Com_RigidBody */
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = true;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.vOffsetRotation = XMQuaternionRotationRollPitchYaw(0.f, 0.f, 0.f);
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
		MSG_BOX("[CWeapon_Dragon_Left_Wing] Failed Initialize : Com_RigidBody");
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

	if (FAILED(CComposite::Add_Component(LEVEL_SANCTUM, TEXT("Prototype_GameObject_Particle_WingAttack_TraceRocks"),
		TEXT("Com_Particle_WingAttack_TraceRocks"), reinterpret_cast<CComponent**>(&m_pEffect_WingAttack_TraceRocks))))
		throw TEXT("Com_Particle_WingAttack_TraceRocks");

	return S_OK;
}

CWeapon_Dragon_Left_Wing* CWeapon_Dragon_Left_Wing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Dragon_Left_Wing* pInstance = New CWeapon_Dragon_Left_Wing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CWeapon_Dragon_Left_Wing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CWeapon_Dragon_Left_Wing* CWeapon_Dragon_Left_Wing::Clone(void* pArg)
{
	CWeapon_Dragon_Left_Wing* pInstance = New CWeapon_Dragon_Left_Wing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CWeapon_Dragon_Left_Wing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon_Dragon_Left_Wing::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBody);
	Safe_Release(m_pEffect_WingAttack_TraceDarkCloud);
	Safe_Release(m_pEffect_WingAttack_TraceRocks);
#ifdef _DEBUG
	Safe_Release(m_pRenderer);
#endif // _DEBUG
}
