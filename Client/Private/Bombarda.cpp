#include "Bombarda.h"
#include "GameInstance.h"
#include "Engine_Function.h"

#include "Trail.h"
#include "ParticleSystem.h"

CBombarda::CBombarda(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CBombarda::CBombarda(const CBombarda& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CBombarda::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype(iLevel)))
		return E_FAIL;
	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Trail")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Diffindo_Trail")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/Diffindo/Diffindo.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect")
			, CTrail::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/TrailData/BasicTrail/BasicTrail.trail"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Reverse")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Reverse")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Hit_Reverse/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Hit_Spread_01/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle01")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle01")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Hit_Spread_Twinkle01/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle02")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle02")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Hit_Spread_Twinkle02/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Ball")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Ball")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Main_Ball/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_eye")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_eye")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Main_Cast_eye/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Flare")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Flare")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Main_Cast_Flare/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Rainbow")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Rainbow")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Main_Cast_Rainbow/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Wand_Twinkle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_Wand_Twinkle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Bombarda/Wand_Twinkle/"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_MeshEffect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_Bombarda_MeshEffect")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Bombarda/Bombarda.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			return E_FAIL;
		}
	}
	ENDINSTANCE;

	return S_OK;
}

HRESULT CBombarda::Initialize(void* pArg)
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
	m_pTransform->Set_Speed(30);

	Ready_Shake(60.0f, 2.0f, 0.04f);

	return S_OK;
}

void CBombarda::Tick(_float fTimeDelta)
{
	if (m_pTargetWorldMatrix != nullptr)
		m_CurrentTargetMatrix = /*(*m_pTargetOffsetMatrix) * */(*m_pTargetWorldMatrix);

	if (m_pWeaponWorldMatrix != nullptr)
		m_CurrentWeaponMatrix = (*m_pWeaponOffsetMatrix) * (*m_pWeaponWorldMatrix);
	Tick_MagicBall_State(fTimeDelta);
	CGameObject::Tick(fTimeDelta);
}

void CBombarda::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBombarda::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//¸÷ÀÌ¶û Ãæµ¹ÇßÀ¸¸é?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		m_pRenderer->Set_ScreenRadial(true, 0.2f, 0.2f);

#pragma region Ä«¸Þ¶ó ½¦ÀÌÅ©
		BEGININSTANCE;

		//pGameInstance->Set_SlowTime(TEXT("MainTimer"), 0.2f, 0.05f);

		_float3 vRandomAxis = *pGameInstance->Get_CamRight();

		_float3 vLook = *pGameInstance->Get_CamLook();

		_float fRandomRadian = _float(rand() % 30);

		fRandomRadian *= _float((rand() % 2) - 1);

		vRandomAxis = XMVector3TransformNormal(vRandomAxis, XMMatrixRotationAxis(vLook, XMConvertToRadians(fRandomRadian)));

		pGameInstance->Set_Shake(
			CCamera_Manager::SHAKE_PRIORITY_2,
			CCamera_Manager::SHAKE_TYPE_TRANSLATION,
			CCamera_Manager::SHAKE_AXIS_SET,
			CEase::OUT_QUAD,
			10.0f,
			2.0f,
			Shake_Power(CollisionEventDesc.pOtherTransform->Get_Position()),
			CCamera_Manager::SHAKE_POWER_DECRECENDO,
			vRandomAxis);

		ENDINSTANCE;
#pragma endregion
	}
}

void CBombarda::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CBombarda::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CBombarda::Reset(MAGICBALLINITDESC& InitDesc)
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

void CBombarda::Ready_Begin()
{
	__super::Ready_Begin();
}

void CBombarda::Ready_DrawMagic()
{
	__super::Ready_DrawMagic();
}

void CBombarda::Ready_CastMagic()
{
	Ready_StraightMove(m_TrailVec[EFFECT_STATE_MAIN][0]);
	
	__super::Ready_CastMagic();
}

void CBombarda::Ready_Dying()
{
	ADD_DECREASE_LIGHT(m_vEndPosition, 80.f, 0.2f, m_vLightColor);
	m_pMeshEffect->Play(m_vEndPosition);
	__super::Ready_Dying();
}

void CBombarda::Tick_Begin(_float fTimeDelta)
{
	__super::Tick_Begin(fTimeDelta);
}

void CBombarda::Tick_DrawMagic(_float fTimeDelta)
{
	__super::Tick_DrawMagic(fTimeDelta);
}

