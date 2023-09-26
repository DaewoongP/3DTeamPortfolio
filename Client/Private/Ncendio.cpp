#include "Ncendio.h"
#include "GameInstance.h"
#include "Engine_Function.h"

CNcendio::CNcendio(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMagicBall(pDevice, pContext)
{
}

CNcendio::CNcendio(const CNcendio& rhs)
	: CMagicBall(rhs)
{
}

HRESULT CNcendio::Initialize_Prototype(_uint iLevel)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iLevel = iLevel;
	BEGININSTANCE;

	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Ncendio/BurnTarget/"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Ncendio/FireCircleBoom/"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_SmokeCloud_Particle")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_SmokeCloud_Particle")
			, CParticleSystem::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/ParticleData/Ncendio/SmokeCloud/"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}
	if (nullptr == pGameInstance->Find_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireRing_MeshEffect")))
	{
		if (FAILED(pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_FireRing_MeshEffect")
			, CMeshEffect::Create(m_pDevice, m_pContext, TEXT("../../Resources/GameData/MeshEffectData/Ncendio/FireRing.ME"), m_iLevel))))
		{
			ENDINSTANCE;
			__debugbreak();
			return E_FAIL;
		}
	}

	ENDINSTANCE;
	return S_OK;
}

HRESULT CNcendio::Initialize(void* pArg)
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

	if (FAILED(Add_Effect()))
	{
		MSG_BOX("Failed Player Add_Effect");

		return E_FAIL;
	}

	m_pSmokeCloudEffect->Disable();
	m_pFireCircleBoomEffect->Disable();
	m_pBurnTargetEffect->Disable();

	m_pSmokeCloudEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pFireCircleBoomEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pBurnTargetEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));
	m_pFireRingMeshEffect->Get_Transform()->Set_Scale(_float3(2.f, 2.f, 2.f));

	return S_OK;
}

void CNcendio::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float3 vWandPosition = _float4x4(m_WeaponOffsetMatrix * (*m_pWeaponMatrix)).Translation();
	//m_pWandTrailEffect->Get_Transform()->Set_Position(vWandPosition);
}

void CNcendio::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CNcendio::OnCollisionEnter(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionEnter(CollisionEventDesc);
	//몹이랑 충돌했으면?
	if (wcsstr(CollisionEventDesc.pOtherCollisionTag, TEXT("Enemy_Body")) != nullptr)
	{
		Set_MagicBallState(MAGICBALL_STATE_DYING);
	}

}

void CNcendio::OnCollisionStay(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionStay(CollisionEventDesc);
}

void CNcendio::OnCollisionExit(COLLEVENTDESC CollisionEventDesc)
{
	__super::OnCollisionExit(CollisionEventDesc);
}

HRESULT CNcendio::Reset(MAGICBALLINITDESC& InitDesc)
{
	__super::Reset(InitDesc);

	m_pSmokeCloudEffect->Disable();
	m_pFireCircleBoomEffect->Disable();
	m_pBurnTargetEffect->Disable();
	m_fLerpAcc = 0.0f;
	m_isSmoke = false;
	m_isSmokeStart = true;
	return S_OK;
}

void CNcendio::Ready_Begin()
{
	m_pTransform->Set_Position(m_MagicBallDesc.vStartPosition);
}

void CNcendio::Ready_DrawMagic()
{
}

void CNcendio::Ready_CastMagic()
{
	// 인센디오 원형 불꽃을 타겟 포지션보다 살짝 낮춘다.
	_float3 vOffsetPosition = m_MagicBallDesc.vStartPosition;
	vOffsetPosition.y -= 0.5f;

	// 맞춘 대상보다 텍스처가 더 앞으로 나오게 조정한다.
	_float3 vBurnTargetOffset = m_MagicBallDesc.vStartPosition - m_vTargetPosition;
	vBurnTargetOffset.Normalize();
	vBurnTargetOffset *= 0.5f;

	// 재생
	m_pBurnTargetEffect->Play(m_vTargetPosition + vBurnTargetOffset);
	m_pFireCircleBoomEffect->Play(vOffsetPosition);
	m_pSmokeCloudEffect->Play(vOffsetPosition);
	m_pFireRingMeshEffect->Play(vOffsetPosition);
}

void CNcendio::Ready_Dying()
{

}

void CNcendio::Tick_Begin(_float fTimeDelta)
{
	//인센디오는 비긴 없습니다.
	Do_MagicBallState_To_Next();
}

void CNcendio::Tick_DrawMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CNcendio::Tick_CastMagic(_float fTimeDelta)
{
	Do_MagicBallState_To_Next();
}

void CNcendio::Tick_Dying(_float fTimeDelta)
{
	if(!m_pFireCircleBoomEffect->IsEnable())
		Do_MagicBallState_To_Next();
}

HRESULT CNcendio::Add_Components()
{
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireCircleBoom_Particle")
		, TEXT("Com_FireCircleBoom"), (CComponent**)&m_pFireCircleBoomEffect), E_FAIL);
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_BurnTarget_Particle")
		, TEXT("Com_BurnTarget"), (CComponent**)&m_pBurnTargetEffect), E_FAIL);
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_SmokeCloud_Particle")
		, TEXT("Com_SmokeCloud"), (CComponent**)&m_pSmokeCloudEffect), E_FAIL);
	FAILED_CHECK_RETURN(CComposite::Add_Component(m_iLevel, TEXT("Prototype_GameObject_FireRing_MeshEffect")
		, TEXT("Com_FireRing"), (CComponent**)&m_pFireRingMeshEffect), E_FAIL);

	return S_OK;
}

HRESULT CNcendio::Add_Effect()
{
	return S_OK;
}

HRESULT CNcendio::Add_RigidBody()
{
	CRigidBody::RIGIDBODYDESC RigidBodyDesc;
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isTrigger = true;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestitution = 0.f;
	PxSphereGeometry SphereGeometry = PxSphereGeometry(3.f);
	RigidBodyDesc.pGeometry = &SphereGeometry;
	RigidBodyDesc.eConstraintFlag = CRigidBody::AllRot;
	RigidBodyDesc.vDebugColor = _float4(1.f, 0.f, 0.f, 1.f);
	RigidBodyDesc.vInitPosition = _float3(0.f, 0.f, 0.f);
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerObject = this;
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

CNcendio* CNcendio::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	CNcendio* pInstance = New CNcendio(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iLevel)))
	{
		MSG_BOX("Failed to Created CNcendio");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNcendio::Clone(void* pArg)
{
	CNcendio* pInstance = New CNcendio(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CNcendio");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNcendio::Free()
{
	__super::Free();
	if (true == m_isCloned)
	{
		Safe_Release(m_pSmokeCloudEffect);
		Safe_Release(m_pFireCircleBoomEffect);
		Safe_Release(m_pBurnTargetEffect);
		Safe_Release(m_pFireRingMeshEffect);
	}
}

