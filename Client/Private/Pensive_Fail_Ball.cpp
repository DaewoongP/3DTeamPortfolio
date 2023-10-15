#include "Pensive_Fail_Ball.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "ParticleSystem.h"

CPensive_Fail_Ball::CPensive_Fail_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CPensive_Fail_Ball::CPensive_Fail_Ball(const CPensive_Fail_Ball& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CPensive_Fail_Ball::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Distotion_Illusion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Distotion_Illusion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Distotion_Illusion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Flash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Flash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Flash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Ground_Crack")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Ground_Crack")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Ground_Crack/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Stone_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Stone_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Stone_Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Charge")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Orb_Charge")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/OrbThrow/Orb_Charge/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Foot")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Foot")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Dust_Foot/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_GlowBall")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_GlowBall")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Pensive_GlowBall/Pensive_Glow_Ball.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CPensive_Fail_Ball::Initialize(void* pArg)
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

void CPensive_Fail_Ball::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPensive_Fail_Ball::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPensive_Fail_Ball::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CPensive_Fail_Ball::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CPensive_Fail_Ball::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CPensive_Fail_Ball::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fDyingTimer = 1000.f;
	m_ParticleVec[EFFECT_STATE_HIT][2]->Get_ShapeModuleRef().vLength.y = 0;
	m_ParticleVec[EFFECT_STATE_HIT][2]->Get_VelocityOverLifetimeModuleRef().vLinearMin.y = 5.f;
	m_ParticleVec[EFFECT_STATE_HIT][2]->Get_VelocityOverLifetimeModuleRef().vLinearMax.y = 5.f;
	return S_OK;
}

void CPensive_Fail_Ball::Ready_Begin()
{
	__super::Ready_Begin();
}

void CPensive_Fail_Ball::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
	m_pMeshEffect->Play(m_CurrentWeaponMatrix.Translation());
}

void CPensive_Fail_Ball::Ready_CastMagic()
{
	m_ParticleVec[EFFECT_STATE_WAND][0]->Disable();
	m_pTransform->Set_Position(m_vStartPosition);
	m_fTimeScalePerDitance = 100 / _float3(m_vEndPosition - m_vStartPosition).Length();
	__super::Ready_CastMagic();
}

void CPensive_Fail_Ball::Ready_Dying()
{
	m_pMeshEffect->SetJustActionStop(true);
	for (int i = 0; i < m_TrailVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_TrailVec[EFFECT_STATE_HIT].data()[i]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
	}
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		if(i==2||i==4)
			continue;
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Play(m_pMeshEffect->Get_Transform()->Get_Position());
	}
	if (!m_isHeavyChange)
	{
		m_ParticleVec[EFFECT_STATE_HIT].data()[2]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_HIT].data()[2]->Play(m_pMeshEffect->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_HIT].data()[4]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_HIT].data()[4]->Play(m_pMeshEffect->Get_Transform()->Get_Position());
	}
	else 
	{
		for (int j = 0; j < m_ParticleVec[EFFECT_STATE_WAND].size(); j++)
		{
			m_ParticleVec[EFFECT_STATE_WAND].data()[j]->Disable();
		}
		m_pMeshEffect->Stop();
	}
	_float3 vAxis = _float3(1, 1, 1);

	vAxis.Normalize();
	BEGININSTANCE;
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_TYPE)0,
		(CCamera_Manager::SHAKE_AXIS)3,
		(CEase::EASE)0,
		20,
		0.6f,
		0.2f,
		(CCamera_Manager::SHAKE_POWER)1,
		vAxis);
	pGameInstance->Add_InstanceLight(m_pMeshEffect->Get_Transform()->Get_Position(), 60.f, 0.6f, m_vLightColor);
	ENDINSTANCE;
}

void CPensive_Fail_Ball::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CPensive_Fail_Ball::Tick_DrawMagic(_float fTimeDelta)
{
	m_pMeshEffect->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_WAND][0]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
}

void CPensive_Fail_Ball::Tick_CastMagic(_float fTimeDelta)
{
	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
		m_pMeshEffect->Get_Transform()->Set_Position(m_pTransform->Get_Position());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CPensive_Fail_Ball::Tick_Dying(_float fTimeDelta)
{
	m_ParticleVec[EFFECT_STATE_HIT][2]->Get_ShapeModuleRef().vLength.y += fTimeDelta * 5;
	
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

HRESULT CPensive_Fail_Ball::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Charge"),
		TEXT("Com_Orb_Charge"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Charge)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(6);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Distotion_Illusion"),
		TEXT("Com_Distotion_Illusion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Distotion_Illusion)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Flash"),
		TEXT("Com_Flash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Flash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Ground_Crack"),
		TEXT("Com_Ground_Crack"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Ground_Crack)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Splash"),
		TEXT("Com_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Splash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Stone_Splash"),
		TEXT("Com_Stone_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][4]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Stone_Splash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Dust_Foot"),
		TEXT("Com_Dust_Foot"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][5]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Dust_Foot)");
		__debugbreak();
		return E_FAIL;
	}
	
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Pensive_GlowBall"),
		TEXT("Com_Pensive_GlowBall"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Pensive_GlowBall)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPensive_Fail_Ball::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(2.f);
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

CPensive_Fail_Ball* CPensive_Fail_Ball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CPensive_Fail_Ball* pInstance = New CPensive_Fail_Ball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPensive_Fail_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPensive_Fail_Ball::Clone(void* pArg)
{
	CPensive_Fail_Ball* pInstance = New CPensive_Fail_Ball(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Fail_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPensive_Fail_Ball::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