void CBombarda::Tick_CastMagic(_float fTimeDelta)
{
	__super::Tick_CastMagic(fTimeDelta);

	m_ParticleVec[EFFECT_STATE_MAIN][1]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN][2]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());
	m_ParticleVec[EFFECT_STATE_MAIN][3]->Get_Transform()->Set_Position(m_CurrentWeaponMatrix.Translation());

	if (m_fLerpAcc != 1)
	{
		m_fLerpAcc += fTimeDelta / m_fLifeTime * m_fTimeScalePerDitance;
		if (m_fLerpAcc > 1)
			m_fLerpAcc = 1;
		m_TrailVec[EFFECT_STATE_MAIN][0]->Spline_Move(m_vSplineLerp[0], m_vStartPosition, m_vEndPosition, m_vSplineLerp[1], m_fLerpAcc);
		m_pTransform->Set_Position(XMVectorLerp(m_vStartPosition, m_vEndPosition, m_fLerpAcc));
		m_ParticleVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Set_Position(m_pTransform->Get_Position());
		m_pLight->Set_Position(m_TrailVec[EFFECT_STATE_MAIN][0]->Get_Transform()->Get_Position().TransCoord());
	}
	else
	{
		Do_MagicBallState_To_Next();
	}
}

void CBombarda::Tick_Dying(_float fTimeDelta)
{
	__super::Tick_Dying(fTimeDelta);
}

HRESULT CBombarda::Add_Components()
{
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_MeshEffect"),
		TEXT("Com_Mesh_Effect"), reinterpret_cast<CComponent**>(&m_pMeshEffect))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_MeshEffect)");
		__debugbreak();
		return E_FAIL;
	}

	m_TrailVec[EFFECT_STATE_WAND].resize(1);
	m_ParticleVec[EFFECT_STATE_WAND].resize(1);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Diffindo_Trail"),
		TEXT("Com_Wand_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Diffindo_Trail)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Wand_Twinkle"),
		TEXT("Com_Wand_Twinkle"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_WAND][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Wand_Twinkle)");
		__debugbreak();
		return E_FAIL;
	}

	
	m_TrailVec[EFFECT_STATE_MAIN].resize(1);
	m_ParticleVec[EFFECT_STATE_MAIN].resize(4);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_BasicCast_Wand_Trail_Effect"),
		TEXT("Com_MAIN_Trail"), reinterpret_cast<CComponent**>(&m_TrailVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_BasicCast_Wand_Trail_Effect)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Main_Ball"),
		TEXT("Com_Main_Ball"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Main_Ball)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Main_Cast_eye"),
		TEXT("Com_Main_Cast_eye"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Main_Cast_eye)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Flare"),
		TEXT("Com_Main_Cast_Flare"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Main_Cast_Flare)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Main_Cast_Rainbow"),
		TEXT("Com_Main_Cast_Rainbow"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_MAIN][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Main_Cast_Rainbow)");
		__debugbreak();
		return E_FAIL;
	}

	m_ParticleVec[EFFECT_STATE_HIT].resize(4);
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Hit_Reverse"),
		TEXT("Com_Hit_Reverse"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][0]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Hit_Reverse)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_01"),
		TEXT("Com_Hit_Spread_01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][1]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Hit_Spread_01)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle01"),
		TEXT("Com_Hit_Spread_Twinkle01"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][2]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Hit_Spread_Twinkle01)");
		__debugbreak();
		return E_FAIL;
	}
	if (FAILED(CComposite::Add_Component(LEVEL_STATIC, TEXT("Prototype_GameObject_Bombarda_Hit_Spread_Twinkle02"),
		TEXT("Com_Hit_Spread_Twinkle02"), reinterpret_cast<CComponent**>(&m_ParticleVec[EFFECT_STATE_HIT][3]))))
	{
		MSG_BOX("Failed Add_GameObject : (Prototype_GameObject_Bombarda_Hit_Spread_Twinkle02)");
		__debugbreak();
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBombarda::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.vInitPosition = m_pTransform->Get_Position();
	RigidBodyDesc.vOffsetPosition = _float3(0.f, 0.0f, 0.f);
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxBoxGeometry BoxGeometry = PxBoxGeometry(2.f,0.1f,2.f);
	//PxSphereGeometry SphereGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &BoxGeometry;
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

CBombarda* CBombarda::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iLevel)
{
	CBombarda* pInstance = New CBombarda(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CBombarda");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBombarda::Clone(void* pArg)
{
	CBombarda* pInstance = New CBombarda(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CBombarda");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBombarda::Free()
{
	__super::Free();

	if (m_isCloned)
	{
		Safe_Release(m_pMeshEffect);
	}
}
