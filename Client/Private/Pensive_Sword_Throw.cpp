#include "Pensive_Sword_Throw.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "ParticleSystem.h"

CPensive_Sword_Throw::CPensive_Sword_Throw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CPensive_Sword_Throw::CPensive_Sword_Throw(const CPensive_Sword_Throw& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CPensive_Sword_Throw::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Distotion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Distotion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Distotion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Distotion_illusion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Distotion_illusion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Distotion_illusion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Dust_Center")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Dust_Center")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Dust_Center/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Dust_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Dust_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Dust_Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Flash_Screen")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Flash_Screen")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Flash_Screen/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Splash_Back")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Splash_Back")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Splash_Back/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Stone")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Sword_Stone")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Sword/Stone/"), m_iLevel))))
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
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Sword")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Pensive_Sword")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Pensive_Sword/Pensive_Sword.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CPensive_Sword_Throw::Initialize(void* pArg)
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
	m_vLightColor = _float4(1, 1, 1, 1);
	return S_OK;
}

void CPensive_Sword_Throw::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPensive_Sword_Throw::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPensive_Sword_Throw::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CPensive_Sword_Throw::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CPensive_Sword_Throw::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CPensive_Sword_Throw::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	m_fDyingTimer = 1000.f;
	m_ParticleVec[EFFECT_STATE_HIT][8]->Get_ShapeModuleRef().vLength.y = 0;
	m_ParticleVec[EFFECT_STATE_HIT][8]->Get_VelocityOverLifetimeModuleRef().vLinearMin.y = 5.f;
	m_ParticleVec[EFFECT_STATE_HIT][8]->Get_VelocityOverLifetimeModuleRef().vLinearMax.y = 5.f;
	return S_OK;
}

void CPensive_Sword_Throw::Ready_Begin()
{
	__super::Ready_Begin();
}

void CPensive_Sword_Throw::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
	m_pMeshEffect->Play(m_vStartPosition);
	ADD_INCREASE_LIGHT(m_vStartPosition, 3.f, 3.f, m_vLightColor, true, 3.f);
}

void CPensive_Sword_Throw::Ready_CastMagic()
{
	m_vEndPosition = m_CurrentTargetMatrix.Translation();
	m_pTransform->Set_Position(m_vStartPosition);
	m_fTimeScalePerDitance = 100 / _float3(m_vEndPosition - m_vStartPosition).Length();
	__super::Ready_CastMagic();
}

void CPensive_Sword_Throw::Ready_Dying()
{
	for (int i = 0; i < m_TrailVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_TrailVec[EFFECT_STATE_HIT].data()[i]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
	}
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_HIT].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Enable(m_pMeshEffect->Get_Transform()->Get_Position());
		m_ParticleVec[EFFECT_STATE_HIT].data()[i]->Play(m_pMeshEffect->Get_Transform()->Get_Position());
	}

	m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.2f);
	_float3 vAxis = _float3(0, 1, 0);
	vAxis.Normalize();
	BEGININSTANCE;
	pGameInstance->Set_Shake(
		(CCamera_Manager::SHAKE_PRIORITY)0,
		(CCamera_Manager::SHAKE_TYPE)0,
		(CCamera_Manager::SHAKE_AXIS)3,
		(CEase::EASE)0,
		10,
		0.6f,
		0.2f,
		(CCamera_Manager::SHAKE_POWER)1,
		vAxis);
	pGameInstance->Add_InstanceLight(m_CurrentWeaponMatrix.Translation(), 60.f, 1.5f, m_vLightColor);
	ENDINSTANCE;
}

void CPensive_Sword_Throw::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CPensive_Sword_Throw::Tick_DrawMagic(_float fTimeDelta)
{
	_float4x4 DirMatrix = {};
	m_pTransform->Set_Position(m_vStartPosition);
	_float3 vDir = m_pTargetWorldMatrix->Translation() - m_pTransform->Get_Position();
	vDir.Normalize();
	DirMatrix.Look(vDir);
	DirMatrix.m[3][0] = m_pTransform->Get_Position().x;
	DirMatrix.m[3][1] = m_pTransform->Get_Position().y;
	DirMatrix.m[3][2] = m_pTransform->Get_Position().z;
	m_pMeshEffect->Get_Transform()->Set_WorldMatrix(DirMatrix);
}

void CPensive_Sword_Throw::Tick_CastMagic(_float fTimeDelta)
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

void CPensive_Sword_Throw::Tick_Dying(_float fTimeDelta)
{
	m_ParticleVec[EFFECT_STATE_HIT][8]->Get_ShapeModuleRef().vLength.y += fTimeDelta * 5;

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

HRESULT CPensive_Sword_Throw::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_HIT].resize(9);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Distotion"),
		TEXT("Com_Sword_Distotion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Distotion)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Distotion_illusion"),
		TEXT("Com_Sword_Distotion_illusion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Distotion_illusion)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Dust_Center"),
		TEXT("Com_Sword_Dust_Center"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Dust_Center)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Dust_Splash"),
		TEXT("Com_Sword_Dust_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Dust_Splash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Flash_Screen"),
		TEXT("Com_Sword_Flash_Screen"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][4]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Flash_Screen)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Splash"),
		TEXT("Com_Sword_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][5]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Splash)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Splash_Back"),
		TEXT("Com_Sword_Splash_Back"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][6]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Splash_Back)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Sword_Stone"),
		TEXT("Com_Sword_Stone"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][7]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Sword_Stone)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Orb_Ground_Crack"),
		TEXT("Com_Orb_Ground_Crack"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][8]))))
	{
		MSG_BOX("Failed Add_GameObject : (Particle_Pensive_Orb_Ground_Crack)");
		__debugbreak();
		return E_FAIL;
	}

	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Pensive_Sword"),
		TEXT("Com_Pensive_Sword"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Pensive_Sword)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPensive_Sword_Throw::Add_RigidBody()
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

CPensive_Sword_Throw* CPensive_Sword_Throw::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CPensive_Sword_Throw* pInstance = New CPensive_Sword_Throw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPensive_Sword_Throw");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPensive_Sword_Throw::Clone(void* pArg)
{
	CPensive_Sword_Throw* pInstance = New CPensive_Sword_Throw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Sword_Throw");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPensive_Sword_Throw::Free()
{
	__super::Free();
	if (m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
