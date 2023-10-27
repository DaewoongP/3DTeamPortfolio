#include "Pensive_Ground_Ball.h"
#include "GameInstance.h"
#include "Engine_Function.h"
#include "ParticleSystem.h"

CPensive_Ground_Ball::CPensive_Ground_Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CPensive_Ground_Ball::CPensive_Ground_Ball(const CPensive_Ground_Ball& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CPensive_Ground_Ball::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Foot")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Foot")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Dust_Foot/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Trace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Dust_Trace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Dust_Trace/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Explosive_Trace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Explosive_Trace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Explosive_Trace/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Flash_Screen")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Flash_Screen")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Flash_Screen/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Foot_Splash")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Foot_Splash")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Foot_Splash/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Glow_Trace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Glow_Trace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Glow_Trace/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Stone_Trace")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Stone_Trace")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Stone_Trace/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Particle_Pensive_Trace_Distotion")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Particle_Pensive_Trace_Distotion")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Monster_Particle/Pensive/Ground/Trace_Distotion/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;
	return S_OK;
}

HRESULT CPensive_Ground_Ball::Initialize(void* pArg)
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
	m_vLightColor = _float4(1, 1, 1, 1);
	m_pTransform->Set_Speed(100);
	return S_OK;
}

void CPensive_Ground_Ball::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPensive_Ground_Ball::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CPensive_Ground_Ball::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
}

void CPensive_Ground_Ball::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CPensive_Ground_Ball::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CPensive_Ground_Ball::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	CLight::LIGHTDESC LightDesc;
	LightDesc.eType = CLight::TYPE_POINT;
	LightDesc.fRange = 5.f;
	m_vLightColor = LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;
	LightDesc.vPos = m_pTransform->Get_Position().TransCoord();
	pGameInstance->Add_Light(LightDesc, &m_pLight);
	Safe_Release(pGameInstance);
	return S_OK;
}

void CPensive_Ground_Ball::Ready_Begin()
{
	ADD_DECREASE_LIGHT(m_vStartPosition, 100.f, 0.6f, m_vLightColor);
	__super::Ready_Begin();
}

void CPensive_Ground_Ball::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CPensive_Ground_Ball::Ready_CastMagic()
{
	m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.1f);
	m_pTransform->Set_Position(m_vStartPosition);
	m_pTransform->LookAt(m_vEndPosition);
	m_fTimeScalePerDitance = 100.f / _float3(m_vEndPosition - m_vStartPosition).Length();
	__super::Ready_CastMagic();
	
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->Enable(m_CurrentWeaponMatrix.Translation());
		m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->Play(m_CurrentWeaponMatrix.Translation());
	}
}

void CPensive_Ground_Ball::Ready_Dying()
{
	ADD_DECREASE_LIGHT(m_pTransform->Get_Position(), 50.f, 2.f, m_vLightColor);
	__super::Ready_Dying();
}

void CPensive_Ground_Ball::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CPensive_Ground_Ball::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
	Do_MagicBallState_To_Next();
}

void CPensive_Ground_Ball::Tick_CastMagic(_float fTimeDelta)
{
	//__super::Tick_CastMagic(fTimeDelta);
	m_fLerpAcc += fTimeDelta / m_fLifeTime;
	//애는 러프 안시키고 그쪽 방향으로 무지성 이동 시켜줄거임.
	
	m_pTransform->Go_Straight(fTimeDelta);
	for (_int i = 0; i < 5; i++)
	{
		m_ParticleVec[EFFECT_STATE_MAIN][i]->Get_Transform()->Set_Position(m_pTransform->Get_Position());
	}

	for (_int i = 5; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
	{
		m_ParticleVec[EFFECT_STATE_MAIN][i]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	}
	m_pLight->Set_Position(m_pTransform->Get_Position().TransCoord());
	
	_float distance =m_pTransform->Get_Speed()* m_fLerpAcc;
	if (distance>100)
	{
		for (_int i = 0; i < 5; i++)
		{
			m_ParticleVec[EFFECT_STATE_MAIN][i]->Stop();
		}

		for (_int i = 5; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
		{
			m_ParticleVec[EFFECT_STATE_MAIN][i]->Stop();
		}
		Safe_Release(m_pLight);
		m_pLight = nullptr;
		Do_MagicBallState_To_Next();
	}
}

void CPensive_Ground_Ball::Tick_Dying(_float fTimeDelta)
{
	_bool isAlive = { false };
	for (int i = 0; i < m_ParticleVec[EFFECT_STATE_MAIN].size(); i++)
	{
		if (m_ParticleVec[EFFECT_STATE_MAIN].data()[i]->IsEnable())
		{
			isAlive = true;
			break;
		}
	}

	if (!isAlive)
		Do_MagicBallState_To_Next();
}

HRESULT CPensive_Ground_Ball::Add_Components()
{
	m_ParticleVec[EFFECT_STATE_MAIN].resize(8);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Trace_Distotion"),
		TEXT("Com_Pensive_Trace_Distotion"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Ball_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Stone_Trace"),
		TEXT("Com_Pensive_Stone_Trace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Dust)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Glow_Trace"),
		TEXT("Com_Pensive_Glow_Trace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Explosive_Trace"),
		TEXT("Com_Pensive_Explosive_Trace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Dust_Trace"),
		TEXT("Com_Pensive_Dust_Trace"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][4]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Foot_Splash"),
		TEXT("Com_Pensive_Foot_Splash"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][5]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Dust_Foot"),
		TEXT("Com_Pensive_Dust_Foot"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][6]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Particle_Pensive_Flash_Screen"),
		TEXT("Com_Pensive_Flash_Screen"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][7]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Cast_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPensive_Ground_Ball::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(1.f);
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

CPensive_Ground_Ball* CPensive_Ground_Ball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CPensive_Ground_Ball* pInstance = New CPensive_Ground_Ball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CPensive_Ground_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPensive_Ground_Ball::Clone(void* pArg)
{
	CPensive_Ground_Ball* pInstance = New CPensive_Ground_Ball(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CPensive_Ground_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPensive_Ground_Ball::Free()
{
	__super::Free();
}
