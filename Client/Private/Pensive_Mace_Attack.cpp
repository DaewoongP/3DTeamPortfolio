#include "Pensive_Mace_Attack.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "ParticleSystem.h"

CPensive_Mace_Attack::CPensive_Mace_Attack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CPensive_Mace_Attack::CPensive_Mace_Attack(const CPensive_Mace_Attack& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CPensive_Mace_Attack::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Distotion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Distotion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Distotion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Dust")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Dust")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Dust/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Fire")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Fire")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Fire/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Flash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Flash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Flash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Mob_Distotion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Mob_Distotion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Mob_Distotion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Stone_Rain")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Beast_Stone_Rain")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Beast/Stone_Rain/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Hurray")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Hurray")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Pensive_Hurray/Distotion.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Mace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Mace")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Pensive_Mace/Pensive_Mace.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CPensive_Mace_Attack::Initialize(void* pArg)
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
	m_pTransform->Set_Speed(100);
	m_vLightColor = _float4(1, 0, 0, 1);
	return S_OK;
}

void CPensive_Mace_Attack::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPensive_Mace_Attack::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPensive_Mace_Attack::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CPensive_Mace_Attack::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CPensive_Mace_Attack::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CPensive_Mace_Attack::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fDyingTimer = 1000.f;
	return S_OK;
}

void CPensive_Mace_Attack::Ready_Begin()
{
	__super::Ready_Begin();
	BEGININSTANCE;
	_float3 vAxis = _float3(0, 1, 0);
	vAxis.Normalize();
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_PRIORITY)0,
		(CCamera_Manager::SHAKE_TYPE)0,
		(CCamera_Manager::SHAKE_AXIS)3,
		(CEase::EASE)0,
		10,
		4.f,
		0.05f,
		(CCamera_Manager::SHAKE_POWER)0,
		vAxis);
	ENDINSTANCE;
}

void CPensive_Mace_Attack::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
	m_pMeshEffect->Play(m_CurrentWeaponMatrix.Translation());
	m_pMeshEffect->Get_Transform()->Set_WorldMatrix(m_CurrentWeaponMatrix);
}

void CPensive_Mace_Attack::Ready_CastMagic()
{
	m_pTransform->Set_Position(m_vStartPosition);
	m_fTimeScalePerDitance = 100 / _float3(m_vEndPosition - m_vStartPosition).Length();
	__super::Ready_CastMagic();
}

void CPensive_Mace_Attack::Ready_Dying()
{
	m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.1f);
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Enable((_float4x4(XMMatrixTranslation(0, 2, 0)) * m_pMeshEffect->Get_Transform()->Get_WorldMatrix()).Translation());
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Play((_float4x4(XMMatrixTranslation(0, 2, 0)) * m_pMeshEffect->Get_Transform()->Get_WorldMatrix()).Translation());
	}
	m_pTransform->Set_Position((_float4x4(XMMatrixTranslation(0, 2, 0)) * m_pMeshEffect->Get_Transform()->Get_WorldMatrix()).Translation());
	m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.2f);
	_float3 vAxis = _float3(0, 1, 0);
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
	pGameInstance->Add_InstanceLight(m_CurrentWeaponMatrix.Translation(), 60.f, 0.6f, m_vLightColor);
	ENDINSTANCE;
}

void CPensive_Mace_Attack::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CPensive_Mace_Attack::Tick_DrawMagic(_float fTimeDelta)
{
	m_pMeshEffect->Get_Transform()->Set_WorldMatrix(_float4x4(XMMatrixScaling(3, 3, 3)) * 
		_float4x4(XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians (-90.f), 0.f, XMConvertToRadians(90.f)))) * m_CurrentWeaponMatrix);
}

void CPensive_Mace_Attack::Tick_CastMagic(_float fTimeDelta)
{
	m_pTransform->Set_Position((_float4x4(XMMatrixTranslation(0, 2, 0)) * m_pMeshEffect->Get_Transform()->Get_WorldMatrix()).Translation());
	m_pMeshEffect->Get_Transform()->Set_WorldMatrix(_float4x4(XMMatrixScaling(3, 3, 3)) *
		_float4x4(XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-90.f), 0.f, XMConvertToRadians(90.f)))) * m_CurrentWeaponMatrix);
	Do_MagicBallState_To_Next();
}

void CPensive_Mace_Attack::Tick_Dying(_float fTimeDelta)
{
	m_pTransform->Set_Position((_float4x4(XMMatrixTranslation(0, 2, 0)) * m_pMeshEffect->Get_Transform()->Get_WorldMatrix()).Translation());
	m_pMeshEffect->Get_Transform()->Set_WorldMatrix(_float4x4(XMMatrixScaling(3, 3, 3)) *
		_float4x4(XMMatrixRotationQuaternion(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(-90.f), 0.f, XMConvertToRadians(90.f)))) * m_CurrentWeaponMatrix);
	
	_bool isAlive = { false };
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		if (m_ParticleVec[EFFECT_STATE_HIT].data()[i]->IsEnable())
		{
			isAlive = true;
			break;
		}
	}

	if (!isAlive)
		Do_MagicBallState_To_Next();
}

HRESULT CPensive_Mace_Attack::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Stone_Rain"),
		TEXT("Com_Beast_Stone_Rain"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Stone_Rain)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(6);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Distotion"),
		TEXT("Com_Beast_Distotion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Distotion)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Dust"),
		TEXT("Com_Beast_Dust"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Dust)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Fire"),
		TEXT("Com_Beast_Fire"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Fire)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Flash"),
		TEXT("Com_Beast_Flash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Flash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Mob_Distotion"),
		TEXT("Com_Mob_Distotion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][4]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Mob_Distotion)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Beast_Splash"),
		TEXT("Com_Beast_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][5]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Beast_Splash)");
		__debugbreak();
		return E_FAIL;
	}
	
	
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Pensive_Mace"),
		TEXT("Com_Pensive_Mace"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Pensive_Mace)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPensive_Mace_Attack::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
	RigidBodyDesc.eThisCollsion = COL_MAGIC;
	RigidBodyDesc.eCollisionFlag = m_eCollisionFlag;
	strcpy_s(RigidBodyDesc.szCollisionTag, MAX_PATH, "Magic_Ball");

	/* Com_RigidBody */
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBody), &RigidBodyDesc)))
	{
		MSG_BOX("Failed CTest_Player Add_Component : (Com_RigidBody)");
		return E_FAIL;
	}

	return S_OK;
}

CPensive_Mace_Attack* CPensive_Mace_Attack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CPensive_Mace_Attack* pInstance = New CPensive_Mace_Attack(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPensive_Mace_Attack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPensive_Mace_Attack::Clone(void* pArg)
{
	CPensive_Mace_Attack* pInstance = New CPensive_Mace_Attack(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Mace_Attack");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPensive_Mace_Attack::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
